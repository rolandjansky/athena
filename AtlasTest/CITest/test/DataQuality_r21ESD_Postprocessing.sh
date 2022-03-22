#!/bin/bash

echo ../DataQuality_r21ESD/DataQuality_r21ESD_HIST.root >> tomerge.txt && \
DQ_POSTPROCESS_ERROR_ON_FAILURE=1 DQHistogramMerge tomerge.txt merged.root 1