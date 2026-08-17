#include <glog/logging.h>

#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vitis/ai/yolovx.hpp>
//#include <vitis/ai/demo.hpp>

//#include "./process_result.hpp"

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

        static const std::vector<std::string> class_names = {
            "person","bicycle","car","motorcycle","airplane","bus","train","truck","boat",
            "traffic light","fire hydrant","stop sign","parking meter","bench","bird","cat",
            "dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack",
            "umbrella","handbag","tie","suitcase","frisbee","skis","snowboard","sports ball",
            "kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket",
            "bottle","wine glass","cup","fork","knife","spoon","bowl","banana","apple",
            "sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair",
            "couch","potted plant","bed","dining table","toilet","tv","laptop","mouse",
            "remote","keyboard","cell phone","microwave","oven","toaster","sink",
            "refrigerator","book","clock","vase","scissors","teddy bear","hair drier",
            "toothbrush"
        };

        for (const auto &obj : result.bboxes) {
            int label = obj.label;
            auto &box = obj.box;

            cv::rectangle(frame,
                          cv::Point((int)box[0], (int)box[1]),
                          cv::Point((int)box[2], (int)box[3]),
                          cv::Scalar(0,255,0), 2);

            std::string text = class_names[label] + " " +
                               cv::format("%.2f", obj.score);

            cv::putText(frame,
                        text,
                        cv::Point((int)box[0], std::max(0, (int)box[1] - 5)),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.5,
                        cv::Scalar(0,255,0),
                        2);
        }

        imshow("YOLOvX Nano Live Inference", frame);

        // // 4. Overlay Detection Results
        // // Uses the drawing logic provided in your process_result.hpp
        // process_result(frame, result, false);

        // // 5. Display through DisplayPort
        // // imshow renders to the connected DP monitor on the ZCU104
        // imshow("YOLOvX Nano Live Inference", frame);

        // Exit on 'q' or 'ESC'
        char key = (char)waitKey(1);
        if (key == 'q' || key == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
