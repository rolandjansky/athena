#!/bin/sh

# art-description: DAOD building TOPQ5 mc16
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.1/Athena

pwd
athena.py -c "REO_doAutoConfiguration=False" RecExOnline/RecExOnline_User_Offline_isOnlineTrue.py >& log_RecExOnline_User_Offline_isOnlineTrue_py_noautoconfig.log
grep "successful run" log_RecExOnline_User_Offline_isOnlineTrue_py_noautoconfig.log
