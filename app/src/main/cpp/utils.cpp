//
// Created by Marco Signoretto on 07/03/2017.
//
#include <iostream>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "utils.h"

cv::Mat mcv::normalize_hist(cv::Mat &hist, const cv::Mat &image){
    //we use float data into normalized histogram because as necessary precision and it uses less memory than double
    float pixels = image.rows*image.cols;
    cv::Mat norm_hist{cv::Mat::zeros(1,hist.cols, CV_32FC1)}; // Create 0 matrix 1 channel and float data

    //Skip continuous control because hist has only a single row that is surely continuous so I have used direct
    //pointer access to increase efficiency
    int* hist_data_ptr = hist.ptr<int>(0);  // hist is of type int
    float * norm_hist_data_ptr = norm_hist.ptr<float>(0); // norm_hist is of type float
    const int* hist_end_ptr = hist_data_ptr + hist.cols;
    while( hist_data_ptr < hist_end_ptr ) {
        *(norm_hist_data_ptr++) = (float)(*(hist_data_ptr++)) / pixels;
    }
    return norm_hist;
}

cv::Mat mcv::generate_hist_image(const cv::Mat &hist, int max_value, string filename){
    cv::Mat hist_image{cv::Mat::zeros(256, 256, CV_8UC1)};  //  Init all black
    for(int i = 0; i < hist.cols; ++i){
        int height = (int)round((float)hist.at<int>(0,i) * (256.0f / (float)max_value)); // truncate to display
        // initially image is completely black, then using fact that (0,0) is at "top, left" I fill with white pixels
        // part of the image that is over the height of the histogram ( this procedure is repeated for each value of
        // the intensity represented into histogram
        for(int j = hist_image.rows-1-height; j >= 0; --j){
            hist_image.at<uchar>(j,i) = 255;  //make white pixel over height value
        }
    }
    cv::imwrite(filename, hist_image);
    return hist_image;
}

cv::Mat mcv::compute_hist(const cv::Mat& image_gray, int& max_value){
    max_value = 0;
    // use 32 bits to store image (worst case 307200 so 16-bit is not sufficient)
    cv::Mat hist{cv::Mat::zeros(1, 256, CV_32SC1)};

    int nRows = image_gray.rows;
    int nCols = image_gray.cols;

    const uchar* p;
    for(int i = 0; i < nRows; ++i) {
        // Iterate on image with pointer of row, this method is less efficient that all pointer iteration but it works
        // also for non continuous images
        p = image_gray.ptr<uchar>(i);
        for (int j = 0; j < nCols; ++j) {
            // get intensity of pixel at i,j position and use its value as index to increase frequency of histogram
            // for that intensity
            int j_index = (int)(p[j]);
            int& hist_value = hist.at<int>(0, j_index);
            hist_value += 1;
            //update max frequency if needed
            if(hist_value > max_value) max_value = hist_value;
        }
    }
    return hist;
}

void mcv::test_normalized_hist(const cv::Mat &normalized_hist){
    float sum = 0.0f;
    for(int i = 0; i < normalized_hist.cols; ++i){
        sum += normalized_hist.at<float>(0,i);
    }
    cout << "Sum: " << sum <<endl;
}

inline cv::Mat mcv::compute_CDF(cv::Mat &norm_hist){
    // not init matrix with 1 channel and float data for the same reason as before
    cv::Mat cum_sum(1,norm_hist.cols, CV_32FC1);
    float res = 0.0f;

    float* norm_hist_data_ptr = norm_hist.ptr<float>(0);
    float* norm_hist_end_ptr = norm_hist_data_ptr + norm_hist.cols;
    float* cum_sum_data_ptr = cum_sum.ptr<float>(0);
    while( norm_hist_data_ptr < norm_hist_end_ptr ) {
        res += *(norm_hist_data_ptr++);
        *(cum_sum_data_ptr++) = res;
    }
    return cum_sum;
}

cv::Mat mcv::compute_cumulative_mean(cv::Mat& norm_hist){
    // not init matrix with 1 channel and float data for the same reason as before
    cv::Mat cum_mean(1,norm_hist.cols, CV_32FC1);
    float partial_sum = 0.0f;

    // Calculate cumulative mean
    float* norm_hist_data_ptr = norm_hist.ptr<float>(0);
    float* norm_hist_end_ptr = norm_hist_data_ptr + norm_hist.cols;
    float* cum_mean_data_ptr = cum_mean.ptr<float>(0);
    float i = 1.0f;
    while( norm_hist_data_ptr < norm_hist_end_ptr ) {
        partial_sum += i * (*(norm_hist_data_ptr++));
        *(cum_mean_data_ptr++) = partial_sum;

        i+= 1.0f;
    }
    return cum_mean;
}

