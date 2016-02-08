PATCHAREA=\
/data/patchArea/InstallArea

export PATH=\
$PATCHAREA/share/bin:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/sw/lcg/releases/LCG_72a/Python/2.7.6/x86_64-slc6-gcc48-opt/bin:\
$PATH

export ROOTSYS=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/sw/lcg/releases/LCG_72a/ROOT/5.34.25/x86_64-slc6-gcc48-opt


export PYTHONPATH=\
`pwd`/python:\
$PATCHAREA/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/AtlasTrigger/20.1.6/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/AtlasConditions/20.1.6/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/AtlasCore/20.1.6/InstallArea/x86_64-slc6-gcc48-opt/lib/python2.7:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/AtlasCore/20.1.6/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/sw/lcg/releases/LCG_72a/pygraphics/1.4_python2.7/x86_64-slc6-gcc48-opt/lib/python2.7/site-packages:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/sw/lcg/releases/LCG_72a/pytools/1.8_python2.7/x86_64-slc6-gcc48-opt/lib/python2.7/site-packages:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/sw/lcg/releases/LCG_72a/COOL/COOL_2_9_5/x86_64-slc6-gcc48-opt/python:\
$ROOTSYS/lib:\


export LD_LIBRARY_PATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/AtlasConditions/20.1.6/InstallArea/x86_64-slc6-gcc48-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/DetCommon/20.1.6/InstallArea/x86_64-slc6-gcc48-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/LCGCMT/LCGCMT_72a/InstallArea/x86_64-slc6-gcc48-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/sw/lcg/releases/LCG_72a/Python/2.7.6/x86_64-slc6-gcc48-opt/lib:\
/afs/cern.ch/sw/lcg/releases/LCG_72a/gcc/4.8.1/x86_64-slc6/lib64:\
/usr/lib64

# for the dbreplica.config file
export DATAPATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.6/AtlasCore/20.1.6/InstallArea/share

export CORAL_DBLOOKUP_PATH=/data/authentication
export CORAL_AUTH_PATH=/data/authentication

export TNS_ADMIN=/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/DBRelease/current/oracle-admin

export HOSTNAME=$HOSTNAME

export NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1

export COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP=YES

unset HOME

printenv

`pwd`/share/AtlRunQuery.py $*

