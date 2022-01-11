#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 and RUN4 geometry, minimum bias
# art-include: master/Athena
# art-type: grid
# art-output: test_minbias.HITS.pool.root
# art-output: test_minbias.HITS_FILT.pool.root

Input=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/EVNT/mc15_14TeV.800381.Py8EG_A3NNPDF23LO_minbias_inelastic_high_keepJets.evgen.EVNT.e8205/EVNT.22128782._000036.pool.root.1
Output=test_minbias.HITS.pool.root

# RUN4 setup
# ATLAS-P2-ITK-24-00-00 and OFLCOND-MC15c-SDR-14-05
Sim_tf.py \
--multithreaded \
--CA \
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-05' \
--simulator 'FullG4MT' \
--postInclude 'default:PyJobTransforms.UseFrontier' \
--preInclude 'EVNTtoHITS:Campaigns.PhaseIISimulation' \
--geometryVersion 'default:ATLAS-P2-ITK-24-00-00' \
--inputEVNTFile $Input \
--outputHITSFile $Output \
--maxEvents 10 \
--imf False

rc=$?
status=$rc
echo "art-result: $rc simCA"

rc2=-9999
if [ $rc -eq 0 ]
then
  FilterHit_tf.py \
  --CA \
  --TruthReductionScheme SingleGenParticle \
  --inputHITSFile $Output \
  --outputHITS_FILTFile test_minbias.HITS_FILT.pool.root

  rc2=$?
  status=$rc2
fi

echo "art-result: $rc2 FilterHitCA"

exit $status
