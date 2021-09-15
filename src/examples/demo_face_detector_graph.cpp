// Copyright 2021 The DaisyKit Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "daisykitsdk/graphs/core/graph.h"
#include "daisykitsdk/graphs/core/node.h"
#include "daisykitsdk/graphs/core/utils.h"
#include "daisykitsdk/graphs/nodes/distributors/image_distributor_node.h"
#include "daisykitsdk/graphs/nodes/models/face_detector_node.h"
#include "daisykitsdk/graphs/nodes/visualizers/face_visualizer_node.h"
#include "daisykitsdk/thirdparties/json.hpp"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <streambuf>
#include <string>

using namespace cv;
using namespace std;
using json = nlohmann::json;
using namespace daisykit::common;
using namespace daisykit::graphs;

int main(int, char**) {
  // Create processing nodes
  std::shared_ptr<ImageDistributorNode> image_distributor_node =
      std::make_shared<ImageDistributorNode>("image_distributor",
                                             NodeType::kAsyncNode);
  std::shared_ptr<FaceDetectorNode> face_detector_node =
      std::make_shared<FaceDetectorNode>(
          "face_detection",
          "../assets/models/face_detection/yolo_fastest_with_mask/"
          "yolo-fastest-opt.param",
          "../assets/models/face_detection/yolo_fastest_with_mask/"
          "yolo-fastest-opt.bin",
          NodeType::kAsyncNode);
  std::shared_ptr<FaceVisualizerNode> face_visualizer_node =
      std::make_shared<FaceVisualizerNode>("face_visualizer",
                                           NodeType::kAsyncNode);

  // Create connections between nodes
  Graph::Connect(nullptr, "", image_distributor_node.get(), "input",
                 TransmissionProfile(5, true), true);
  Graph::Connect(image_distributor_node.get(), "output",
                 face_visualizer_node.get(), "image",
                 TransmissionProfile(5, true), true);
  Graph::Connect(image_distributor_node.get(), "output",
                 face_detector_node.get(), "input",
                 TransmissionProfile(5, true), true);
  Graph::Connect(face_detector_node.get(), "output", face_visualizer_node.get(),
                 "faces", TransmissionProfile(5, true), true);

  // Need to init these nodes before use
  // This method also start worker threads of asynchronous node
  image_distributor_node->Activate();
  face_detector_node->Activate();
  face_visualizer_node->Activate();

  VideoCapture cap(0);

  while (1) {
    std::shared_ptr<Packet> in_packet;
    Mat frame;
    cap >> frame;
    cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
    CvMat2Packet(frame, in_packet);
    image_distributor_node->Input("input", in_packet);
  }

  return 0;
}