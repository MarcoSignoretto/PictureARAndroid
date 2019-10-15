//
// Created by Marco Signoretto on 29/06/2017.
//

#ifndef PROJECT_MARKER_H
#define PROJECT_MARKER_H

#include <opencv2/core/mat.hpp>
#include "Matcher.h"

namespace mcv{
    namespace marker{

        /// Constants related to boundary length filter
        /*
         * These value has been chosen empirically
         */
        const int BOUNDARY_MIN_LENGTH = 200;
        const int BOUNDARY_MAX_LENGTH = 1500;

        /// Threshold value for marker matching
        /* This is a very important parameter:
         * value must be between 0.0 and 1.0 and it is the threshold of probability that it is a given marker.
         * value close to 1.0:
         *      - easier to reject marker as candidate because we request that marker extracted must be very similar to
         *        the original one, in this configuration it's difficult to confuse between the two markers
         * value far away for 1.0:
         *      - easier to consider a candidate marker as a marker also if "noised" but in this configuration it's more
         *        probable to have confusion between the two markers
         * In this project I have chosen a value close to 1.0 because I would like to see correct marker or not at all,
         * if you prefer to see markers also if there is higher probability to confuse them you are free to change this
         * parameter ( for example 0.85 ensure strong detection also on fast movement or high inclination but
         * maybe you will see the wrong marker )
         *
         * Also higher image resolution could help correct recognition also far away from webcam or high angle but
         * for this project the resolution has been chosen to be equal of resolution of the video test
         *
         * */
        const float MATCH_THRESHOLD = 0.90f;
        /// Constants related to marker orientation detection
        const int WIDTH = 135;
        const int HEIGHT = 55;

        const int OFFSET = 53; // marker border size
        const std::vector< cv::Point > RECT_0 = {
                cv::Point(256-(OFFSET+WIDTH), 256-(OFFSET+HEIGHT)),
                cv::Point(256-OFFSET, 256-OFFSET)
        };
        const std::vector< cv::Point > RECT_90 = {
                cv::Point(256-(OFFSET+HEIGHT), OFFSET),
                cv::Point(256-OFFSET, OFFSET+WIDTH)
        };
        const std::vector< cv::Point > RECT_180 = {
                cv::Point(OFFSET, OFFSET),
                cv::Point(OFFSET+WIDTH, OFFSET+HEIGHT)
        };
        const std::vector< cv::Point > RECT_270 = {
                cv::Point(OFFSET, 256-(OFFSET+WIDTH)),
                cv::Point(OFFSET+HEIGHT, 256-OFFSET)
        };

        const std::vector<cv::Vec2d> DST_POINTS = {
                cv::Vec2d(0, 0),
                cv::Vec2d(256, 0),
                cv::Vec2d(256, 256),
                cv::Vec2d(0, 256),
        };

        /**
         * it detects orientation of a given marker and it returns its orientation in degree to obtain the original marker orientation
         * @param warped_image: image which should contain a marker to work properly (must be thresholded and 256x256)
         * @return 0,90,180,270 degree of rotation respect to the original marker
         */
        int detect_orientation(const cv::Mat& warped_image);

        /**
         * This function, given the "rotation_degree" obtained from detect_orientation function, calculates the rotation
         * matrix which will be saved into rotation_matrix
         * @see detect_orientation
         * @param rotation_matrix: reference of matrix where rotation matrix will be set
         * @param rotation_degree: rotation in degree to obtain the original marker orientation
         * @param rotation_update: if this is true rotation matrix will be updated and not recreated
         */
        void calculate_rotation_matrix(cv::Mat& rotation_matrix, int rotation_degree, const bool rotation_update = false);

        /**
         * This function works as calculate_rotation_matrix but it updates an already inited rotation_matrix which will be used to rotate picture before warp
         * @see calculate_rotation_matrix
         * @param rotation_matrix: reference of matrix where rotation matrix will be set
         * @param rotation_degree: rotation in degree to obtain the original marker orientation (WARNING not picture orientation but marker)
         */
        void calculate_picture_rotation(cv::Mat& rotation_matrix, int rotation_degree);

        /**
         * This function computes the probability of a certain marker ( marker_extracted ) to be the "marker_candidate"
         * @param marker_extracted: marker extracted from frame
         * @param marker_candidate: one of the marker for the pictures ( OM or 1M )
         * @return probability that the two markers are the same
         */
        float compute_matching(const cv::Mat& marker_extracted, const cv::Mat& marker_candidate, cv::Point top_left = cv::Point(0,0), cv::Point bottom_right = cv::Point(256,256));



        /**
         * This function executes the pipeline to apply AR to the original image "camera_frame", the pipeline is the following:
         * 1) Convert original frame into grayscale
         * 2) Apply Otzu threshold to grayscale image
         * 3) Extract image boundaries
         * 4) Filter the above boundaries with length between BOUNDARY_MIN_LENGTH and BOUNDARY_MAX_LENGTH
         * 5) Create image with filtered boundaries
         * 6) Apply harris corner into the above image
         * 7) Compute which pixels of the boundaries are corners
         * 8) Keep only boundaries which have 4 corners
         * 9) Improve boundary corners of the remaining boundaries with cornerSubPix
         * For each boundary:
         * 10) find homography and warp image into a 256x256 image ( from unblured_grayscale )
         * 11) detect marker orientation ( in this step also other candidate marker has been rotate because final filtering is applied during matching phase )
         * 12) calculate rotation for placeholder and warp into 256x256 image to perform matching
         * 13) compute matching coefficient
         * 14) warp placeholder with higher probability into original image if matching is above MATCH_THRESHOLD
         *
         * A Gaussian bluring has been applied during test phase but this filter doesn't improve quality of recognition
         * so it has been thrown away
         *
         * @param img_0p: image placeholder 0 ( leo picture )
         * @param img_1p: image placeholder 1 ( van picture )
         * @param img_0m_th: image marker 0 thresholded ( leo marker )
         * @param img_1m_th: image marker 1 thresholded ( van marker )
         * @param camera_frame: original image on which AR will be applied
         * @param debug_info: if true additional images will be shown with debug pourpose
         */
        void apply_AR(const mcv::Matcher& matcher, cv::Mat& camera_frame,  bool debug_info);

    }
}

#endif //PROJECT_MARKER_H
