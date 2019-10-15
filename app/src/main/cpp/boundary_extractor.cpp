//
// Created by Marco Signoretto on 06/03/2017.
//

#include <iostream>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <opencv2/imgproc.hpp>

#include "boundary_extractor.h"
#include "utils.h"

using namespace mcv;

boundary_extractor::boundary_extractor(const std::string& filename):filename_(filename){
    const cv::Mat image_gray{cv::imread(filename, cv::IMREAD_GRAYSCALE)};

    //compute otsu thresholding
    cv::Mat image;
    mcv::image_otsu_thresholding(image_gray,image);

    // Create image with 1 pixel of padding in order to avoid bounds checking on moore's algorithm
    image_ = cv::Mat::zeros(image.rows+2, image.cols+2, CV_8UC1);

    int nRows = image_.rows;
    int nCols = image_.cols;

    // Copy source image ( image ) into padded image ( image_ )
    int i,j;
    uchar* p;
    const uchar* p2;
    for( i = 1; i < nRows-1; ++i) {
        // Iterate on image with pointer of row, this method is less efficient that all pointer iteration but it works
        // also for non continuous images
        p = image_.ptr<uchar>(i);
        p2 = image.ptr<uchar>(i-1);
        for ( j = 1; j < nCols-1; ++j) {
            p[j] = p2[j-1];
        }
    }
    // decomment if you want visualize threshold image generated
    //cv::imwrite("bacteria_border.png",image_);
}


boundary_extractor::boundary_extractor(const cv::Mat& image_gray, bool compute_threshold):filename_(""){

    assert(image_.channels() == 1 && "Invalid channel number");

    //compute otsu thresholding
    cv::Mat image;
    if(compute_threshold) {
        mcv::image_otsu_thresholding(image_gray, image);
    }else{
        image = image_gray; // soft copy
    };

    // Create image with 1 pixel of padding in order to avoid bounds checking on moore's algorithm
    image_ = cv::Mat::zeros(image.rows+2, image.cols+2, CV_8UC1);

    int nRows = image_.rows;
    int nCols = image_.cols;

    // Copy source image ( image ) into padded image ( image_ )
    int i,j;
    uchar* p;
    const uchar* p2;
    for( i = 1; i < nRows-1; ++i) {
        // Iterate on image with pointer of row, this method is less efficient that all pointer iterations but it works
        // also for non continuous images
        p = image_.ptr<uchar>(i);
        p2 = image.ptr<uchar>(i-1);
        for ( j = 1; j < nCols-1; ++j) {
            p[j] = p2[j-1];
        }
    }
}

void boundary_extractor::find_boundaries(const uchar boundary_color) {
    const uchar other_color = (boundary_color==WHITE)? BLACK : WHITE;
    // clear boundaries in order to recompute all
    boundaries_.clear();
    // this flag is used to skip white pixels that are close each other
    bool valid_next = true;

    assert(image_.channels() == 1 && "Invalid channel number");

    int nRows = image_.rows;
    int nCols = image_.cols;

    int i,j;
    const uchar* p;
    for( i = 1; i < nRows-1; ++i) {
        p = image_.ptr<uchar>(i);
        for ( j = 1; j < nCols-1; ++j) {

            // Condition true when algorithm must consider valid next pixels
            if(!valid_next && (p[j] == other_color)){
                valid_next = true;
            }// If condition true the pixel is a candidate solution for a begin of a boundary
            else if(valid_next && (p[j] == boundary_color)){
                valid_next = false;
                // if it is a valid boundary point, it finds boundary with moore's algorithm and then it adds to boundaries set
                if(is_valid(j,i)) {
                    boundaries_.push_back(moore_algorithm(j, i, boundary_color));
                }
            }
        }
    }
    normalize();
}

inline bool boundary_extractor::is_valid(int x, int y) {
    for(const boundary& b : boundaries_){
        // Use boundary max and min corner to avoid to check all pixels of all boundaries already present,
        // this is very useful to speed-up computation
        if(x <= b.max_x && x >= b.min_x && y <= b.max_y && y >= b.min_y){
            // If (x,y) pixel is into a boundary it isn't valid so it returns false
            if(check_in(b, x, y))return false;
        }
    }
    return true;
}

