#!/bin/csh
#- auto-testing procedure for MuonGeoModel - comparing with reference files 
echo "\n Start auto-test procedure for MuonGeoModel at `date` \n"
cd ../test
#- produce test-maps with the current code
athena -c 'DetDescrVersion="DC2"'          MuonGeoModelTestMaps.py
echo "\n Athena job on DC2 (P03) DONE \n "
athena -c 'DetDescrVersion="Rome-Initial"' MuonGeoModelTestMaps.py
echo "\n Athena job on Rome-Initial (Q02initial) DONE \n "
athena -c 'DetDescrVersion="Rome-Final"'   MuonGeoModelTestMaps.py
echo "\n Athena job on Rome-Final (Q02) DONE "
echo "\n Output test-maps "
ls -la *current*
echo "\n"

#- compile and build the checking tool 
g++ -o CheckDifferences CheckDifferences.cxx
ls -al CheckDifferences

#- check differences between current and reference test-maps
rm -Rf CheckDiff_log
#-
rm -f CheckDiff_log
touch CheckDiff_log
echo "\n Start Checking differences "
#CheckDifferences CSC csc_DC2SIM_reference csc_current_P03 >>  CheckDiff_log  ! this is known to have changed !!!!
CheckDifferences MDT mdt_DC2SIM_reference mdt_current_P03 >> CheckDiff_log
CheckDifferences RPC rpc_DC2SIM_reference rpc_current_P03 >> CheckDiff_log
CheckDifferences TGC tgc_DC2SIM_reference tgc_current_P03 >> CheckDiff_log
echo "\n DC2 (P03) differences checked "
CheckDifferences CSC csc_RomeFinal_reference csc_current_Q02 >> CheckDiff_log
CheckDifferences MDT mdt_RomeFinal_reference mdt_current_Q02 >> CheckDiff_log
CheckDifferences RPC rpc_RomeFinal_reference rpc_current_Q02 >> CheckDiff_log
CheckDifferences TGC tgc_RomeFinal_reference tgc_current_Q02 >> CheckDiff_log
echo "\n Rome-Final (Q02) differences checked "
CheckDifferences CSC csc_RomeInitial_reference csc_current_Q02_initial >> CheckDiff_log
CheckDifferences MDT mdt_RomeInitial_reference mdt_current_Q02_initial >> CheckDiff_log
CheckDifferences RPC rpc_RomeFinal_reference rpc_current_Q02 >> CheckDiff_log
CheckDifferences TGC tgc_RomeFinal_reference tgc_current_Q02 >> CheckDiff_log
echo "\n Rome-Initial (Q02initial) differences checked "
#-
echo "\n DONE auto-test procedure for MuonGeoModel  at `date` \n"
exit 1 
