#! /bin/sh
# Customised environment
athena.py --preloadlib=/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/2018-01-23T0022/AthenaExternals/21.0.61/InstallArea/x86_64-slc6-gcc62-opt/lib/libintlc.so.5:/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/2018-01-23T0022/AthenaExternals/21.0.61/InstallArea/x86_64-slc6-gcc62-opt/lib/libimf.so --drop-and-reload runargs.ESDtoAOD.py RecJobTransforms/skeleton.ESDtoAOD_tf.py
