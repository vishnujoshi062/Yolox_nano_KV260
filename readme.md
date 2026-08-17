YOLOX-Nano on Kria KV260 (Vitis-AI Deployment)

```markdown
# YOLOX-Nano on Kria KV260 (Vitis-AI Deployment)

Deployment notes and code for running real-time YOLOX-Nano object detection on the AMD Xilinx Kria KV260 Vision AI Starter Kit using Vitis-AI and PetaLinux.

---

## Directory Structure

```text
Yolox_nano_KV260/
├── images/                                 # Hardware and live detection captures
├── mpsoc/                                  # PetaLinux BSP & host compiler setup files
├── pt_yolox-nano_coco_416_416_1G_3.0/      # Downloaded Model Zoo artifacts & .xmodel
├── yolox_nano_custom.cpp                   # C++ application using Vitis-AI runtime
├── results.md                              # Live inference verification & results
└── README.md

```

---

## Deployment Steps

### 1. Vitis-AI Setup (Host Machine)

Clone the repository and pull the PyTorch Docker container:

```bash
git clone [https://github.com/Xilinx/Vitis-AI](https://github.com/Xilinx/Vitis-AI)
cd Vitis-AI
docker pull xilinx/vitis-ai-pytorch-cpu:latest

```

**Steps to Enter Vitis-AI:**

1. Open a Linux shell inside the `Vitis-AI` repository directory.
2. Launch the container:
```bash
./docker_run.sh xilinx/vitis-ai-pytorch-cpu:latest

```


3. Activate the PyTorch environment:
```bash
conda activate vitis-ai-pytorch

```



---

### 2. Download the Model Zoo XModel

The download link for the compiled `.xmodel` is located in the model YAML file:

* **Model Name:** `yolox_nano_pt`
* **Target Board:** ZCU102 / ZCU104 / KV260
* **Direct Package Download:** [yolox_nano_pt-zcu102_zcu104_kv260-r3.0.0.tar.gz](https://www.xilinx.com/bin/public/openDownload?filename=yolox_nano_pt-zcu102_zcu104_kv260-r3.0.0.tar.gz)
* **Checksum:** `a13348e3ffd758531989b5dccac3b794`

Extract the downloaded archive on your host machine to obtain `yolox_nano.xmodel` and `yolox_nano.prototxt`.

---

### 3. Cross-Compile the Application (Host Machine)

1. Run the host environment setup script:
```bash
./mpsoc/host_cross_compiler_setup.sh

```


2. Source the cross-compilation environment:
```bash
source ~/petalinux/2022.2/environment-setup-cortexa72-cortexa53-xilinx-linux

```


3. Navigate to the sample directory inside the Vitis-AI repository:
```bash
cd ./Vitis-AI/examples/vai_library/samples/yolovx

```


4. Place `yolox_nano_custom.cpp` into this directory and compile:
```bash
./build.sh

```


5. Verify that the output binary is compiled for ARM64:
```bash
file yolox_nano_app

```


*Expected output snippet:* `ELF 64-bit LSB executable, ARM aarch64`

---

## Running on KV260

### 4. Hardware Connections & File Transfer

**Required Connections:**

* **Ethernet Cable:** For data transfer / SCP
* **Micro-USB Cable:** For UART / SSH serial terminal
* **USB Web Camera:** For live video frame input (`/dev/video0`)
* **HDMI Monitor & Mouse:** For live graphical display output

**Transfer Files to the Board:**
Set the IP address for the board, then copy the compiled binary, `.xmodel`, and `.prototxt` to the board:

```bash
scp yolox_nano_app petalinux@<KV260_IP>:/usr/share/vitis_ai_library/models/yolox_nano/
scp yolox_nano.xmodel petalinux@<KV260_IP>:/usr/share/vitis_ai_library/models/yolox_nano/
scp yolox_nano.prototxt petalinux@<KV260_IP>:/usr/share/vitis_ai_library/models/yolox_nano/

```

> **Note:** Vitis AI requires matching names for the folder, executable, `.xmodel`, and `.prototxt`. Otherwise, runtime errors will occur.

**Target Directory Layout:**

```text
/usr/share/vitis_ai_library/models/
└── yolox_nano/
    ├── yolox_nano.prototxt
    ├── yolox_nano.xmodel
    └── yolox_nano_app

```

---

### 5. Setup Permissions on the Board

SSH into the KV260 and make the binary executable:

```bash
cd /usr/share/vitis_ai_library/models/yolox_nano
sudo chmod +x yolox_nano_app

```

---

### 6. Run Inference with HDMI Display

Ensure your monitor is plugged into the KV260 display output, set the display variable, and run the binary:

```bash
export DISPLAY=:0.0
./yolox_nano_app yolox_nano 0

```

*(Argument `0` maps to the `/dev/video0` USB camera input)*

---

## 7. Results & Verification

For live detection captures and inference performance logs on the KV260, see [results.md](results.md).

```

```
