#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <streambuf>
#include <string>

#include <daisykitsdk/flows/face_detector_with_mask_flow.h>
#include <daisykitsdk/thirdparties/json.hpp>

using namespace cv;
using namespace std;
using json = nlohmann::json;
using namespace daisykit::common;
using namespace daisykit::flows;

int main(int, char **) {
  std::ifstream t("../data/configs/face_detector_with_mask_config.json");
  std::string config_str((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());

  FaceDetectorWithMaskFlow flow(config_str);

  Mat frame;
  VideoCapture cap(0);

  while (1) {
    cap >> frame;
    cv::Mat rgb;
    cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);

    flow.Process(rgb);
    flow.DrawResult(rgb);

    cv::Mat draw;
    cv::cvtColor(rgb, draw, cv::COLOR_RGB2BGR);
    imshow("Image", draw);
    waitKey(1);
  }

  return 0;
}