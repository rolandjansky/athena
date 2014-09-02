#!/bin/sh

wwwdir=`dirname $0`

# Note: voatlas152 has only Python 2.5, so for now we either need a ROOT version
#       where PyROOT is available for Python 2.5 (such as 5.26.00), or we use the
#       LCG / ATLAS distribution from afs.

# Manual setup, misses numpy
#. /afs/cern.ch/sw/lcg/contrib/gcc/4.3/x86_64-slc5/setup.sh
#. /afs/cern.ch/sw/lcg/app/releases/ROOT/5.27.06/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh
#export PATH=/afs/cern.ch/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/bin:/afs/cern.ch/sw/lcg/contrib/gcc/4.3.2/x86_64-slc5-gcc34-opt/bin:${PATH}
#export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/sw/lcg/contrib/gcc/4.3.2/x86_64-slc5-gcc34-opt/lib64:${LD_LIBRARY_PATH}
#export PYTHONPATH=$wwwdir:$PYTHONPATH

# Setup based on ATLAS release (from asetup asetup 17.0.3.1,64), then copying
# minimal PATH as well as full LD_LIBRARY_PATH and PYTHONPATH
# (or in python: print '\n'.join(os.environ['PATH'].split(':')) etc)
export PATH=/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/pytools/1.4_python2.6/x86_64-slc5-gcc43-opt/bin:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/app/releases/ROOT/5.28.00e/x86_64-slc5-gcc43-opt/root/bin:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/bin:/afs/cern.ch/atlas/software/releases/17.0.3/gcc-alt-435/x86_64-slc5-gcc43-opt/bin:${PATH}
export LD_LIBRARY_PATH=/afs/cern.ch/user/a/atlidbs/atl/17.0.3.1/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasProduction/17.0.3.1/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasOffline/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasAnalysis/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasSimulation/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasTrigger/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasReconstruction/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasEvent/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasConditions/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasCore/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/DetCommon/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/GAUDI/v22r1p4/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/LCGCMT/LCGCMT_60c/InstallArea/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/MCGenerators/pythia8/150/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/valgrind/3.5.0/x86_64-slc5-gcc43-opt/lib/valgrind:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/MCGenerators/toprex/4.23/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/gcc-alt-435/x86_64-slc5-gcc43-opt/lib64:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/coin3d/3.1.3.p1/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/soqt/1.5.0.p1_qt4.6.3.p1_coin3d3.1.3.p1/x86_64-slc5-gcc43-opt/lib:/usr/lib64:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/tdaq-common/tdaq-common-01-17-02/installed/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/Java/JDK/1.6.0/amd64/jre/lib/amd64:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/Java/JDK/1.6.0/amd64/jre/lib/amd64/server:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/Java/JDK/1.6.0/amd64/jre/lib/amd64/native_threads:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/Java/JDK/1.6.0/amd64/jre/lib/amd64/xawt:/afs/cern.ch/atlas/software/releases/17.0.3/dqm-common/dqm-common-00-15-06/installed/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/qwt/5.2.1_qt4.6.3/x86_64-slc5-gcc43-opt/lib
export PYTHONPATH=/afs/cern.ch/user/a/atlidbs/atl/17.0.3.1/InstallArea/python:/afs/cern.ch/user/a/atlidbs/atl/17.0.3.1/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasProduction/17.0.3.1/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasProduction/17.0.3.1/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasOffline/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasOffline/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasAnalysis/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasAnalysis/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasSimulation/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasSimulation/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasTrigger/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasTrigger/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasReconstruction/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasReconstruction/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasEvent/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasEvent/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasConditions/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasConditions/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasCore/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/AtlasCore/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/DetCommon/17.0.3/InstallArea/x86_64-slc5-gcc43-opt/lib/python2.6:/afs/cern.ch/atlas/software/releases/17.0.3/DetCommon/17.0.3/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/GAUDI/v22r1p4/InstallArea/python:/afs/cern.ch/atlas/software/releases/17.0.3/external/ZSI/2.1-a1/lib/python:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/pygraphics/1.1p1_python2.6/x86_64-slc5-gcc43-opt/lib/python2.6/site-packages:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/external/pytools/1.4_python2.6/x86_64-slc5-gcc43-opt/lib/python2.6/site-packages:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/app/releases/COOL/COOL_2_8_10/x86_64-slc5-gcc43-opt/python:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/app/releases/CORAL/CORAL_2_3_16/x86_64-slc5-gcc43-opt/python:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/app/releases/CORAL/CORAL_2_3_16/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/sw/lcg/app/releases/ROOT/5.28.00e/x86_64-slc5-gcc43-opt/root/lib:/afs/cern.ch/user/a/atlidbs/packages/lib/python:/afs/cern.ch/atlas/software/releases/17.0.3/tdaq-common/tdaq-common-01-17-02/installed/share/lib/python:/afs/cern.ch/atlas/software/releases/17.0.3/tdaq-common/tdaq-common-01-17-02/installed/x86_64-slc5-gcc43-opt/lib:/afs/cern.ch/atlas/software/releases/17.0.3/dqm-common/dqm-common-00-15-06/installed/share/lib/python

# For testing only:
#export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
#alias asetup='source $AtlasSetup/scripts/asetup.sh'
#asetup 17.0.3.1,64

# Local setup for PyROOT (for debugging)
#export ROOTSYS=/data/package/root/current
#export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
#export PYTHONPATH=$wwwdir:$ROOTSYS/lib:$PYTHONPATH

# Show what we got
#echo 'Software setup check:'
#gcc --version
#python -V
#echo ''
#echo 'LD_LIBRARY_PATH = '$LD_LIBRARY_PATH
#echo ''
#echo 'PATH = '$PATH
#echo ''
#echo 'PYTHONPATH = ',$PYTHONPATH


#
# Remove older plots
#
if [ -d $wwwdir/tmp ] ; then
  cd $wwwdir/tmp
  find beamspot-*.gif beamspot-*.eps beamspot-*.pdf -mtime +3 -exec /bin/rm -f {} \;
fi


#
# Generate file for current request
#
$wwwdir/beamspotnt.py $*
