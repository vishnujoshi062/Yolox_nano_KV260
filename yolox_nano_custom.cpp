#include <glog/logging.h>

#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vitis/ai/yolovx.hpp>
//#include <vitis/ai/demo.hpp>

#include "./process_result.hpp"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
    // 1. Initialize the YOLOvX model using your custom model folder name
    // The factory in yolovx.cpp will create a YOLOvXImp instance
    string model_name = "yolox_nano_custom";
    auto det = vitis::ai::YOLOvX::create(model_name);

    // 2. Open the USB webcam (device node 0)
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open USB webcam at /dev/video0" << endl;
        return -1;
    }

    // Set resolution (optional, adjust based on webcam support)
    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);

    Mat frame;
    while (true) {
        cap >> frame; // Capture live frame
        if (frame.empty()) break;

        // 3. Execute DPU Inference
        // This triggers the letterbox preprocessing and DPU task in yolovx_imp.cpp
        auto result = det->run(frame);

        // 4. Overlay Detection Results
        // Uses the drawing logic provided in your process_result.hpp
        process_result(frame, result, false);

        // 5. Display through DisplayPort
        // imshow renders to the connected DP monitor on the ZCU104
        imshow("YOLOvX Nano Live Inference", frame);

        // Exit on 'q' or 'ESC'
        char key = (char)waitKey(1);
        if (key == 'q' || key == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}