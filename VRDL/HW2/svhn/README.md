# Tools for SVHN

This folder contains tools that allow us to convert the annotation file to the type we want

Original data given from TAs contains `digitStruct.mat` which contains the annotation information.

`svhn_dataextract_tojson.py` is a tool for extracting the SVHN groundtruth from the provided matlab file and save them as a JSON file. Use it like:

```
python svhn_dataextract_tojson.py -f ${MAT_FILE} -o ${OUT_FILE}
```

`utils.py` contains useful function for data preparation.
