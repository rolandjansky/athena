#!/bin/sh

# art-description: DAOD building TOPQ5 mc16
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.1/AthenaP1

pwd
athena.py -c "inpuevtMax=500" RecExOnline/RecExOnline_User_Offline.py >& log_RecExOnline_User_Offline_py.log
grep "successful run" log_RecExOnline_User_Offline_py.log
grep "FATAL" log_RecExOnline_User_Offline_py.log
grep "Traceback" log_RecExOnline_User_Offline_py.log
grep "ERROR" log_RecExOnline_User_Offline_py.log
root -b -q /afs/cern.ch/user/y/yunju/public/ARTRootMacro/RecExOnline_Test_Obj_His_RTT_Cpp_rename.C+ >& log_RecExOnline_Test_Obj_His_RTT_Cpp_rename_C.log
day=$(date +"%u")
prefix="/eos/user/y/yunju/ARTjobs/Rel_"
wbmserver=$prefix$day
rm -rf $wbmserver/*
cp *.png $wbmserver