inline bool boundary_extractor::check_in(const boundary &b, int x, int y) {
    // check if given point is part of boundary points
    for(const cv::Vec2i& item : b.points){
        if((item[0] == x) && (item[1] == y))return true;
    }
    return false;

}

inline boundary boundary_extractor::moore_algorithm(int x, int y, const uchar boundary_color) {
    cv::Vec2i b(-1,-1);
    cv::Vec2i c(-1,-1);
    boundary boundary;
    cv::Vec2i b0(x,y); // b0
    cv::Vec2i c0(x-1,y); // c0

    // Add b0 to pixel set
    boundary.add_item(b0);
    // search next pixel ( if it is false only if b0 is unique pixel into boundary )
    if(search_clockwise(c0, b0, &c, &b, boundary_color)){
        // Iterate over boundary searching in clockwise until reach already b0
        while(b0[0] != b[0] || b0[1] != b[1]){
            boundary.add_item(b);
            search_clockwise(c, b, &c, &b, boundary_color); // use same c and b as input and output ( see search clockwise docs )
        }
    }
    return boundary;
}


inline bool boundary_extractor::search_clockwise(cv::Vec2i& current_c, cv::Vec2i& current_b, cv::Vec2i* c_ptr, cv::Vec2i* b_ptr, const uchar boundary_color) {
    cv::Vec2i& c = *c_ptr;
    cv::Vec2i& b = *b_ptr;
    int iteration = 0; // iteration counter used to detect 1 pixel boundary
    bool found = false;
    int index = find_clock_index(&current_c,&current_b); // Detect clock index from current b and c
    while(!found){
        index = (index+1)%8; // Go to next clock index

        // Different cases to handle clock index, when it finds next white in clockwise, it sets new b and c
        // NB c updated before b because if update b before algorithm doesn't work if current_b == *b_ptr
        switch(index){
            case 0:
                if(image_.at<uchar>(current_b[1]-1,current_b[0]-1) == boundary_color){  // start from pixel over current and it procedes in clockwise order
                    found = true;
                    c[0] = current_b[0]-1;
                    c[1] = current_b[1];
                    b[0] = current_b[0]-1;
                    b[1] = current_b[1]-1;
                }
                break;
            case 1:
                if(image_.at<uchar>(current_b[1]-1,current_b[0]) == boundary_color){
                    found = true;
                    c[0] = current_b[0]-1;
                    c[1] = current_b[1]-1;
                    b[0] = current_b[0];
                    b[1] = current_b[1]-1;
                }
                break;
            case 2:
                if(image_.at<uchar>(current_b[1]-1,current_b[0]+1) == boundary_color){
                    found = true;
                    c[0] = current_b[0];
                    c[1] = current_b[1]-1;
                    b[0] = current_b[0]+1;
                    b[1] = current_b[1]-1;
                }
                break;
            case 3:
                if(image_.at<uchar>(current_b[1],current_b[0]+1) == boundary_color){
                    found = true;
                    c[0] = current_b[0]+1;
                    c[1] = current_b[1]-1;
                    b[0] = current_b[0]+1;
                    b[1] = current_b[1];
                }
                break;
            case 4:
                if(image_.at<uchar>(current_b[1]+1,current_b[0]+1) == boundary_color){
                    found = true;
                    c[0] = current_b[0]+1;
                    c[1] = current_b[1];
                    b[0] = current_b[0]+1;
                    b[1] = current_b[1]+1;
                }
                break;
            case 5:
                if(image_.at<uchar>(current_b[1]+1,current_b[0]) == boundary_color){
                    found = true;
                    c[0] = current_b[0]+1;
                    c[1] = current_b[1]+1;
                    b[0] = current_b[0];
                    b[1] = current_b[1]+1;
                }
                break;
            case 6:
                if(image_.at<uchar>(current_b[1]+1,current_b[0]-1) == boundary_color){
                    found = true;
                    c[0] = current_b[0];
                    c[1] = current_b[1]+1;
                    b[0] = current_b[0]-1;
                    b[1] = current_b[1]+1;
                }
                break;
            case 7:
                if(image_.at<uchar>(current_b[1],current_b[0]-1) == boundary_color){
                    found = true;
                    c[0] = current_b[0]-1;
                    c[1] = current_b[1]+1;
                    b[0] = current_b[0]-1;
                    b[1] = current_b[1];
                }
                break;
            default:
                assert("Impossible case something was wrong");
                break;

        }

        if(iteration > 8)break; // single point, impossible to find next border item
        ++iteration;
    }
    return found;
}

