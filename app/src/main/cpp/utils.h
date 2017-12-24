//
// Created by Marco Signoretto on 07/03/2017.
//

#ifndef ASSIGNMENT2_UTILS_H
#define ASSIGNMENT2_UTILS_H

#include <opencv2/core/mat.hpp>

using namespace std;

namespace mcv{

    const uchar WHITE = 255;
    const uchar BLACK = 0;

    /**
     * Normalize histogram of image
     * @param hist
     * @param image
     * @return a Mat object which represents the normalized histogram of an image
     */
    cv::Mat normalize_hist(cv::Mat &hist, const cv::Mat &image);

    /**
     * Function that given an histogram "hist", it plots an histogram figure on 256x256 image with name specified into filename
     * In order to have a good representation height are proportional to max value into the histogram
     * (max value into the histogram has all pixels black, other values have a proportional amount of pixels filled )
     * @param hist: non normalized histogram
     * @param max_value: max value reach on hist ( passed as param for performance reason )
     * @param filename: name of the histogram image
     * @return image of the histogram
     */
    cv::Mat generate_hist_image(const cv::Mat &hist, int max_value, string filename);

    /**
     * Calculate histogram for a 8-bit gray scale image
     * @param imageGray: image into gray scale
     * @param max_value: additional return param passed not init, in the end of function it contains max frequency present
     * into histogram
     * @return histogram of image_gray given as param
     */
    cv::Mat compute_hist(const cv::Mat& image_gray, int& max_value);

    /**
     * Testing function used only for feedback
     * @param normalized_hist
     */
    void test_normalized_hist(const cv::Mat &normalized_hist);

    /**
     * Calculate and return CDF of a normalized_hist
     * @param normHist: used to calculate CDF
     * @return 1,L matrix that represent CDF
     */
    cv::Mat compute_CDF(cv::Mat &norm_hist);

    /**
     * Calculate and return cumulative mean of normalizedHist
     * @param norm_hist: used to calculate cumulative mean vector
     * @return 1,L matrix that represent cumulative mean vector
     */
    cv::Mat compute_cumulative_mean(cv::Mat& norm_hist);

    /**
     * Calculate the between variance for cluster C1 (0,T] and cluster C2 (T,L-1]
     * @param threshold: the T value
     * @param cum_sum: CDF of the normalized histogram
     * @param cum_mean: cumulative mean
     * @return value of the between class variance
     */
    float between_class_variance(int threshold, const cv::Mat& cum_sum, const cv::Mat& cum_mean);

    /**
     * Search best value for thresholding using Otsu tecnique and returns this value
     * @param norm_hist: normalized histogram
     * @return best threshold for Otsu tecnique
     */
    int compute_Otsu_thresholding(cv::Mat &norm_hist);

    /**
     * Generate new image obtained from thresholding operation.
     * @param threshold: param the separate black and white values
     * @param image_gray: image were applied thresholding
     * @return
     */
    cv::Mat image_threshold(int threshold, const cv::Mat &image_gray);

    /**
     * Compute full image otsu thresholding process:
     * hist + normalization + thresholding
     * @param image_gray: input grayscale image
     * @param image_th: output thresholded image
     */
    void image_otsu_thresholding(const cv::Mat& image_gray, cv::Mat& image_th);

    /**
     *
     * @param window_mat: input matrix
     * @param H: output of rho_theta_plane generated ( can be used for some other operations )
     * @param best_rho_theta: return value of the rho theta with higher number of votes:
     *          best_rho_theta.x are theta
     *          best_rho_theta.y are rho
     */
    void compute_rho_theta_plane(const cv::Mat &window_mat, cv::Mat &H, cv::Point2f& best_rho_theta) ;

    void extract_lines(const cv::Mat &window_mat, cv::Mat& H, std::vector<cv::Point2f>& lines, int min_score);

    /**
     * Convert radiant to degree
     * @param radiant: input
     * @return degree conversion of radiant
     */
    double to_degree(double radiant);

    /**
     * Draw line with theta perpendicular to the given
     * @param dest: image matrix where plot
     * @param rho: param of hough transform
     * @param theta: theta of hough transform
     * @param origin_x: x origin on x axes respect to window where rho, theta are calculated
     * @param origin_y: y origin on y axes respect to window where rho, theta are calculated
     */
    void line(cv::Mat& dest, double rho, double theta, double origin_x=0.0, double origin_y = 0.0);

    /**
     * Get m of the equation Y = mx+q
     * @param theta: theta of hough transform
     * @return m coefficient
     */
    double m(double theta);

    /**
     * Get q of the equation Y = mx+q
     * @param rho: param of hough transform
     * @param theta: theta of hough transform
     * @return q coefficient
     */
    double q(double rho, double theta);

    /**
     * Convert rho_theta plane in a 8bit 1 channel image in order to plot his representation
     * @param rho_theta_plane: obtained from compute_rho_theta_plane
     * @return matrix to use as image
     */
    cv::Mat to_image(cv::Mat& rho_theta_plane);

    /**
     * This function wraps rectangle function of openCV
     * @param dst: colored image where write
     * @param rect: rectangle to write
     */
    void draw_rect(cv::Mat& dst, const std::vector<cv::Point>& rect);

}

#endif //ASSIGNMENT2_UTILS_H
