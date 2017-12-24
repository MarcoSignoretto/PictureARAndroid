//
// Created by Marco Signoretto on 09/07/2017.
//

#include "boundary.h"

void mcv::boundary::print(){
    std::cout << "Boundary:" << std::endl;
    for (cv::Vec2i& v : points) {
        std::cout << "(" << v[0] << "," << v[1] << ")" << std::endl;
    }
}

void mcv::boundary::compute_corners(cv::Mat& img_corners){
    const float CORNER_THRESHOLD = 2.0f;
    int kernel_size = 0; // kernel_size greater than zero was used to improve corners before cornerSubPix optimization
    bool on_corner = false;
    cv::Vec2i* corner = nullptr;
    float corner_intensity = 0.0f; // Intensity of the corners
    for(int i = 0; i < points.size(); ++i) {
        cv::Vec2i &point = points[i];

        // Calculate intensity based on neighbourhood
        float current_intensity = 0.0f;
        const float *p;
        for(int y = point[1]+1-kernel_size; y <= point[1]+1+kernel_size; ++y) { // Added 1px because img_corners is an image with padding
            if (y >= 0 && y < img_corners.rows) { // Bound checking
                p = img_corners.ptr<float>(y);
                for (int x = point[0] + 1 - kernel_size; x <= point[0] + 1 + kernel_size; ++x) {
                    if (x >= 0 && x < img_corners.cols) { // Bound checking for neighbour technique
                        current_intensity += p[x];
                    }
                }
            }
        }

        // Check if the current point is a corner or not
        if(current_intensity > CORNER_THRESHOLD){
            if(!on_corner){
                on_corner = true;
            }
            // pixel already on the corner but if the current intensity is greater respect to the previous one, update corner
            if(current_intensity>corner_intensity){
                corner_intensity = current_intensity;
                corner = &point; // Use reference to point to avoid normalization problem
            }
        }else{
            if(on_corner){
                corners_number += 1;
                corners.push_back(*corner); // Copy point to corners set
                // Reset values for next corner
                on_corner = false;
                corner_intensity = 0.0f;
                corner = nullptr;
            }
        }

    }
}

