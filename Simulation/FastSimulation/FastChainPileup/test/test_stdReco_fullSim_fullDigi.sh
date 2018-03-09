#!/usr/bin/env bash
#
# art-description: test job ttFC_fullSim_fullDigi
# art-type: grid
FastChain_tf.py --simulator ATLFASTII --digiSteeringConf "SplitNoMerge" --useISF True --randomSeed 123 --enableLooperKiller True --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root --outputRDOFile RDO_pileup_fullsim_fulldigi.pool.root --maxEvents 50 --skipEvents 0 --geometryVersion ATLAS-R2-2015-03-01-00 --conditionsTag OFLCOND-RUN12-SDR-31 --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True' --postInclude='PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py,DigitizationTests/postInclude.RDO_Plots.py' --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' --DataRunNumber '284500'

echo "art-result: $? RDO step"

Reco_tf.py --inputRDOFile=RDO_pileup_fullsim_fulldigi.pool.root --outputAODFile=AOD_fullSim_fullDigi.pool.root --autoConfiguration=everything --maxEvents=500 --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.doStandardPlots.set_Value_and_Lock(True)"

echo "art-result: $? ESD step"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}  
echo  "art-result: $? regression"
#rootcomp.py -o comparison -c /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/ISF_G4_WriteCalHitsTest.truth.root truth.root
echo  "art-result: $? histcomp"