inline int boundary_extractor::find_clock_index(cv::Vec2i* c_ptr, cv::Vec2i* b_ptr) {
    cv::Vec2i& c = *c_ptr;
    cv::Vec2i& b = *b_ptr;
    int index;

    // Simple condition tree to find correct index respect of the structure shows in doc of this function
    if(c[0] < b[0]){
        if(c[1] < b[1]){
            index = 0;
        }else if(c[1] == b[1]){
            index = 7;
        }else{
            index = 6;
        }
    }else if(c[0] == b[0]){
        if(c[1] < b[1]){
            index = 1;
        }else if(c[1] == b[1]){
            index = -1;
        }else{
            index = 5;
        }
    }else{
        if(c[1] < b[1]){
            index = 2;
        }else if(c[1] == b[1]){
            index = 3;
        }else{
            index = 4;
        }
    }
    return index;
}

void boundary_extractor::draw_boundaries(const std::string &dest) {
    cv::Mat fin_img;
    // read original image in grayscale
    cv::Mat image = cv::imread(filename_, CV_LOAD_IMAGE_GRAYSCALE);

    // create a channel vector to produce colored image from
    vector<cv::Mat> channels;
    // set each channel image value ( remain grayscale )
    // channels as classical OpenCV order (bgr)
    // channels[0] => blue
    // channels[1] => green
    // channels[2] => red
    channels.push_back(image.clone());
    channels.push_back(image.clone());
    channels.push_back(image.clone());

    for(boundary& b : boundaries_){
        draw_boundary(image,b,channels); // it draws red line into image respect to boundary pixels
    }

    // it merges channels into color image fin_image
    merge(channels, fin_img);
    cv::imwrite(dest,fin_img);
}

void boundary_extractor::create_boundaries_image(cv::Mat& image) {
    image = cv::Mat::zeros(image_.rows, image_.cols, CV_8UC1); // image is larger of 1 px respect to input

    for(boundary& b : boundaries_){
        draw_boundary(image,b,true); // it draws each boundary
    }

}

void boundary_extractor::draw_boundaries(cv::Mat &image) {
    assert(image.channels() == 3 && "Invalid channel number");

    for(boundary& b : boundaries_){
        draw_boundary(image, b); // it draws red line into image respect to boundary pixels
    }
}

void boundary_extractor::draw_boundaries_corners(cv::Mat& image){

    assert(image.channels() == 3 && "Invalid channel number");

    for(boundary& b : boundaries_){
        draw_corners(image, b); //it draws green dot into image respect to boundary pixels
    }
}

inline void boundary_extractor::draw_boundary(const cv::Mat& image, const boundary &b, vector<cv::Mat>& channels) {
    for(cv::Vec2i v : b.points){
        int j = v[0];
        int i = v[1];
        // it draws only if boundary inside image content
        if(j>=0 && i>=0 && j<image.cols && i<image.rows) {
            // it removes all color from blue and green channel and it adds full color to red channel
            channels[0].at<uchar>(i, j) = 0;
            channels[1].at<uchar>(i, j) = 0;
            channels[2].at<uchar>(i, j) = 255;
        }
    }
}

inline void boundary_extractor::draw_corners(cv::Mat& image, const boundary &b) {

    assert(image.channels() == 3 && "Invalid channel number");

    for(cv::Vec2i v : b.corners) {
        int offset = 1; // Used to make corners visible ( bigger than a pixel )
        int j_origin = v[0];
        int i_origin = v[1];

        // it removes all color from blue and green channel and it adds full color to green channel
        // Corners are drawn with "offset" as extra size to make them more visible
        for (int i = i_origin-offset; i<=i_origin+offset; ++i) {
            if (i >= 0 && i < image.rows) {
                for (int j = j_origin - offset; j <= j_origin + offset; ++j) {
                    if (j >= 0 && j < image.cols) {
                        cv::Vec3b &intensity = image.at<cv::Vec3b>(i, j);

                        intensity[0] = 0;
                        intensity[1] = 255;
                        intensity[2] = 0;
                    }
                }
            }
        }
    }
}

