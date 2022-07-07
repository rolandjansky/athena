#!/bin/bash
# art-description: Run 4 configuration, ITK only recontruction, 10 GeV Electrons, no pileup
# art-input: mc15_14TeV:mc15_14TeV.900035.PG_singleel_Pt10_etaFlatnp0_43.evgen.EVNT.e8185
# art-input-nfiles: 1
# art-type: grid
# art-include: master/Athena
# art-output: idpvm.root
# art-output: *.xml
# art-output: dcube*

lastref_dir=last_results
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/dcube/config/IDPVMPlots_R22.xml"
ref_21p9=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetPhysValMonitoring/ReferenceHistograms/900035_el10_ITk_21p9_v1.IDPVM.root  # Ref release = 21.9.26

geometry=ATLAS-P2-RUN4-01-00-00

run () {
    name="${1}"
    cmd="${@:2}"
    ############
    echo "Running ${name}..."
    time ${cmd}
    rc=$?
    echo "art-result: $rc ${name}"
    return $rc
}

run "Simulation" \
    Sim_tf.py \
    --CA \
    --conditionsTag 'default:OFLCOND-MC15c-SDR-14-05' \
    --simulator 'FullG4MT' \
    --postInclude 'default:PyJobTransforms.UseFrontier' \
    --preInclude 'EVNTtoHITS:Campaigns.PhaseIISimulation' \
    --geometryVersion "default:${geometry}" \
    --inputEVNTFile ${ArtInFile} \
    --outputHITSFile HITS.root \
    --maxEvents -1 \
    --imf False \
    --detectors ITkStrip ITkPixel Bpipe HGTD

run "Digitization"\
    Digi_tf.py \
    --CA \
    --conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion "default:${geometry}" \
    --inputHITSFile HITS.root \
    --jobNumber 568 \
    --maxEvents -1 \
    --outputRDOFile RDO.root \
    --preInclude 'HITtoRDO:Campaigns.PhaseIINoPileUp' \
    --postInclude 'PyJobTransforms.UseFrontier' \
    --detectors ITkStrip ITkPixel Bpipe HGTD

run "Reconstruction" \
    Reco_tf.py --CA \
    --inputRDOFile RDO.root \
    --outputAODFile AOD.root \
    --steering doRAWtoALL \
    --preInclude InDetConfig.ConfigurationHelpers.OnlyTrackingPreInclude

run "IDPVM" \
    runIDPVM.py \
    --filesInput AOD.root \
    --outputFile idpvm.root \
    --doTightPrimary \
    --doHitLevelPlots

reco_rc=$?
if [ $reco_rc != 0 ]; then
    exit $reco_rc
fi

echo "download latest result..."
art.py download --user=artprod --dst="$lastref_dir" "$ArtPackage" "$ArtJobName"
ls -la "$lastref_dir"

run "dcube-21p9" \
    $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube_21p9 \
    -c ${dcubeXml} \
    -r ${ref_21p9} \
    idpvm.root

run "dcube-last" \
    $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube_last \
    -c ${dcubeXml} \
    -r ${lastref_dir}/idpvm.root \
    idpvm.root
