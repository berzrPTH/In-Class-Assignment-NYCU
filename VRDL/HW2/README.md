# VRDL_HW2

The code for the assignment of Selected Topics in Visual Recognition using Deep Learning, NCTU, in fall 2020.

## Abstract

The jupyter notebook contains all the workflow, and all the work is done on Google Colab.  
You can follow the notebook to check the detail if you want.

## Hardware

The model is all training on Google Colab.  
The following specs were used to create the original solution.

- Ubuntu 18.04.5 LTS
- Intel(R) Xeon(R) CPU @ 2.30GHz
- Tesla P100-PCIE-16GB

## Requirements

As the requirements of MMDetection

## Preparation

```
.
{WORK_DIR}
+-- svhn
|   +-- __init__.py
|   +-- utils.py
|   +-- svhn_dataextract_tojson.py
+-- faster_rcnn_r50_fpn_1x_svhn.py
+-- mmdetection
+-- data
|   +-- train
|   |   +-- 1.png
|   |   +-- 2.png
|   |   +-- ...
|   |   +-- digitStruct.mat
|   |   +-- annotation_coco_train
|   |   +-- annotation_coco_val
|   +-- test
|   |   +-- 1.png
|   |   +-- 2.png
|   |   +-- ...
|   |   +-- annotation_coco_test
+-- ...
.
```

## Train

```
python ./mmdetection/tools/train.py ./faster_rcnn_r50_fpn_1x_svhn.py
```

## Test

Replace the `${CHECKPOINT}` with the checkpoint file corresponding to the configuration.  
The checkpoint of 1x (12 epochs) we trained could be download [here](https://drive.google.com/file/d/1-NgPX85Eb0IGGFyM0Lb4kxlAqfhLqvhk/view?usp=sharing).

```
python ./mmdetection/tools/test.py ./faster_rcnn_r50_fpn_1x_svhn.py ${CHECKPOINT}
```
