#!/bin/sh
#
# art-description: Reco_tf runs on mc12 with Hephaestos for memory leak checking
# art-type: grid

export LD_PRELOAD=/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/sw/lcg/releases/libunwind/5c2cade-76996/x86_64-slc6-gcc62-opt/lib/libunwind.so; Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/RDO.01572821._000019.pool.root.1 --conditionsTag='OFLCOND-RUN12-SDR-31' --geometryVersion='ATLAS-R2-2015-03-01-00' --maxEvents 10 --outputESDFile myESD.pool.root --athenaopts '--stdcmalloc --leak-check-execute'

