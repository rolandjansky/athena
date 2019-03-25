#!/usr/bin/env bash
# art-description: test for job configuration ttFC_fastSim_fulldigi (Sim/Digi job) + stdReco_fastSim_fullDigi
# art-type: grid
#
# specify branches of athena that are being targeted:
# art-include: 21.3/Athena

FastChain_tf.py --simulator ATLFASTIIF_PileUp \
    --digiSteeringConf "SplitNoMerge" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root \
    --outputRDOFile RDO_pileup_fastsim_fulldigi.pool.root \
    --maxEvents 100 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True' \
    --preSimInclude FastChainPileup/FastPileup.py \
    --postInclude='PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest_FCpileup.py,DigitizationTests/postInclude.RDO_Plots.py' \
    --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' \
    --DataRunNumber '284500' \
    --postSimExec='genSeq.Pythia8.NCollPerEvent=10;'

echo "art-result: $? RDO step"

Reco_tf.py --inputRDOFile='RDO_pileup_fastsim_fulldigi.pool.root'\
 --outputAODFile=AOD_fastSim_fullDigi.pool.root \
    --autoConfiguration=everything \
    --maxEvents=500 \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.doStandardPlots.set_Value_and_Lock(True)"

echo "art-result: $? ESD step"


#add an additional payload from the job (corollary file).                                                           
# art-output: InDetStandardPlots.root  
#Regression test

ArtPackage=$1
ArtJobName=$2

art.py compare grid --entries 10 --imf=False ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube TEST_ttFC_stdReco_fastSim_fullDigi InDetStandardPlots.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/dcube_configs/config/dcube_indetplots_no_pseudotracks.xml /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/InDetStandardPlots_TEST.root


# art-output: dcube/dcube.xml
# art-output: dcube/dcube.log
# art-output: dcube/dcubelog.xml
# art-output: dcube/dcube.xml.php 
echo "art-result: $? histcompArtPackage

