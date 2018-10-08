#!/bin/sh
#
# art-description: Reco_tf runs on mc16 with Hephaestos for memory leak checking
# art-type: grid

export LD_PRELOAD=$LCG_RELEASE_BASE/libunwind/5c2cade-76996/$LCG_PLATFORM/lib/libunwind.so; Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.RDO.e5458_s3126_d1437/RDO.11426804._000001.pool.root.1 --conditionsTag=default:OFLCOND-MC16-SDR-16 --geometryVersion=default:ATLAS-R2-2016-01-00-01 --runNumber=410501 --maxEvents 10 --outputESDFile myESD.pool.root --athenaopts '--stdcmalloc --leak-check-execute'

echo "art-result: $?"

