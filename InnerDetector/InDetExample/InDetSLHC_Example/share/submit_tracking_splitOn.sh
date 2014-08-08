#
## Pathena options
## --nFiles=2
##
## With Cluster SPlitting

# echo " Run Reco With Cluster Splitting On"
#

pathena --trf "Reco_trf.py \
inputRDOFile=%IN \
outputESDFile=%OUT.ESD.root \
outputAODFile=%OUT.AOD.pool.root \
outputDESDM_TRACKFile=%OUT.D3PD.root \
topOptions=jobOptions_tracking_SLHC_trf_splitOn.py \
preExec='jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(23.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)' \
geometryVersion=ATLAS-SLHC-01-02-01 \
conditionsTag=OFLCOND-SDR-BS14T-ATLAS-00 \
--omitvalidation=ALL --test maxEvents=10 \
preExec_r2e='InDetFlags.writeRDOs.set_Value_and_Lock(True)' " \
    "--inDS=group.det-slhc.mc11_14TeV.105568.ttbar_Pythia.RDO.s010201.atlas-00.pileup23.v2/" \
    "--outDS=user.tperez.mc11_14TeV.105568.ttbar_Pythia.ESD.s010201.atlas-00.pileup23.v2_SplittingOn_v3/" \
    "--nFilesPerJob=2" \
    "--dbRelease=ddo.000001.Atlas.Ideal.DBRelease.v200001:DBRelease-20.0.1.tar.gz" \
    --individualOutDS


# #
# sleep 1
# echo " Run Reco With Cluster Splitting OFF"
# #
# pathena --trf "Reco_trf.py \
# inputRDOFile=%IN \
# outputESDFile=%OUT.ESD.root \
# outputAODFile=%OUT.AOD.pool.root \
# outputDESDM_TRACKFile=%OUT.D3PD.root \
# topOptions=myJobOptions_tracking_SLHC_trf_splitOff.py \
# preExec='jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(23.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)' \
# geometryVersion=ATLAS-SLHC-01-02-01 \
# conditionsTag=OFLCOND-SDR-BS14T-ATLAS-00 \
# --omitvalidation=ALL --test maxEvents=10 \
# preExec_r2e='InDetFlags.writeRDOs.set_Value_and_Lock(True)' " \
#     "--inDS=group.det-slhc.mc11_14TeV.105568.ttbar_Pythia.RDO.s010201.atlas-00.pileup23.v2/" \
#     "--outDS=user.tperez.mc11_14TeV.105568.ttbar_Pythia.ESD.s010201.atlas-00.pileup23.v2_SplittingOff/" \
#     "--nFilesPerJob=2" \
#     "--dbRelease=ddo.000001.Atlas.Ideal.DBRelease.v200001:DBRelease-20.0.1.tar.gz" \
#     --individualOutDS 

