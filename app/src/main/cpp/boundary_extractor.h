//
// Created by Marco Signoretto on 06/03/2017.
//

#ifndef ASSIGNMENT2_BOUNDARY_EXTRACTOR_H
#define ASSIGNMENT2_BOUNDARY_EXTRACTOR_H

#include <vector>
#include <opencv2/core/mat.hpp>
#include "boundary.h"
#include "utils.h"

namespace mcv{


    /**
     * Class that allows operations on boundaries, c and b are the common parameters used in Moore's algorithm
     * Vec2i is used to store (x,y) Vec2i[0] is x Vec2i[1] is y
     */
    class boundary_extractor {
    public:

        /**
         * Constructor: this constructor performs loading of an image with filename given as a parameter then it computes otsu thresholding and it
         * keeps the result of thresholding in memory ( this is a target of all class operations )
         * @param filename name of image that we want to analize
         */
        explicit boundary_extractor(const std::string& filename);

        /**
        * Constructor: as before but using a gray scale image instead of a filename
        * @param image_gray: GrayScale image if "compute_threshold" = true, thresholded image if compute threshold = false
        * @param compute_threshold: if true an otsu threshold will be compute into input image
        */
        boundary_extractor(const cv::Mat& image_gray, bool compute_threshold = true);

        /**
         * Find all boundaries of the image
         * @param boundary_color: color of the boundary ( mcv::BLACK or mcv::WHITE )
         */
        void find_boundaries(const uchar boundary_color = WHITE);

        /**
         * Given a point with coordinate x,y find boundary starting from that point
         * @param x is the column index of image
         * @param y is the row index of the image
         * @param boundary_color: color of the boundary ( mcv::BLACK or mcv::WHITE )
         */
        inline boundary moore_algorithm(int x, int y, const uchar boundary_color);

        /**
         * Draw boundaries evaluated with find_boundaries and alive after keep_between
         * @param dest filename of the destionation image
         */
        void draw_boundaries(const std::string& dest);

        /**
         * This function creates a new binary image where pixel is WHITE (255) if it is in a boundary BLACK (0) otherwise
         * @param image: image where results are stored
         */
        void create_boundaries_image(cv::Mat& image);

        /**
         * This function draws all boundaries in RED into the "image" parameter
         * @param image: image where boundaries will be drawn ( must have 3 channels and colored )
         */
        void draw_boundaries(cv::Mat& image);

        /**
         * This function draws all corners in GREEN into the "image" parameter
         * @param image: image where corners will be drawn ( must have 3 channels and colored )
         */
        void draw_boundaries_corners(cv::Mat& image);

        /**
         * This function purges boundaries vector and it keeps only boundary which has length between
         * min_length and min_length
         * @param min_length lower bound (included)
         * @param max_length upper bound (included)
         */
        void keep_between(int min_length, int max_length);

        /**
         * This function allows to keep only boundaries with a number of corners between "min_corners" and "max_corners"
         * @param min_corners: min corners that boundary must contains
         * @param max_corners: max corners that boundary must contains
         */
        void keep_between_corners(int min_corners, int max_corners);

        /**
         * Compute boundary corners starting from an image obtained from harris corner "img_corners"
         * @param img_corners: images with harris corners
         */
        void compute_corners(cv::Mat& img_corners);

        /**
         * This function converts internal boundary corners representation in cv::Mat format and it stores this result in
         * the "corner_matrix" (WARNING corners are stored in order of boundaries)
         * between this function and "matrix_to_corners" function no boundary filters can be applied
         * @param corner_matrix: matrix where corners will be saved
         * @see matrix_to_corners
         */
        void corners_to_matrix(cv::Mat& corner_matrix);

        /**
         * This function performs the inverse operation respect to "corners_to_matrix" function and it updates boundary corners
         * between "corners_to_matrix" function and this function no boundaries filters can be applied
         * @param corner_matrix: improved corner matrix which will be used to update boundaries corners
         * @see corners_to_matrix
         */
        void matrix_to_corners(const cv::Mat& corner_matrix);

        /**
         * This function returns all boundaries which haven't been throw away
         * @return boundaries
         */
        inline std::vector<boundary>& get_boundaries(){
            return boundaries_;
        }

        /**
         * This function, for each boundary, prints index of boundary into boundaries vector and his length
         */
        void print_boundary_lengths();

    private:
        // Name of the target image
        const std::string filename_;
        // Image thresholded with 1 pixel of padding outside
        cv::Mat image_;
        // Vector of all boundaries of the image ( full after calling find_boundaries )
        std::vector<boundary> boundaries_;

        /**
         * Internal function which checks position (x,y) is a valid initial boundary point not already found
         * @param x column index in image_
         * @param y row index in image_
         * @return true if that point is the starting point for a new boundary
         */
        inline bool is_valid(int x, int y);

        /**
         * Check if point (x,y) is already present into a boundary given as param, this is used to avoid multiple
         * boundaries with different starting point but the same sequence
         * @param b: boundary where search (x,y)
         * @param x
         * @param y
         * @return true if (x,y) is present into bounddary b
         */
        inline bool check_in(const boundary& b, int x, int y);

        /**
         * Search in clock wise order a new c and b starting from current_c current_b, current_b is the center
         * @param current_c: precedent black value in terms of anti-clockwise respect the previous b
         * @param current_b: center of the clock
         * @param c: used as return param (contain new c value if found = true)
         * @param b: used as return param (contain new b value if found = true)
         * @param boundary_color: color of the boundary ( mcv::BLACK or mcv::WHITE )
         * @return found: true if a new b and c are found false otherwise ( single point boundary never found new b,c)
         */
        inline bool search_clockwise(cv::Vec2i& current_c, cv::Vec2i& current_b, cv::Vec2i* c, cv::Vec2i* b, const uchar boundary_color);

        /**
         * Find clock index of c given that center is b
         * clock index are defined as:
         *
         * 0  1  2
         * 7  b  3
         * 6  5  4
         *
         * where b is the center (param b)
         *
         * @param c: c in moore algorithm
         * @param b: center of the clock
         * @return clock index
         */
        inline int find_clock_index(cv::Vec2i* c, cv::Vec2i* b);

        /**
         * Internal function which draws in red a boundary on a image divided into channel given.
         * @param b: boundary to draw
         * @param channels: 0 is blue, 1 is green, 2 is red (as standard OpenCV)
         */
        inline void draw_boundary(const cv::Mat& image, const boundary& b, std::vector<cv::Mat>& channels);

        /**
         * This function draws boundary's corners into the "image"
         * @param image: destination of corners drawing ( must have 3 channels and colored )
         * @param b: boundary which will be drawn
         */
        inline void draw_corners(cv::Mat& image, const boundary &b);

        /**
         * This function draws boundary into the "image"
         * @param image: destination of boundary drawing ( must have 3 channels and colored or single channel grayscale )
         * @param b: boundary which will be drawn
         * @param padding: if the destination image "image" has 1 pixel of padding or not
         */
        inline void draw_boundary(cv::Mat& image, const boundary &b, const bool padding = false);

        /**
         * This function removes offset of padding image for all boundary points of each boundary
         */
        inline void normalize();

        /**
         * This function performs the raw operation of conversion between boundaries corners and cv::Mat and it inits the
         * destination matrix "corner_matrix"
         * @param corner_matrix: matrix where results are stored
         * @param all_corners: vector of pointers where all corners of all boundaries have been putted together
         */
        inline void internal_corners_to_matrix(cv::Mat& corner_matrix, std::vector<cv::Vec2i*>& all_corners);
    };

}



#endif //ASSIGNMENT2_BOUNDARY_EXTRACTOR_H
