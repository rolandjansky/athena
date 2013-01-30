export PATH=\
/afs/cern.ch/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/bin:\
$PATH

export ROOTSYS=/data/atrqadm/sw/root

export PYTHONPATH=`pwd`:\
/data/atrqadm/sw/InstallArea/python:\
/afs/cern.ch/atlas/software/builds/AtlasConditions/16.0.1/InstallArea/python:\
/afs/cern.ch/atlas/software/builds/AtlasCore/16.0.1/InstallArea/i686-slc5-gcc43-opt/lib/python2.6:\
/afs/cern.ch/atlas/software/builds/AtlasCore/16.0.1/InstallArea/python:\
/afs/cern.ch/atlas/software/builds/AtlasTrigger/16.0.1/InstallArea/python:\
/afs/cern.ch/atlas/offline/external/ZSI/2.1-a1/lib/python:\
/afs/cern.ch/sw/lcg/app/releases/COOL/COOL_2_8_7/i686-slc5-gcc43-opt/python:\
$ROOTSYS/lib:\
/afs/cern.ch/sw/lcg/external/pytools/1.2_python2.6/i686-slc5-gcc43-opt/lib/python2.6/site-packages

export LD_LIBRARY_PATH=\
/data/atrqadm/sw/InstallArea/i686-slc5-gcc43-opt/lib:\
/afs/cern.ch/atlas/software/builds/DetCommon/16.0.1/InstallArea/i686-slc5-gcc43-opt/lib:\
/afs/cern.ch/atlas/offline/external/LCGCMT/LCGCMT_59a/InstallArea/i686-slc5-gcc43-opt/lib:\
/afs/cern.ch/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/lib:\
/afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/lib:\
/usr/lib

export CORAL_DBLOOKUP_PATH=/data/atrqadm/authentication
export CORAL_AUTH_PATH=/data/atrqadm/authentication

export TNS_ADMIN=/afs/cern.ch/sw/lcg/external/oracle/11.2.0.1.0p2/admin
export HOSTNAME=$HOSTNAME

export NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1

export COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP=YES

`pwd`/share/AtlRunQuery.py $*

