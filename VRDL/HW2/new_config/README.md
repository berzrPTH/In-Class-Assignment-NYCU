# New config and checkpoint

The task is to ditect numbers, and it does not sound reasonable to flip the image. Hence, we remove the flip in the data augmentation and replace it with photometric distortion.  
At the same time because most of the anchors have aspect ratio > 1, we set the ratio to 1.5, 2.0 and 3.0.

The checkpoint of model can be downloaded [here](https://drive.google.com/file/d/1-EzroybLIuCm0yWlZp7XJS0HPC8jp65f/view?usp=sharing).
