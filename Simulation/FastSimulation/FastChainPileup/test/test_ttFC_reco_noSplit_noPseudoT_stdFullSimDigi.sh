#!/usr/bin/env bash

# art-description: Job ttFC_stdFullSim + ttFC_stdFullSimMerge + ttFC_stdFullSimDigi + ttFC_reco_noSplit_noPseudoT_stdFullSimDigi
# art-type: grid
# art-include: master/Athena
# art-output: *.root
# art-output: dcube-truth
# art-output: dcube-id
# art-html: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_reco_noSplit_noPseudoT_stdFullSimDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
dcubeXmlID="${inputXmlDir}/physval-noSplit_noPseudoT_stdFullSimDigi.xml"
dcubeRefID="${inputRefDir}/physval-noSplit_noPseudoT_stdFullSimDigi.root"
dcubeXmlTruth="${inputXmlDir}/dcube_truth.xml"
dcubeRefTruth="${inputRefDir}/truth.root"

rdoFile="RDO.pool.root"
aodFile="AOD_noSplit_noPseudoT_stdFullSimDigi.pool.root"
ntupFile="physval-noSplit_noPseudoT_stdFullSimDigi.root"

rc1=999
rc2=999
rc3=999
rc4=999
Sim_tf.py --conditionsTag 'default:OFLCOND-MC16-SDR-RUN2-09' \
    --physicsList 'FTFP_BERT' \
    --truthStrategy 'MC15aPlus' \
    --simulator 'FullG4' \
    --postInclude 'default:RecJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py' \
    --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
    --DataRunNumber '222525' \
    --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" \
    --outputHITSFile "HITS.pool.root" \
    --maxEvents 50 \
    --imf False
rc=$?
echo "art-result: ${rc} EVNTtoHITS"

if [ ${rc} -eq 0 ]
then
    # Histogram comparison with DCube
    $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube-truth \
    -c ${dcubeXmlTruth} -r ${dcubeRefTruth} truth.root
    echo "art-result: $? dcubeTruth"

    # Merging of hits file
    HITSMerge_tf.py --inputHITSFile='HITS.pool.root' \
        --outputHITS_MRGFile='Merge.pool.root' \
        --maxEvents=50 \
        --skipEvents='0' \
        --geometryVersion 'ATLAS-R2-2015-03-01-00' \
        --conditionsTag 'OFLCOND-MC16-SDR-RUN2-09' \
        --imf False
    echo "art-result: $? HITSMerge"

    #digi
    Digi_tf.py --inputHITSFile 'Merge.pool.root' \
        --outputRDOFile ${rdoFile} \
        --maxEvents '50' \
        --skipEvents '0' \
        --geometryVersion 'ATLAS-R2-2015-03-01-00' \
        --digiSeedOffset1 '123456' \
        --digiSeedOffset2 '2345678' \
        --postInclude 'PyJobTransforms/UseFrontier.py' \
        --AddCaloDigi 'False' \
        --conditionsTag 'OFLCOND-MC16-SDR-RUN2-09' \
        --imf False
    echo "art-result: $? HITStoRDO"
    
    Reco_tf.py --maxEvents '-1' \
        --skipEvents '0' \
        --geometryVersion ATLAS-R2-2015-03-01-00 \
        --conditionsTag OFLCOND-MC16-SDR-RUN2-09 \
        --inputRDOFile ${rdoFile} \
        --outputAODFile ${aodFile} \
        --preExec "all:rec.doTrigger.set_Value_and_Lock(False)" \
        --imf False
    rc1=$?
    if [ ${rc1} -eq 0 ]
    then
         # NTUP prod.
         Reco_tf.py --inputAODFile ${aodFile} --maxEvents '-1' \
                    --outputNTUP_PHYSVALFile ${ntupFile} \
                    --ignoreErrors True \
                    --validationFlags 'doInDet' \
                    --valid 'True'
         rc2=$?

         # Regression test
         ArtPackage=$1
         ArtJobName=$2
         art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
         rc3=$?

         if [ ${rc2} -eq 0 ]
         then
             # Histogram comparison with DCube
             $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
             -p -x dcube-id \
             -c ${dcubeXmlID} -r ${dcubeRefID} ${ntupFile}
             rc4=$?
         fi
    fi
fi
echo "art-result: ${rc1} RDOtoAOD"
echo "art-result: ${rc2} AODtoNTUP"
echo "art-result: ${rc3} regression"
echo "art-result: ${rc4} dcubeID"
