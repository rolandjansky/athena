# Example of changing D3PD maker flags.
# Redefine SG keys for evgen running

###################################################################
# Configure RecExCommon.
#
# Need to set dummy values for when running on pure EVNT files
# if you are running on normal AODs (full simulation), comment next 4 lines
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-MC12b-SIM-00')
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-20-00-01")

## Turn off geometry construction...
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.all_setOff()

from RecExConfig.RecFlags import rec
rec.doDPD=True
rec.doTruth=True
rec.readAOD.set_Value_and_Lock(True)
rec.AutoConfiguration.set_Value_and_Lock(['ProjectName','BeamType','RealOrSim','DoTruth','InputType'])
rec.doApplyAODFix.set_Value_and_Lock(False)
rec.doTrigger=False
#rec.LoadGeometry.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)

# The following flags are mandatory if you build jets
from JetRec.JetRecFlags import jetFlags
jetFlags.noStandardConfig.set_Value_and_Lock(True)
jetFlags.evgenJets.set_Value_and_Lock(True)

#--------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------
# Set up the D3PD flags the way we want them
# These can be overriden via a set_and_lock_value()
from AthenaCommon.SystemOfUnits import GeV
from D3PDMakerConfig.D3PDMakerFlags import jobproperties
jobproperties.D3PDMakerFlags.DoTrigger  = False
jobproperties.D3PDMakerFlags.TruthSGKey = 'GEN_EVENT,GEN_AOD,TruthEvent'
jobproperties.D3PDMakerFlags.TruthWriteTauHad        = True
jobproperties.D3PDMakerFlags.TruthWriteBSM           = True
jobproperties.D3PDMakerFlags.TruthWriteBHadrons      = False
jobproperties.D3PDMakerFlags.TruthWriteBosons        = True
jobproperties.D3PDMakerFlags.TruthWriteHadrons       = False
jobproperties.D3PDMakerFlags.TruthWritePartons       = False
jobproperties.D3PDMakerFlags.TruthPartonPtThresh     = 5.*GeV
jobproperties.D3PDMakerFlags.TruthWriteBosonProducts = True
jobproperties.D3PDMakerFlags.TruthWriteBSMProducts   = True
jobproperties.D3PDMakerFlags.TruthWriteTopAndDecays  = True
#jobproperties.D3PDMakerFlags.TruthWriteFirstN        = 10

from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.JetTrackAssoc=False

rec.DPDMakerScripts.append("TruthD3PDMaker/TruthD3PDfromEVGEN_prodOptions.py")

