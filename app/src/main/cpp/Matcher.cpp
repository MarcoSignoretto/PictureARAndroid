//
// Created by Marco Signoretto on 13/10/2019.
//

#include "Matcher.h"
#include "marker.h"

int maxIndex(const std::vector<float>& scores){
    int max_index = -1;
    float max = 0.0f;
    for (int i = 0; i < scores.size() ; ++i) {
        if(scores[i] >= max){
            max = scores[i];
            max_index = i;
        }
    }
    return max_index;
}



mcv::Matcher::Matcher(const std::vector<const cv::Mat*>& markers,
                      const std::vector<const cv::Mat*>& replacements)
:m_markers{markers},
 m_replacements{replacements}
{}

const cv::Mat* mcv::Matcher::findBestMatch(const cv::Mat& frame_to_match, const float threshold) const {
    std::vector<float> scores(m_markers.size());
    for(int i=0; i < scores.size(); ++i){
        scores[i] = mcv::marker::compute_matching(*(m_markers[i]), frame_to_match);
    }

    int max_index = maxIndex(scores);
    if(max_index > -1 && scores[max_index] > threshold){
        return m_replacements[max_index];
    }else{
        return nullptr;
    }
}
