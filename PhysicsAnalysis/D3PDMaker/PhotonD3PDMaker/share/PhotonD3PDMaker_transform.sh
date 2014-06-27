# $Id $

maxevents=100
# scratch2/mhance/data/mc12_8TeV.160008.PowhegPythia8_AU2CT10_ggH120_gamgam.merge.AOD.e1189_s1469_s1470_r3542_r3549_tid781414_00/AOD.781414._000035.pool.root.1
inputfile="AOD.pool.root"

preExecString="preExec=from@RecExConfig.RecFlags@import@rec"

doZgamma=0
if [[ $doZgamma != 0 ]]; then
    preExecString="${preExecString},,from@PhotonD3PDMaker.PhotonD3PDMakerFlags@import@PhotonD3PDMakerFlags,,PhotonD3PDMakerFlags.AdditionalMETOutputLevel=1,,PhotonD3PDMakerFlags.DumpAllTracks=False,,PhotonD3PDMakerFlags.FilterTracksForMET=False,,PhotonD3PDMakerFlags.FillFullMET=False,,PhotonD3PDMakerFlags.FillR0p6Jets=False"
fi

doFPECheck=0
if [[ $doFPECheck != 0 ]]; then
    preExecString="${preExecString},,rec.doFloatingPointException.set_Value_and_Lock(True)"
fi


# run each test in its own subdirectory
let dir=000
mkdir ${dir}
while [[ $? != 0 ]]; do
    let dir=$(($dir+1))
    mkdir ${dir}
done

pushd $dir

Reco_trf.py \
    inputAODFile=${inputfile} \
    outputNTUP_PHOTONFile=test.NTUP_PHOTON.root \
    autoConfiguration=everything \
    ${preExecString} \
    maxEvents=${maxevents} | tee test.NTUP_PHOTON.log 2>&1

popd

