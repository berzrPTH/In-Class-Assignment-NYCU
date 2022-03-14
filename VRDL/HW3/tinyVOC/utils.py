import json
import numpy as np

def split_ann(ann_file, train_out, val_out, val_rate=0.2, rd_seed=42):
    """Split the CoCo format annotation file for training and validation.

    Args:
        ann_file (str): CoCo format annotation file path.
        train_out (str): Training annotation file path.
        val_out (str): Validation annotation file path.
        val_rate (float): Percentage of validation set.
        rd_seed (int): Seed for numpy random function.
    
    Returns:
        train_info (dict): Training annotation dictionary.
        val_info (dict): Validation annotation dictionary.
    """
    train_info = dict(annotations=[], images=[], categories=[])
    val_info = dict(annotations=[], images=[], categories=[])

    with open(ann_file, 'r') as f:
        data_info = json.load(f)

        train_info['categories'] = data_info['categories']
        val_info['categories'] = data_info['categories']

        np.random.seed(rd_seed)
        num_images = len(data_info['images'])
        choice = np.random.choice(num_images, int(num_images * val_rate), replace=False)
        mask = np.zeros(num_images, dtype=bool)
        mask[choice] = True # Set val part to True
        train_info['images'] = np.asarray(data_info['images'])[~mask].tolist()
        val_info['images'] = np.asarray(data_info['images'])[mask].tolist()

        # Do we need to split annotations too?
        train_info['annotations'] = data_info['annotations']
        val_info['annotations'] = data_info['annotations']

    json.dump(train_info, open(train_out, 'w'))
    json.dump(val_info, open(val_out, 'w'))
    return train_info, val_info
