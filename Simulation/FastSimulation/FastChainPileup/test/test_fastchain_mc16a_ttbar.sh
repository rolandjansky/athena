#!/bin/sh
#
# art-description: Run AFII simulation and full digitization of an MC16a ttbar sample with 2016a geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: master/Athena
# art-output: *.root
# art-output: config.txt
# art-output: dcube-rdo-truth
# art-html: dcube-rdo-truth

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"
inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_fastchain_mc16a_ttbar"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth_ATLFASTII.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"

FastChain_tf.py \
    --simulator ATLFASTII \
    --digiSteeringConf "Split" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1 \
    --outputRDOFile RDO.pool.root \
    --maxEvents 10 \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.experimentalDigi=["NewMerge"]' \
    --preExec 'EVNTtoRDO:ToolSvc.NewMergeMcEventCollTool.OutputLevel=VERBOSE;' \
    --postInclude='PyJobTransforms/UseFrontier.py,DigitizationTests/postInclude.RDO_Plots.py' \
    --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' \
    --DataRunNumber '284500' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --imf False

# Add Reco step?

rc=$?
echo  "art-result: $rc EVNTtoRDO"
rc2=-9999
rc3=-9999

if [ ${rc} -eq 0 ]
then
    # Regression test
    ArtPackage=$1
    ArtJobName=$2
    echo ArtJobName
    echo $ArtJobName
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc2=$?

    # Histogram comparison with DCube
    $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube-rdo-truth \
    -c ${dcubeXmlRDO} -r ${dcubeRefRDO} RDO_truth.root
    rc3=$?

fi
echo  "art-result: ${rc2} regression"
echo  "art-result: ${rc3} dcubeRDO"