float mcv::between_class_variance(int threshold, const cv::Mat& cum_sum, const cv::Mat& cum_mean){
    assert(cum_sum.cols == cum_mean.cols && "cum_sum and cum_mean differ in size!!");
    const int levels = cum_sum.cols;
    const float& cum_sum_threshold = cum_sum.at<float>(0,threshold);
    return pow(cum_mean.at<float>(0,levels-1) * cum_sum_threshold - cum_mean.at<float>(0,threshold), 2.0f)/
           (cum_sum_threshold * (1.0f - cum_sum_threshold));
}

int mcv::compute_Otsu_thresholding(cv::Mat &norm_hist){
    // init values
    float max = -1.0f;
    int threshold = -1;
    // evaluate CDF
    const cv::Mat cum_sum = mcv::compute_CDF(norm_hist);
    // evaluate cumulative mean
    const cv::Mat cum_mean = mcv::compute_cumulative_mean(norm_hist);
    // try all possible thresholding and keep the best one ( maximize between variance )
    for(int i = 0; i < norm_hist.cols; ++i){
        float betweenVariance = mcv::between_class_variance(i, cum_sum, cum_mean);
        if (betweenVariance > max ){
            max = betweenVariance;
            threshold = i;
        }
    }
    assert(max>=0 && "Max must be greater than zero (negative or wrong norm_hist?)");
    assert(threshold>=0 && "Threshold must be greater than zero (wrong norm_hist?)");
    return threshold;
}

cv::Mat mcv::image_threshold(int threshold, const cv::Mat &image_gray){
    cv::Mat image_threshold(image_gray.rows, image_gray.cols, CV_8UC1);

    auto it = image_gray.begin<uchar>();
    auto it_end = image_gray.end<uchar>();
    auto it_threshold = image_threshold.begin<uchar>();
    while(it < it_end){
        *(it_threshold++) = ( *(it++) > threshold )?(uchar)255:(uchar)0;
    }
    return image_threshold;
}

void mcv::image_otsu_thresholding(const cv::Mat &image_gray, cv::Mat& image_th) {
    assert(image_gray.channels()==1 && "Invalid channels number");
    //Calculate histogram
    int max_value;
    cv::Mat hist = compute_hist(image_gray, max_value);
    // normalize histogram
    cv::Mat normHist = normalize_hist(hist, image_gray);
    //compute OtsuThresholding
    int threshold = compute_Otsu_thresholding(normHist);
    //generate image from threshold
    image_th = image_threshold(threshold, image_gray);
}

void mcv::compute_rho_theta_plane(const cv::Mat &window_mat, cv::Mat& H, cv::Point2f& best_rho_theta) {
    int max_value = -1;
    int theta_max = 180; // 180 degree of range of theta
    const double step = CV_PI / 180; // step of 1 degree

    // rho max is given by length of diagonal of the window_mat
    int rho_max = (int)round(sqrt((window_mat.rows*window_mat.rows) + (window_mat.cols*window_mat.rows)));
    H = cv::Mat::zeros(2*rho_max,theta_max, CV_32SC1); // init output matrix, CV_8UC1 is not sufficient

    int nRows = window_mat.rows;
    int nCols = window_mat.cols;

    // Iterate over window
    int y,x;
    for( y = 0; y < nRows; ++y) {
        // Iterate on image with pointer of row, this method is less efficient that all pointer iteration but it works
        // also for non continuous images
        const uchar* p = window_mat.ptr<uchar>(y);

        for ( x = 0; x < nCols; ++x) {
            // black is all zero, if use otsu thresholding white is 1 so it uses greater than zero in order to work also
            // with my implementation
            if(p[x] > mcv::BLACK){
                // iteration on theta range
                for(float theta=0.0; theta < CV_PI; theta += step){
                    // add rho max, now the center is in the middle
                    int discrete_r = rho_max + (int)round((float)x * cos(theta) + (float)y * sin(theta));

                    // in degree ( same operation of to_degree with the given step )
                    int discrete_theta = (int)round(theta*(1/step));

                    if(discrete_r >= 0 && discrete_r < H.rows && discrete_theta >= 0 && discrete_theta < H.cols){
                        int& element = H.at<int>(discrete_r,discrete_theta);
                        element += 1;
                        // if new max was found update best_rho_theta
                        if(element > max_value){
                            max_value = element;
                            best_rho_theta.y = discrete_r - rho_max; // remove rho offset used into rho_theta plane
                            best_rho_theta.x = theta;
                        }
                    }
                }
            }
        }
    }
}

