######################################################################
##
## Check out 
##
######################################################################


setupATLAS 
lsetup git
mkdir testdir
cd testdir
git atlas init-workdir https://:@gitlab.cern.ch:8443/giulini/athena.git
cd athena/
git atlas addpkg FTKStandaloneMonitoring
git fetch upstream 
git checkout -b FTKSM-ckeckout upstream/21.1 --no-track
cd ..

######################################################################
##
##  Compilation
##
######################################################################

##for old athena releases e.g. 21.0.17,AtlasHLT
asetup 21.0.17,AtlasHLT,here
mv CMakeLists.txt athena/
cd athena
rm -r Projects
mv Trigger/TrigFTK/FTKStandaloneMonitoring/ .
rm -r Trigger
mkdir build
cd build/
cmake ../athena/
make

##for new athena releases
asetup 21.1.2,AthenaP1,here
mkdir build
cd build/
cmake ../athena/Projects/WorkDir/

#####################################################################
##
##  Setup and Running
##
######################################################################

source x86*/setup.sh
cd x86*/bin

## At lxplus
./MonitoringFTKHWSWComparison.exe -f /eos/user/a/amaranti/FTK_DQM_tests/21.0.11_lxplus/OUT.BS_FTK.root -n /eos/user/a/amaranti/FTK_DQM_tests/20.11.2.6.2_AtlasP1MON_lxplus/FTKIPtoFTK/OUT.NTUP.root -s -v
## At P1
./MonitoringFTKHWSWComparison -f ../../../files/alex.OUT.BS_FTK.root -n ../../../files/alex.OUT.NTUP.root
## to be added if you are not running within the partition
# -p PARTITION_NAME 

