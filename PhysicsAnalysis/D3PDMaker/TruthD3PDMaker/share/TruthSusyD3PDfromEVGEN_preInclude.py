# Example of changing D3PD maker flags.
# Redefine SG keys for evgen running
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey = 'GEN_EVENT,GEN_AOD,TruthEvent'
D3PDMakerFlags.DoTrigger  = False
D3PDMakerFlags.TruthWriteBHadrons = True
D3PDMakerFlags.TruthWriteEverything = True

###################################################################
# Configure RecExCommon.
#
# Need to set dummy values for when running on pure EVNT files
# if you are running on normal AODs (full simulation), comment next 4 lines
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SDR-BS7T-04-13')
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-16-00-00")

from RecExConfig.RecFlags import rec
rec.doDPD=True
rec.readAOD.set_Value_and_Lock(True)
rec.AutoConfiguration.set_Value_and_Lock(['ProjectName','BeamType','RealOrSim','DoTruth','InputType'])

# The following flags are mandatory if you build jets
from JetRec.JetRecFlags import jetFlags
jetFlags.noStandardConfig.set_Value_and_Lock(True)
jetFlags.evgenJets.set_Value_and_Lock(True)

rec.DPDMakerScripts.append("TruthD3PDMaker/TruthSusyD3PDfromEVGEN_prodOptions.py")




