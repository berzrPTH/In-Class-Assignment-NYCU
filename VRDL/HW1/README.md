# VRDL_HW1

HW1 for NCTU CS Selected Topics in Visual Recognition using Deep Learning.

## Hardware

The code is running on Google Colab.  
The following specs were used to create the original solution.

- Ubuntu 18.04.5 LTS
- Intel(R) Xeon(R) CPU @ 2.30GHz
- Tesla P100-PCIE-16GB

## Requirements

```
numpy==1.18.5
pandas==1.1.4
scikit-image==0.16.2
torch==1.7.0+cu101
torchvision==0.8.1+cu101
efficientnet-pytorch==0.7.0
matplotlib==3.2.2
```

## Dataset Preparation

After downloading the data, the data directory should be structured as:

```
root
  +- training_data/training_data
  |  +- 000001.jpg
  |  +- 000002.jpg
  |  +- ...
  +- testing_data/testing_data
  |  +- 000004.jpg
  |  +- 000005.jpg
  |  +- ...
  +- training_label.csv
```

## Dataset/DataLoader

Run the cells below the **Dataset** markdown block.

## Train/Fine-tune

Run the cells below the **Train model** markdown block.

## Inference

Run the cells below the **Apply on test dataset** markdown block.

