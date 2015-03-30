export PATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin:\
$PATH

export ROOTSYS=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/sw/lcg/app/releases/ROOT/5.34.13/x86_64-slc6-gcc47-opt/root

export PYTHONPATH=\
`pwd`/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/AtlasTrigger/19.1.3/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/AtlasConditions/19.1.3/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/AtlasCore/19.1.3/InstallArea/x86_64-slc6-gcc47-opt/lib/python2.7:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/AtlasCore/19.1.3/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/sw/lcg/external/pygraphics/1.4_python2.7/x86_64-slc6-gcc47-opt/lib/python2.7/site-packages:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/external/ZSI/2.1-a1/lib/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/sw/lcg/external/pytools/1.8_python2.7/x86_64-slc6-gcc47-opt/lib/python2.7/site-packages:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/sw/lcg/app/releases/COOL/COOL_2_9_1a/x86_64-slc6-gcc47-opt/python:\
$ROOTSYS/lib:\

export LD_LIBRARY_PATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/AtlasConditions/19.1.3/InstallArea/x86_64-slc6-gcc47-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/DetCommon/19.1.3/InstallArea/x86_64-slc6-gcc47-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/LCGCMT/LCGCMT_67b/InstallArea/x86_64-slc6-gcc47-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/lib:\
/afs/cern.ch/sw/lcg/external/gcc/4.7.2/x86_64-slc6/lib64:\
/usr/lib64

# for the dbreplica.config file
export DATAPATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/AtlasCore/19.1.3/InstallArea/share

export CORAL_DBLOOKUP_PATH=`pwd`/authentication
export CORAL_AUTH_PATH=`pwd`/authentication

export TNS_ADMIN=/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/DBRelease/current/oracle-admin

export HOSTNAME=$HOSTNAME

export NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1

export COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP=YES

unset HOME

printenv

`pwd`/share/AtlRunQuery.py $*

