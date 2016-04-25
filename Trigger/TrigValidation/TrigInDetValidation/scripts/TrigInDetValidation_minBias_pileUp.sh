#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_MonitorSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_minBias_pileUp";EventMax=1000;' TrigInDetValidation_RTT_topOptions_MonitorSlice.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-monitor-pileup.root -b Test_bin.dat