void mcv::extract_lines(const cv::Mat &window_mat, cv::Mat& H, std::vector<cv::Point2f>& lines, int min_score) {
    int theta_max = 180; // 180 degree if range of theta
    const double step = CV_PI / 180; // step of 1 degree

    // rho max is given by length of diagonal of the window_mat
    int rho_max = (int)round(sqrt((window_mat.rows*window_mat.rows) + (window_mat.cols*window_mat.rows)));
    H = cv::Mat::zeros(2*rho_max,theta_max, CV_32SC1); // init output matrix, CV_8UC1 is not sufficient

    int nRows = window_mat.rows;
    int nCols = window_mat.cols;

    // Iterate over window
    int y,x;
    for( y = 0; y < nRows; ++y) {
        // Iterate on image with pointer of row, this method is less efficient that all pointer iteration but it works
        // also for non continuous images
        const uchar* p = window_mat.ptr<uchar>(y);

        for ( x = 0; x < nCols; ++x) {
            // black is all zero, if use otsu thresholding white is 1 so use greater than zero in order to work also
            // with my implementation
            if(p[x] > mcv::BLACK){
                // iteration on theta range
                for(float theta=0.0; theta < CV_PI; theta += step){
                    // add rho max, now center is in the middle
                    int discrete_r = rho_max + (int)round((float)x * cos(theta) + (float)y * sin(theta));

                    // in degree ( same operation of to_degree with the given step )
                    int discrete_theta = (int)round(theta*(1/step));

                    if(discrete_r >= 0 && discrete_r < H.rows && discrete_theta >= 0 && discrete_theta < H.cols){
                        int& element = H.at<int>(discrete_r,discrete_theta);
                        element += 1;
                        // if new max was found update best_rho_theta
                        if(element > min_score){
                            lines.push_back(cv::Point2f(theta, discrete_r - rho_max));
                        }
                    }
                }
            }
        }
    }

}

double mcv::to_degree(double radiant) {
    return radiant*(180/CV_PI);
}

void mcv::line(cv::Mat &dest,  double rho, double theta, double origin_x, double origin_y){
    cv::Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = (a*rho)+origin_x, y0 = (b*rho)+origin_y;
    pt1.x = cvRound(x0 + 1000*(-b));
    pt1.y = cvRound(y0 + 1000*(a));
    pt2.x = cvRound(x0 - 1000*(-b));
    pt2.y = cvRound(y0 - 1000*(a));
    cv::line( dest, pt1, pt2, cv::Scalar(255,255,255), 1, cv::LINE_AA);
}

double mcv::m(double theta) {
    return -cos(theta)/sin(theta);
}

double mcv::q(double rho, double theta) {
    return rho/sin(theta);
}

cv::Mat mcv::to_image(cv::Mat &rho_theta_plane) {
    cv::Mat img(rho_theta_plane.rows,rho_theta_plane.cols, CV_8UC1); // init result image

    int nRows = rho_theta_plane.rows;
    int nCols = rho_theta_plane.cols;
    int x,y;
    for(y = 0; y < nRows; ++y) {
        // Iterate on image with pointer of row, this method is less efficient that all pointer iteration but it works
        // also for non continuous images
        const int* p = rho_theta_plane.ptr<int>(y);
        uchar* p_dest = img.ptr<uchar>(y);

        for (x = 0; x < nCols; ++x) {
            // if a value in rho_theta, plane is higher than 255 keep 255 ( for display reason is good ) otherwise it keeps
            // original
            if(p[x] > 255){
                p_dest[x] = 255;
            } else {
                p_dest[x] = (uchar)p[x];
            }
        }
    }
    return img;
}

void mcv::draw_rect(cv::Mat& dst, const std::vector<cv::Point>& rect) {
    cv::rectangle(dst,rect[0],rect[1],cv::Scalar(255,0,255));
}


