#!/usr/bin/env bash
# art-type: grid
# art-description: test job ttFC_fullSim_fullDigi + ttFC_reco_newTracking_PseudoT_fullSim_fullDigi

FastChain_tf.py --simulator ATLFASTII --digiSteeringConf "SplitNoMerge" --useISF True --randomSeed 123 --enableLooperKiller True --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root --outputRDOFile RDO_pileup_fullsim_fulldigi.pool.root --maxEvents 50 --skipEvents 0 --geometryVersion ATLAS-R2-2015-03-01-00 --conditionsTag OFLCOND-RUN12-SDR-31 --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True' --postInclude='PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py,DigitizationTests/postInclude.RDO_Plots.py' --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' --DataRunNumber '284500'

echo "art-result: $? RDO step"

FastChain_tf.py --maxEvents 500 --skipEvents 0 --geometryVersion ATLAS-R2-2015-03-01-00 --conditionsTag OFLCOND-RUN12-SDR-31  --inputRDOFile RDO_pileup_fullsim_fulldigi.pool.root --outputAODFile AOD_newTracking_pseudoTracking_fullSim_fullDigi.pool.root --preExec "RAWtoESD:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doPseudoTracking.set_Value_and_Lock(True);InDetFlags.doNewTracking.set_Value_and_Lock(True);rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(True);" "InDetFlags.doStandardPlots.set_Value_and_Lock(True)"


echo "art-result: $? AOD step"
ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}  
echo  "art-result: $? regression"
rootcomp.py -o comparison -c /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/AOD_newTracking_pseudoTracking_fullSim_fullDigi.pool.root AOD_newTracking_pseudoTracking_fullSim_fullDigi.pool.root
echo  "art-result: $? histcomp"
