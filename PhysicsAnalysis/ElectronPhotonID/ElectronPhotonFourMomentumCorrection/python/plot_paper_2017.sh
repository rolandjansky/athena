#!/bin/sh

## This script produces the plots used in the internal notes and in
## the calibration paper in 2017

# PES vs ET in eta-bins per particles

python plot.py sys_eta_slices --beautify-sysnames --sys-order paper_run2 --superimpose-all --skip-null-sys --esmodels es2016data_mc15c_summer_improved --min-sys-value 0 --max-sys-value 0.032 --symmetrize-labels  --pt-bins-linspace 7E3 200E3 200  --eta-bins 0.0 0.8 1.37 1.52 1.8 2.47 --super-sampling-eta 20 --abs-sys
