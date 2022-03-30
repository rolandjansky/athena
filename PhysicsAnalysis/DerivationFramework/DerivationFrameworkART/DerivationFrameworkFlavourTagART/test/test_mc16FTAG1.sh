#!/bin/sh

# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-description: DAOD building FTAG1 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

INPUT=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1

Reco_tf.py --inputAODFile $INPUT --outputDAODFile art.pool.root --reductionConf FTAG1 --maxEvents 100 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff();'

echo "art-result: $? reco"

checkFile.py art.pool.root > checkFile.txt
