#!/bin/sh

if Reco_tf.py --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/22.0/v1/myRDO.pool.root --outputAODFile=Nightly_AOD_electron.pool.root --maxEvents=1 --autoConfiguration="everything" --preExec="from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyRecoFlags; setRunEgammaOnlyRecoFlags()" --postInclude "ESDtoAOD:egammaValidation/egammaArtCaloCalPostInclude.py" "POOLMergeAthenaMPAOD0:egammaValidation/egammaArtCaloCalPostInclude.py" >>/dev/null 2>&1 ; then
	echo "Reco tf success"
else
	echo "Reco tf failure"
fi

if AODMerge_tf.py --inputAODFile=Nightly_AOD_electron.pool.root --outputAOD_MRGFile=Nightly_AOD_electron.pool.root --preExec "from egammaValidation.egammaOnlyPreExec import setRunEgammaOnlyMergeFlags; setRunEgammaOnlyMergeFlags()" --postInclude "all:egammaValidation/egammaArtCaloCalPostInclude.py">>/dev/null 2>&1 ; then 
	echo "Merge success"
else
	echo "Merge failure"
fi

get_files -jo egamma_art_checker_joboptions.py >> /dev/null
if athena -c "particleType='electron'" egamma_art_checker_joboptions.py >> /dev/null 2>&1; then
	echo "Plot maker success"
else
	echo "Plot maker failure"
fi


