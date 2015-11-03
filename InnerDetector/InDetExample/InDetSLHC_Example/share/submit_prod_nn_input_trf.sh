#
# Tiago Perez, 2012/09/12
# Descr: Submit the job transform to produce NN input to the grid.
# Note that to run locally with ATLAS-SLHC-01-02-01 you'll need to link locally the db.
#
pathena --trf "Reco_trf.py inputRDOFile=%IN outputNTUP_TRKVALIDFile=%OUT.TRK_NTUP.root topOptions=jobOptions_prod_nn_input_trf_ROT_SLHC.py geometryVersion=ATLAS-SLHC-01-02-01 conditionsTag=OFLCOND-SDR-BS14T-ATLAS-00 --omitvalidation=ALL --test maxEvents=-1 preExec_r2e='InDetFlags.writeRDOs.set_Value_and_Lock(True)' " \
    "--inDS=group.det-slhc.mc11_14TeV.105015.J6_pythia_jetjet.RDO.ATLAS_SLHC_01_02_01.v2/" \
    "--outDS=user.tperez.mc11_14TeV.105015.J6_pythia_jetjet.RDO.ATLAS_SLHC_01_02_01.v2_ROT/" \
    "--nFilesPerJob=2" \
    "--dbRelease=ddo.000001.Atlas.Ideal.DBRelease.v200001:DBRelease-20.0.1.tar.gz" \
    --individualOutDS 

pathena --trf "Reco_trf.py inputRDOFile=%IN outputNTUP_TRKVALIDFile=%OUT.TRK_NTUP.root topOptions=jobOptions_prod_nn_input_trf_RIO_SLHC.py geometryVersion=ATLAS-SLHC-01-02-01 conditionsTag=OFLCOND-SDR-BS14T-ATLAS-00 --omitvalidation=ALL --test maxEvents=-1 preExec_r2e='InDetFlags.writeRDOs.set_Value_and_Lock(True)' " \
    "--inDS=group.det-slhc.mc11_14TeV.105015.J6_pythia_jetjet.RDO.ATLAS_SLHC_01_02_01.v2/" \
    "--outDS=user.tperez.mc11_14TeV.105015.J6_pythia_jetjet.RDO.ATLAS_SLHC_01_02_01.v2_RIO/" \
    "--nFilesPerJob=2" \
    "--dbRelease=ddo.000001.Atlas.Ideal.DBRelease.v200001:DBRelease-20.0.1.tar.gz" \
    --individualOutDS 

