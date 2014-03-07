# Reco_trf tweaks (use as preInclude in Reco_trf)

## Default changes (done for f274 tag)
# InDetFlags.useBeamConstraint.set_Value_and_Lock(True);
from CaloRec.CaloCellFlags import jobproperties;
jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False); 

# Special for LArCafJobs: remove unneccessary stuff
from RecExConfig.RecFlags import rec
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
#rec.doTrigger.set_Value_and_Lock(False)
rec.doZdc.set_Value_and_Lock(False)
rec.doLucid.set_Value_and_Lock(False)

# Set data source
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')

# Make sure all the shapes are loaded
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArCoolChannelSelection.set_Value_and_Lock('')
