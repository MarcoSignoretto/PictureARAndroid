package it.signoretto.marco.picturear;

import org.opencv.core.Mat;

/**
 * Created by
 * Marco Signoretto
 * Android Developer
 * on 23/12/2017.
 */

public class PictureAR {

    public static void apply_AR(Mat img_0p, Mat img_1p, Mat img_0m_th, Mat img_1m_th, Mat frame, boolean debug_info){
        applyAR(img_0p.nativeObj, img_1p.nativeObj, img_0m_th.nativeObj, img_1m_th.nativeObj, frame.nativeObj, debug_info);
    }

    private static native void applyAR(long img_0p, long img_1p, long img_0m_th, long img_1m_th, long frame, boolean debug_info);


}