inline void boundary_extractor::draw_boundary(cv::Mat& image, const boundary &b, const bool padding) {
    const int padding_offeset = (padding)? 1 : 0;
    assert((image.channels() == 1 || image.channels() == 3) && "Invalid channel number");
    if(image.channels()==1) {
        for (cv::Vec2i v : b.points) {
            int x = v[0] + padding_offeset;// +padding_offset is in order to add padding in image
            int y = v[1] + padding_offeset;
            if(x >= 0 && y>=0 && x < image.cols && y < image.rows ) {
                image.at<uchar>(y, x) = 255;
            }
        }
    }else{
        for (cv::Vec2i v : b.points) {

            int x = v[0] + padding_offeset;// +padding_offset is in order to add padding in image
            int y = v[1] + padding_offeset;
            if(x >= 0 && y>=0 && x < image.cols && y < image.rows ){
                cv::Vec3b &intensity = image.at<cv::Vec3b>(y, x);
                intensity[0] = 0;
                intensity[1] = 0;
                intensity[2] = 255;
            }

        }
    }
}

void boundary_extractor::print_boundary_lengths() {
    int i=0;
    for(boundary& b : boundaries_){
        std::cout << i++ <<") has length: " << b.length << std::endl;
    }
}

void boundary_extractor::keep_between(int min_length, int max_length) {
    // It iterates into inverse order to avoid index problem after removing element that isn't in the interval given
    for(int i=(int)boundaries_.size()-1; i >=0 ;--i){
        if(boundaries_[i].length < min_length || boundaries_[i].length > max_length)boundaries_.erase(boundaries_.begin()+i);
    }
}

void boundary_extractor::keep_between_corners(int min_corners, int max_corners){
    for(int i=(int)boundaries_.size()-1; i >=0 ;--i){
        if(boundaries_[i].corners_number < min_corners || boundaries_[i].corners_number > max_corners)boundaries_.erase(boundaries_.begin()+i);
    }
}

void boundary_extractor::compute_corners(cv::Mat& img_corners){
    for(boundary& b : boundaries_){
        b.compute_corners(img_corners);
    }
}

void boundary_extractor::corners_to_matrix(cv::Mat& corner_matrix){
    //it creates a vector of pointers to corners ( pointers have been used to avoid multiple copies )
    std::vector<cv::Vec2i*> all_corners;
    for(boundary& b : boundaries_){
        for(cv::Vec2i& v : b.corners){
            all_corners.push_back(&v);
        }
    }
    // Now all_corners contains all corners of all boundaries so it converts into matrix
    internal_corners_to_matrix(corner_matrix, all_corners);
}

void boundary_extractor::matrix_to_corners(const cv::Mat &corner_matrix){
    int y = 0;
    const float* p;
    for(boundary& b : boundaries_){
        for(cv::Vec2i& v : b.corners){
            p = corner_matrix.ptr<float>(y);
            // Use round to obtain better corners respect to truncate
            v[0] = (int)roundf(p[0]);
            v[1] = (int)roundf(p[1]);
            ++y;
        }
    }
}

inline void boundary_extractor::normalize() {
    // it normalizes all points removing padding offset
    for(boundary& b : boundaries_){
        for(cv::Vec2i& v : b.points){
            v[0] = v[0]-1;
            v[1] = v[1]-1;
        }
    }
}

void boundary_extractor::internal_corners_to_matrix(cv::Mat &corner_matrix, std::vector<cv::Vec2i*> &all_corners) {
    corner_matrix = cv::Mat((int)all_corners.size(),2,CV_32FC1);
    for(int y = 0; y < corner_matrix.rows; ++y ){
        float* p = corner_matrix.ptr<float>(y);
        // it copies boundaries corners value into matrix
        p[0] = (float)(*(all_corners[y]))[0] ;
        p[1] = (float)(*(all_corners[y]))[1] ;
    }
}








