package it.signoretto.marco.opencv_test;

import android.app.Activity;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.IOException;
import static org.opencv.core.CvType.CV_8UC1;

public class MainActivity extends Activity implements CameraBridgeViewBase.CvCameraViewListener2 {

    private static final String TAG = "MainActivity";

    private Camera mCamera;

    private Mat img_0p;
    private Mat img_1p;
    private Mat img_0p_rgba;
    private Mat img_1p_rgba;

    private Mat img_0m;
    private Mat img_1m;

    private Mat img_0m_th;
    private Mat img_1m_th;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("opencv_java3");
    }

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION, this, mLoaderCallback);
    }

    private CameraBridgeViewBase mOpenCvCameraView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mCamera = Camera.open();
        Camera.Parameters params = mCamera.getParameters();

        params.setPreviewSize(640, 480);
        params.setPictureSize(640, 480);
        mCamera.setParameters(params);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.HelloOpenCvView);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.enableView();
        mOpenCvCameraView.enableFpsMeter();
        mOpenCvCameraView.setMaxFrameSize(640, 480);

        try{
            img_0p = Utils.loadResource(MainActivity.this, R.drawable.img0p);
            img_1p = Utils.loadResource(MainActivity.this, R.drawable.img1p);
            img_0m = Utils.loadResource(MainActivity.this, R.drawable.img0m);
            img_1m = Utils.loadResource(MainActivity.this, R.drawable.img1m);

            img_0p_rgba = new Mat(img_0p.rows(), img_0p.cols(), img_0p.type());
            Imgproc.cvtColor(img_0p, img_0p_rgba,Imgproc.COLOR_BGR2RGBA);

            img_1p_rgba = new Mat(img_1p.rows(), img_1p.cols(), img_1p.type());
            Imgproc.cvtColor(img_1p, img_1p_rgba,Imgproc.COLOR_BGR2RGBA);

            img_0m_th = new Mat(img_0m.rows(), img_0m.cols(), CV_8UC1);
            Imgproc.cvtColor(img_0m,img_0m_th,Imgproc.COLOR_BGR2GRAY);

            img_1m_th = new Mat(img_1m.rows(), img_1m.cols(), CV_8UC1);
            Imgproc.cvtColor(img_1m,img_1m_th,Imgproc.COLOR_BGR2GRAY);

        }catch(IOException ioe){
            Log.e(TAG, "Impossible load all required resources" , ioe);
        }

    }


    @Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
        if (mCamera != null)
            mCamera = null;
    }

    public void onCameraViewStarted(int width, int height) {
    }

    public void onCameraViewStopped() {

    }


    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        Mat frame = inputFrame.rgba();
        PictureAR.apply_AR(img_0p_rgba, img_1p_rgba, img_0m_th, img_1m_th, frame, false);
        return frame;
    }
}
