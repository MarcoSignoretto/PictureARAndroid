//
// Created by Marco Signoretto on 06/03/2017.
//

#ifndef ASSIGNMENT2_COMPONENT_H
#define ASSIGNMENT2_COMPONENT_H

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>

namespace mcv{
    class boundary{
    public:
        int min_x = -1;
        int min_y = -1;
        int max_x = -1;
        int max_y = -1;
        int length = 0; // Length of the boundary
        int corners_number = 0;

        // Set of points which compose the boundary
        std::vector<cv::Vec2i> points; // clock wise ordered
        // Set of point which compose the corners
        std::vector<cv::Vec2i> corners; // clock wise ordered

        /**
         * This function allows to add a point to the current boundary
         * @param b: point to add into boundary
         */
        void add_item(cv::Vec2i& b){
            ++length;
            // Updated max and min x,y values used for performance improvements  (corner case max = min)
            if(b[0]<min_x || min_x == -1)min_x = b[0];
            if(b[0]>max_x || max_x == -1)max_x = b[0];
            if(b[1]<min_y || min_y == -1)min_y = b[1];
            if(b[1]>max_y || max_y == -1)max_y = b[1];
            points.push_back(b); // add pixel to boundary points
        }

        /**
         * This function prints all boundaries
         */
        void print();

        /**
         * This function computes boundary corners from a harrisCorner output image
         * @param img_corners: grayscale image extracted from harrisCorner detection
         */
        void compute_corners(cv::Mat& img_corners);
    };


}

#endif //ASSIGNMENT2_COMPONENT_H



