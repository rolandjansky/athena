#!/bin/sh

# Reco stage
if Reco_tf.py --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/22.0/v1/myRDO.pool.root --outputAODFile=Nightly_AOD_electron.pool.root --maxEvents=1 --autoConfiguration="everything" --preExec="from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyRecoFlags; setRunEgammaOnlyRecoFlags()" --postInclude "ESDtoAOD:egammaValidation/egammaArtCaloCalPostInclude.py" "POOLMergeAthenaMPAOD0:egammaValidation/egammaArtCaloCalPostInclude.py" >>/dev/null 2>&1 ; then
	echo "Reco tf success"
else
	echo "Reco tf failure"
fi

rm tmp.ESD >> /dev/null 2>&1
rm log.RAWtoESD >> /dev/null 2>&1
rm log.ESDtoAOD >> /dev/null 2>&1

# Merge stage
if AODMerge_tf.py --inputAODFile=Nightly_AOD_electron.pool.root --outputAOD_MRGFile=Nightly_AOD_electron.pool.root --preExec "from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyMergeFlags; setRunEgammaOnlyMergeFlags()" --postInclude "all:egammaValidation/egammaArtCaloCalPostInclude.py">>/dev/null 2>&1 ; then 
	echo "Merge success"
else
	echo "Merge failure"
fi
rm log.AODMerge >> /dev/null 2>&1

# Histo stage 
get_files -jo egamma_art_checker_joboptions.py >> /dev/null 2>&1
if athena -c "particleType='electron'" egamma_art_checker_joboptions.py >> /dev/null 2>&1; then
	echo "Histo maker success"
else
	echo "Histo maker failure"
fi

rm Nightly_AOD_electron.pool.root >> /dev/null 2>&1

# Final plot stage 
if  EgammaARTmonitoring_plotsMaker.py Nightly-monitoring_electron.hist.root Nightly-monitoring_electron.hist.root electron >> /dev/null 2>&1; then
	echo "Plot maker success"
else
	echo "Plot maker failure"
fi

rm *.png >> /dev/null 2>&1
rm Nightly-monitoring_electron.hist.root >> /dev/null 2>&1
rm BN_ComparisonPlots_electron.hist.root >>  /dev/null 2>&1
