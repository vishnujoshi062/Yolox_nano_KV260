
# YOLOX-Nano on Kria KV260 (Vitis-AI Deployment)

Deployment notes and code for running real-time YOLOX-Nano object detection on the AMD Xilinx Kria KV260 Vision AI Starter Kit using Vitis-AI and PetaLinux.



## Directory Structure


Yolox_nano_KV260/
├── images/
├── mpsoc/                                  # PetaLinux BSP & configuration files
├── pt_yolox-nano_coco_416_416_1G_3.0/      # Downloaded Model Zoo artifacts & .xmodel
├── yolox_nano_custom.cpp                   # C++ application using Vitis-AI runtime
├── results.md                              # Live inference verification & results
└── README.md


Deployment Steps

1. Vitis-AI Setup (Host Machine)

Clone the repo and pull the PyTorch Docker container:

git clone [https://github.com/Xilinx/Vitis-AI]
cd Vitis-AI

docker pull xilinx/vitis-ai-pytorch-cpu:latest


Steps to Enter Vitis-AI 
--open linux shell within Vitis-AI repo
--run ./docker_run.sh xilinx/vitis-ai-pytorch-cpu:latest
--enter conda-activate-vitis-ai-pytorch

we are inside vitis-ai now


2. Download the Model Zoo XModel

the link to downlaod xmodel is in the yaml file

name: yolox_nano_pt
  type: xmodel
  board: zcu102 & zcu104 & kv260
  download link: https://www.xilinx.com/bin/public/openDownload?filename=yolox_nano_pt-zcu102_zcu104_kv260-r3.0.0.tar.gz
  checksum: a13348e3ffd758531989b5dccac3b794

3. Cross-Compile the Application (Host Machine)

Source your PetaLinux SDK environment and compile `yolox_nano_custom.cpp` for ARM64:

run this in terminal- .\mpsoc\host_cross_compiler_setup.sh

# Source the cross-compilation environment
source ~petalinux/2022.2/environment-setup-cortexa72-cortexa53-xilinx-linux

after the above command executes , in the Vitis-ai repo navigate to this folder- .\Vitis-AI-3.0\examples\vai_library\samples\yolovx

there will be build.sh where you have to place your yolox_nano_custom.cpp

Run ./build.sh

# Verify architecture
file yolox_nano_app


*(Make sure the output confirms `ELF 64-bit LSB executable, ARM aarch64`)*



Running on KV260

 4. Make the connections & Copy Files to the Board


![Kria KV260 Board Setup](images/kria/jpeg)

connection:ethernet(for data transfer),usb cable(for SSH),web cam (for frames input),HDMI & mouse

for data transfer, set IP address for the board, and transfer the binary, `.xmodel`, and `.prototxt`:


scp yolox_nano_app petalinux@<KV260_IP>:/usr/share/vitis_ai_library/models/yolox_nano
scp yolox_nano.xmodel petalinux@<KV260_IP>:/usr/share/vitis_ai_library/models/yolox_nano
scp yolox_nano.prototxt petalinux@<KV260_IP>:/usr/share/vitis_ai_library/models/yolox_nano

naviagte to this path before doing scp /usr/share/vitis_ai_library/models/, this path has pre built xmodels and prototxt already , group them into anoher folder with the new executible(names should match otherwuse it will give error during runtime)

example file structure-
/usr/share/vitis_ai_library/models/
└── yolox_nano/
    ├── yolox_nano.prototxt
    ├── yolox_nano.xmodel
    └── yolox_nano_app

5. Setup Vitis-AI Model Directory Structure

SSH into the KV260. Vitis-AI requires the folder name, model file, prototxt, and binary to share matching naming conventions under `/usr/share/vitis_ai_library/models/`:



cd /usr/share/vitis_ai_library/models/yolox_nano
sudo chmod +x yolox_nano_app


6. Run Inference with HDMI Display

Ensure your monitor is plugged into the KV260 display output, set the display variable, and execute:


export DISPLAY=:0.0
./yolox_nano_app yolox_nano 0


*(Argument `0` maps to `/dev/video0` USB camera input)*


7.Results & Verification

For live detection captures and inference performance on the KV260, see [results.md](results.md).


[def]: https://github.com/Xilinx/Vitis-AI