#include <jni.h>
#include <string>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "marker.h"

extern "C" {

JNIEXPORT void JNICALL Java_it_signoretto_marco_picturear_PictureAR_applyAR(
        JNIEnv *env,
        jobject, /* this */
        jlong j_img_0p,
        jlong j_img_1p,
        jlong j_img_0m_th,
        jlong j_img_1m_th,
        jlong j_frame,
        jboolean debug_info) {

    cv::Mat &img_0p = *(cv::Mat *) j_img_0p;
    cv::Mat &img_1p = *(cv::Mat *) j_img_1p;
    cv::Mat &img_0m_th = *(cv::Mat *) j_img_0m_th;
    cv::Mat &img_1m_th = *(cv::Mat *) j_img_1m_th;
    cv::Mat &frame = *(cv::Mat *) j_frame;

    const mcv::Matcher matcher{
            std::vector<const cv::Mat *>{&img_0m_th, &img_1m_th},
            std::vector<const cv::Mat *>{&img_0p, &img_1p}
    };

    try {
        mcv::marker::apply_AR(matcher, frame, debug_info);
    } catch (const cv::Exception &e) {
        // TODO report here somehow
    }
}


}

