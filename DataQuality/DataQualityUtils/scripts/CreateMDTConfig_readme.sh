#!/bin/bash --norc
#
# Author Paolo Bagnaia <paolo.bagnaia@cern.ch>
# and Elena Solfaroli Camillocci <elena.solfaroli.camillocci@cern.ch>
# MANUAL
#
cat << E_O_F
#
# readme for CreateMDTConfig package - PB + ESC 100108 v 3.1.2
#
# !!! ROOT has to be configured !!!
#
# CreateMDTConfig_chambers.sh [-i input1-root-file] [-o output1-text-file]
# CreateMDTConfig_algos.sh -c chamber_list_input
#           [ output NOT changeable -o g_output_temp.txt]
# CreateMDTConfig_config.sh -i input2-text-file -o output2-text-file
#         -r reference-root-file [-d "BA BC EA EC"] [-s 1-16] [-h]
# CreateMDTConfig_files.sh tag
#
# where :
#
# input2-text-file = output1-text-file
#
# if [-i file] is missing, then input from stdin
# if [-o file] is missing, then output to  stdout
# if [-r file] is missing, then it searches for "ref_file.root" in the dir
#
# =================================================================
# The "-d" option requires a list of detectors, included in " " :
# Barrel_A : BA (default, if option "d" is missing)
# Barrel_C : BC
# Endcap_A : EA
# Endcap_C : EC
#
# example to select Barrel_C and Endcap_A : -d "EA BC"
# =================================================================
#
# Some files are now included in "CreateMDTConfig_files.sh".
# They are prototypes for the config files for a generic chamber.
# the elements "___xxx" are replaced by the specific barrel, sector
# and chamber names (e.g. "MDT/___barr/___sect/___name" becomes
# "MDT/Barrel_C/Sector07/BIL1C07". Their tags are :
# a_detect.config        : detector algorithms
# b_overview.config      : OVERVIEW
# c_chamber.config       : chamber algorithms
# d_trigdaq.config       : sector algorithms
# e_output.config        : chamber output
# f_trigdaq.config       : trigger and daq part
# h_refe_algo_thr.config : algorithm definitions and thresholds
#
# ================================================================
#
# Example :
#
# !!! ROOT has to be configured !!!
#
# ./CreateMDTConfig_chambers -i Run_FDR2_Run52306_Sect1to10_effi.root -o temp.txt
# ./CreateMDTConfig_algos -c temp.txt
# ./CreateMDTConfig_config -i temp.txt -o file.config -r ref.root -d "BA EC" -s 1-16
# rm -f temp.txt
#
# [for smart people, there is also another running mode with a "|",
#  but smart people does not need examples]
#
# ================================================================
#
# Enjoy it.
#
#               Elena & Paolo
#
E_O_F
