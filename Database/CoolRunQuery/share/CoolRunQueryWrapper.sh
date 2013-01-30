export PATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/bin:\
$PATH

export ROOTSYS=/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/sw/lcg/app/releases/ROOT/5.28.00g/i686-slc5-gcc43-opt/root

export PYTHONPATH=`pwd`:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasProduction/17.0.5.6/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasConditions/17.0.5/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasCore/17.0.5/InstallArea/i686-slc5-gcc43-opt/lib/python2.6:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasCore/17.0.5/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasTrigger/17.0.5/InstallArea/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/external/ZSI/2.1-a1/lib/python:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/sw/lcg/app/releases/COOL/COOL_2_8_10b/i686-slc5-gcc43-opt/python:\
$ROOTSYS/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/sw/lcg/external/pygraphics/1.1p1_python2.6/i686-slc5-gcc43-opt/lib/python2.6/site-packages:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/sw/lcg/external/pytools/1.4_python2.6/i686-slc5-gcc43-opt/lib/python2.6/site-packages

export LD_LIBRARY_PATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasConditions/17.0.5/InstallArea/i686-slc5-gcc43-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/DetCommon/17.0.5/InstallArea/i686-slc5-gcc43-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/LCGCMT/LCGCMT_60d/InstallArea/i686-slc5-gcc43-opt/lib:\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/lib:\
/afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/lib:\
/usr/lib

# for the dbreplica.config file
export DATAPATH=\
/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/AtlasCore/17.0.5/InstallArea/share

export CORAL_DBLOOKUP_PATH=/data/atrqadm/authentication
export CORAL_AUTH_PATH=/data/atrqadm/authentication

export TNS_ADMIN=/cvmfs/atlas.cern.ch/repo/sw/software/17.0.5/DBRelease/current/oracle-admin

export HOSTNAME=$HOSTNAME

export NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1

export COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP=YES

unset HOME

printenv

`pwd`/share/AtlRunQuery.py $*

