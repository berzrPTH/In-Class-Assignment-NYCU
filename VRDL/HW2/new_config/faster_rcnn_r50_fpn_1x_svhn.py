# The new config inherits a base config to highlight the necessary modification
_base_ = './mmdetection/configs/faster_rcnn/faster_rcnn_r50_fpn_1x_coco.py'

# Modify model related settings
model = dict(
    rpn_head=dict(
        anchor_generator=dict(
            ratios=[1.5, 2.0, 3.0])), # Change from [0.5, 1.0, 2.0], base on analyzation on dataset
    roi_head=dict(
        bbox_head=dict(num_classes=10)))
test_cfg = dict(
    rcnn=dict(
        nms=dict(type='soft_nms', iou_threshold=0.5, min_score=0.05))) # Using soft-NMS when test

# Modify dataset related settings
img_norm_cfg = dict(
    mean=[123.675, 116.28, 103.53], std=[58.395, 57.12, 57.375], to_rgb=True)
# Change scale, remove RandomFlip, and add PhotoMetricDistortion
train_pipeline = [
    dict(type='LoadImageFromFile', to_float32=True), # to_float32=True for PhotoMetricDistortion
    dict(type='LoadAnnotations', with_bbox=True),
    dict(
        type='PhotoMetricDistortion',
        brightness_delta=32,
        contrast_range=(0.5, 1.5),
        saturation_range=(0.5, 1.5),
        hue_delta=18),
    dict(type='Resize', img_scale=(800, 400), keep_ratio=True), # Change scale
    dict(type='RandomFlip', flip_ratio=0.0), # No need flip
    dict(type='Normalize', **img_norm_cfg),
    dict(type='Pad', size_divisor=32),
    dict(type='DefaultFormatBundle'),
    dict(type='Collect', keys=['img', 'gt_bboxes', 'gt_labels']),
]
test_pipeline = [
    dict(type='LoadImageFromFile'),
    dict(
        type='MultiScaleFlipAug',
        img_scale=(800, 400), # Change scale
        flip=False,
        transforms=[
            dict(type='Resize', keep_ratio=True),
            dict(type='RandomFlip'),
            dict(type='Normalize', **img_norm_cfg),
            dict(type='Pad', size_divisor=32),
            dict(type='ImageToTensor', keys=['img']),
            dict(type='Collect', keys=['img']),
        ])
]
classes = ('1', '2', '3', '4', '5', '6', '7', '8', '9', '0')
data_root = './data/'
data = dict(
    samples_per_gpu=8,
    workers_per_gpu=4,
    train=dict(
        ann_file=data_root + 'train/annotation_coco_train.json',
        img_prefix=data_root + 'train/',
        pipeline=train_pipeline,
        classes=classes),
    val=dict(
        ann_file=data_root + 'train/annotation_coco_val.json',
        img_prefix=data_root + 'train/',
        pipeline=test_pipeline,
        classes=classes),
    test=dict(
        ann_file=data_root + 'test/annotation_coco_test.json',
        img_prefix=data_root + 'test/',
        pipeline=test_pipeline,
        classes=classes))

# Modify learning rate related settings
optimizer = dict(type='SGD', lr=0.01, momentum=0.9, weight_decay=0.0001) # 0.02/8*num_gpu*samples_per_gpu/2
total_epochs = 12 # Default 12

# Modify runtime related settings
load_from = None
resume_from = None
workflow = [('train', 3), ('val', 1)] # We evaluate performance on training set by mAP, and evaluate the loss on validation set
