#!/bin/sh
#
# art-description: RDO to BS on MC tt MC12
# art-type: grid

Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/RDO.01177879._000001.pool.root.1 --outputBSFile mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.data --autoConfiguration everything --maxEvents -1 --conditionsTag='OFLCOND-RUN12-SDR-31' --geometryVersion='ATLAS-R2-2015-03-01-00'

