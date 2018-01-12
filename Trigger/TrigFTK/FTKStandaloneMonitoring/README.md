# Reference

Explanation of the package structure in [FTKStandaloneMonitoringDoc](https://twiki.cern.ch/twiki/bin/view/Atlas/FTKStandaloneMonitoringDoc)


# Check out (lxplus only)

```
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
```

#  Compilation

if not already done
```
setupATLAS 
lsetup git
```

for old athena releases e.g. 21.0.17,AtlasHLT
```
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
```

for new athena releases
```
asetup 21.1.2,AthenaP1,here
mkdir build
cd build/
cmake ../athena/Projects/WorkDir/
make
```
#  Setup and Running

* At lxplus

```
source x86*/setup.sh
cd x86*/bin

./MonitoringFTKHWSWComparison -f /eos/user/a/amaranti/FTK_DQM_tests/21.0.11_lxplus/OUT.BS_FTK.root -n /eos/user/a/amaranti/FTK_DQM_tests/20.11.2.6.2_AtlasP1MON_lxplus/FTKIPtoFTK/OUT.NTUP.root -s -v
```
* At P1

first setup athena at P1:
```
source /det/ftk/ftk_setup.sh FTK-02-00-01
export TDAQ_PARTITION=FTK-DQM-HWSWComparison
alias asetup='. /sw/atlas/AtlasSetup/scripts/asetup.sh'
asetup 21.1.2,AthenaP1,here
```
assuming you are in testdir
```
source build/x86*/setup.sh
cd build/x86*/bin
```

running
```
./MonitoringFTKHWSWComparison -f /atlas-home/0/giulini/xTakashiAlex/files/alex.OUT.BS_FTK.root -n /atlas-home/0/giulini/xTakashiAlex/files/alex.OUT.NTUP.root
```
to be added if you are not running within the partition
```
 -p PARTITION_NAME 
```

to see the histograms:
```
oh_display 
```

click on Histogram repository and then on the partition name and again on the partition name

#  Setup and running script from offline release at P1


```
source /det/ftk/ftk_setup.sh FTK-02-00-04
export TDAQ_PARTITION=FTK-DQM-HWSWComparison
alias asetup='. /sw/atlas/AtlasSetup/scripts/asetup.sh'
asetup 21.1.4,AthenaP1,here
MonitoringFTKHWSWComparison -f /atlas-home/0/giulini/xTakashiAlex/files/alex.OUT.BS_FTK.root -n /atlas-home/0/giulini/xTakashiAlex/files/alex.OUT.NTUP.root -p $TDAQ_PARTITION 
```
or without -p option if run within the partition
```
oh_display 
```
click on Histogram repository and then on the partition name and again on the partition name
