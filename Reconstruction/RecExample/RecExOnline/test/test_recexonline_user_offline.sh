#!/bin/sh

# art-description: DAOD building TOPQ5 mc16
# art-type: build

pwd
athena.py -c "inpuevtMax=50" RecExOnline/RecExOnline_User_Offline.py >& log_RecExOnline_User_Offline_py.log
grep "successful run" log_RecExOnline_User_Offline_py.log
