package it.signoretto.marco.picturear;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Entity;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.hardware.camera2.*;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.Toast;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.IOException;
import java.util.ArrayList;

import static org.opencv.core.CvType.CV_8UC1;

public class MainActivity extends Activity implements CameraBridgeViewBase.CvCameraViewListener2 {

    private static final String TAG = "MainActivity";
    private static final int MY_PERMISSIONS_REQUEST_CAMERA = 1;

    private CameraManager cameraManager;
    private CameraDevice cameraDevice;

    private String[] cameraIdList;

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
        /*if(!OpenCVLoader.initDebug()) {

        }*/

    }

    private CameraBridgeViewBase mOpenCvCameraView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);

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
        } catch(IOException ioe){
            Log.e(TAG, "Impossible load all required resources" , ioe);
        }

        requestCameraPermission();
    }

    @Override
    public void onResume()
    {
        if(mOpenCvCameraView!=null){
            mOpenCvCameraView.enableView();
        }
        super.onResume();
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
        if (cameraDevice != null) {
            cameraDevice.close();
            cameraDevice = null;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_CAMERA: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                    setupApplication();

                } else {

                    Toast.makeText(this,R.string.camera_permission_not_ready, Toast.LENGTH_LONG).show();
                    finish();

                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                }
                return;
            }

            // other 'case' lines to check for other
            // permissions this app might request.
        }
    }

    public void onCameraViewStarted(int width, int height) {
        Log.v(TAG, "Camera View Started");
    }

    public void onCameraViewStopped() {
        Log.v(TAG, "Camera View Stopped");
    }


    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        Mat frame = inputFrame.rgba();
        PictureAR.apply_AR(img_0p_rgba, img_1p_rgba, img_0m_th, img_1m_th, frame, false);
        return frame;
    }

    private void requestCameraPermission(){
        // Here, thisActivity is the current activity
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {

            // Should we show an explanation?
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.CAMERA)) {


                AlertDialog dialog = new AlertDialog.Builder(this, android.R.style.Theme_Material_Dialog_Alert)
                        .setTitle(R.string.camera_permission_title)
                        .setMessage(R.string.camera_permission_message)
                        .setCancelable(false)
                        .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                ActivityCompat.requestPermissions(MainActivity.this,
                                        new String[]{Manifest.permission.CAMERA},
                                        MY_PERMISSIONS_REQUEST_CAMERA);
                            }
                        })
                        .create();

                dialog.show();

                // Show an explanation to the user *asynchronously* -- don't block
                // this thread waiting for the user's response! After the user
                // sees the explanation, try again to request the permission.

            } else {

                // No explanation needed, we can request the permission.

                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.CAMERA},
                        MY_PERMISSIONS_REQUEST_CAMERA);

                // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
                // app-defined int constant. The callback method gets the
                // result of the request.
            }
        }else{
            setupApplication();
        }
    }

    private void setupApplication(){
        try{
            cameraManager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
            if(cameraManager!=null){
                cameraIdList = cameraManager.getCameraIdList();
            }
            if(cameraIdList!=null && cameraIdList.length>0){
                changeCamera(cameraIdList[0]);
            }
        }catch(CameraAccessException e) {
            Log.e(TAG, "Camera exception", e);
        }




        //OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION, this, mLoaderCallback);
    }

    /*public void showCameraDialog(View view){
        try{
            final String[] items = cameraManager.getCameraIdList();

            final AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Pick one");


            ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplicationContext(),android.R.layout.simple_list_item_1,items);
            builder.setSingleChoiceItems(adapter, 0, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    changeCamera(items[i]);
                }
            });


            builder.show();
        }catch(CameraAccessException e) {
            Log.e(TAG, "Camera exception", e);
        }
    }*/

    public void showCameraDialog(View view){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try{
                    final String[] items = cameraManager.getCameraIdList();
                    final String[] cameraInfo = new String[items.length];

                    for(int i=0; i < items.length; ++i){
                        CameraCharacteristics chars = cameraManager.getCameraCharacteristics(items[i]);
                        // Does the camera have a forwards facing lens?
                        Integer facing = chars.get(CameraCharacteristics.LENS_FACING);
                        String facingString;
                        if(facing!= null && facing == CameraCharacteristics.LENS_FACING_FRONT){
                            facingString = "Front";
                        }else if(facing!= null && facing == CameraCharacteristics.LENS_FACING_BACK){
                            facingString = "Back";
                        }else{
                            facingString = "Unknown";
                        }
                        cameraInfo[i] = "camera index: "+i+" ("+facingString+")";
                    }

                    //Create sequence of items
                    //final String[] Animals = items.toArray(new String[items.size()]);
                    AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(MainActivity.this, android.R.style.Theme_Material_Dialog_Alert);
                    dialogBuilder.setTitle("Cameras");
                    dialogBuilder.setItems(cameraInfo, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int item) {
                            //String selectedText = items[item];  //Selected item in listview
                            changeCamera(items[item]);
                            dialog.dismiss();
                        }
                    });
                    //Create alert dialog object via builder
                    AlertDialog alertDialogObject = dialogBuilder.create();
                    //Show the dialog
                    alertDialogObject.show();
                }catch(CameraAccessException e) {
                    Log.e(TAG, "Camera exception", e);
                }
            }
        });

    }

    private void changeCamera(String cameraIndex){
        if(cameraDevice!=null)cameraDevice.close();
        if(mOpenCvCameraView!=null)mOpenCvCameraView.disableView();
        try{
            cameraManager.openCamera(cameraIndex,new CameraDevice.StateCallback(){
                @Override
                public void onClosed(@NonNull CameraDevice camera) {
                    super.onClosed(camera);
                    Log.e(TAG, "Closed camera "+camera.getId());
                    if(mOpenCvCameraView!=null){
                        mOpenCvCameraView.disableView();
                    }
                }

                @Override
                public void onOpened(@NonNull CameraDevice cameraDevice) {
                    MainActivity.this.cameraDevice = cameraDevice;
                    cameraSetup(cameraDevice);
                    if(mOpenCvCameraView!=null){
                        mOpenCvCameraView.enableView();
                    }
                }

                @Override
                public void onDisconnected(@NonNull CameraDevice cameraDevice) {
                    Log.e(TAG, "Disconnected camera "+cameraDevice.getId());
                }

                @Override
                public void onError(@NonNull CameraDevice cameraDevice, int i) {
                    Log.e(TAG, "Error camera"+cameraDevice.getId()+" error number: "+i);

                }
            },null);


        } catch(CameraAccessException e) {
            Log.e(TAG, "Camera exception", e);
        }
    }

    private void cameraSetup(final CameraDevice cameraDevice){

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.HelloOpenCvView);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.enableView();
        mOpenCvCameraView.enableFpsMeter();
        mOpenCvCameraView.setMaxFrameSize(640, 480);

        /*mCamera = Camera.open();
        Camera.Parameters params = mCamera.getParameters();

        params.setPreviewSize(640, 480);
        params.setPictureSize(640, 480);
        mCamera.setParameters(params);*/





    }
}
