# VRDL_HW3

The code for the assignment of Selected Topics in Visual Recognition using Deep Learning, NCTU, in fall 2020.

## Abstract

The jupyter notebook contains all the workflow, and all the work is done on Google Colab.  
You can follow the notebook to check the detail if you want.

## Hardware

The model is all training on Google Colab.  
The following specs were used to create the original solution.

- Ubuntu 18.04.5 LTS
- Intel(R) Xeon(R) CPU @ 2.30GHz
- NVidia Tesla T4

## Requirements

* pytorch 1.6.0+cu101
* torchvision 0.7.0+cu101
* albumentation 0.5.2
* imgaug-0.4.0
* other requirements of MMDetection

## Preparation

Please install the MMDetection first, which you can find the tutorial on their GitHub site.

Your working directory should be similar as follow.

```
.
{WORK_DIR}
+-- tinyVOC
|   +-- __init__.py
|   +-- utils.py
+-- mask_rcnn_r50_fpn_40e_tinyVOC.py
+-- mmdetection
+-- data
|   +-- annotations
|   |   +-- classes.txt
|   |   +-- instances_train.json
|   |   +-- instances_val.json
|   |   +-- pascal_train.json
|   |   +-- test.json
|   +-- train_images
|   |   +-- xxx.jpg
|   |   +-- yyy.jpg
|   |   +-- ...
|   +-- test_images
|   |   +-- zzz.jpg
|   |   +-- www.jpg
|   |   +-- ...
+-- ...
.
```

## Train

```
python ./mmdetection/tools/train.py ./mask_rcnn_r50_fpn_40e_tinyVOC.py
```

## Test

Replace the `${CHECKPOINT}` with the checkpoint file corresponding to the configuration.  
The checkpoint of 40e (40 epochs) we trained could be download [here](https://drive.google.com/file/d/12kAEsDYkPPB6lKOg567rLqXIr6PiCs-b/view?usp=sharing).

```
python ./mmdetection/tools/test.py ./mask_rcnn_r50_fpn_40e_tinyVOC.py ${CHECKPOINT}
```
