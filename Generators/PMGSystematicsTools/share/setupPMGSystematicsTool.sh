source ${LCG_RELEASE_BASE}/LCG_88/MCGenerators/rivet/${RIVETVER}/${LCG_PLATFORM}/rivetenv.sh
export RIVET_ANALYSIS_PATH=$RIVET_ANALYSIS_PATH:${LCG_RELEASE_BASE}/LCG_88/MCGenerators/rivet/${RIVETVER}/${LCG_PLATFORM}/share/Rivet
export PYTHONPATH=$PYTHONPATH:/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.6/sw/lcg/releases/LCG_88/MCGenerators/lhapdf/6.2.1/x86_64-slc6-gcc62-opt/lib/python2.7/site-packages
export PATH=$PATH:/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.6/sw/lcg/releases/LCG_88/MCGenerators/lhapdf/6.2.1/x86_64-slc6-gcc62-opt/bin/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.6/sw/lcg/releases/LCG_88/MCGenerators/lhapdf/6.2.1/x86_64-slc6-gcc62-opt/lib
if [ -e /usr/lib64/atlas/libsatlas.so ]; then
   workaroundLib="`pwd`/extraLibs"
   if [ ! -e $workaroundLib ]; then
     mkdir -p $workaroundLib
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libptf77blas.so.3
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libptcblas.so.3
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libatlas.so.3
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/liblapack.so.3 
   # do the same for any other atlas lib that is missing and needed
   fi
   export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$workaroundLib"
fi
