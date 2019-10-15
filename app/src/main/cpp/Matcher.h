//
// Created by Marco Signoretto on 13/10/2019.
//

#ifndef PICTUREAR_MATCHER_H
#define PICTUREAR_MATCHER_H


#include <vector>
#include <opencv2/core/mat.hpp>

namespace mcv{
    class Matcher {
    private:
        const std::vector<const cv::Mat*> m_markers;
        const std::vector<const cv::Mat*> m_replacements;
    public:
        Matcher(
                const std::vector<const cv::Mat*>& markers,
                const std::vector<const cv::Mat*>& replacements
        );
        const cv::Mat* findBestMatch(const cv::Mat& frame_to_match, const float threshold) const;
    };
}


#endif //PICTUREAR_MATCHER_H
