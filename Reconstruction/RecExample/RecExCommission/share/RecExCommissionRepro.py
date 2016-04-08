# general configuration for commissioning
from RecExConfig.RecFlags  import rec
from RecExConfig.RecAlgsFlags  import recAlgs
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

#Minimally required flags for commissioning
rec.ScopingLevel.set_Value_and_Lock(2)
include("RecExCommission/MinimalCommissioningSetup.py")

#default

#Commissioning Flags
# include( "RecExCommission/RecExCommissionCommonFlags_jobOptions.py" )
#include( "RecExCommission/RecExCommissionFlags_jobOptions.py" )
#should not be there
#from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags,muidFlags
#muonCombinedRecFlags.doMuGirl = False
#muonCombinedRecFlags.doCaloTrkMuId = True
#muonCombinedRecFlags.doStaco = False
#muonCombinedRecFlags.doMuTag = False
#muonCombinedRecFlags.doAODMuons = True  # switch off AOD making
#muonCombinedRecFlags.doMergeMuons = True # switch off merging for ESD
#muidFlags.SegmentTagger = 'MuTagIMO' # switch off  by ''
#muidFlags.Extrapolated = 'MuidStandalone' # switch off  by ''
#muidFlags.Extrapolated = '' # switch off  by ''
#muidFlags.Combined = 'MuidCombined' # NOW not run # switch off  by ''
#muidFlags.Combined = 'CombinedMuonFit' # switch off  by ''


   
#if read from TAG, cannot write TAG at the same time
#these lines should be in RecExCommon_flags.py
if rec.readTAG():
    rec.doWriteTAGCOM=False

#rec.doTrigger=True
#jet problem right now
#from JetRec.JetRecFlags import jetFlags
#jetFlags.Enabled.set_Value_and_Lock(False)
recAlgs.doObjMissingET.set_Value_and_Lock(False)
recAlgs.doMissingETSig.set_Value_and_Lock(False)


#rec.doJetMissingETTag.set_Value_and_Lock(False)
#from RecExConfig.RecAlgsFlags  import recAlgs
#recAlgs.doMissingET.set_Value_and_Lock(True)
#rec.doEgamma.set_Value_and_Lock(False)
#rec.doTau.set_Value_and_Lock(False)
#recAlgs.doMuonSpShower.set_Value_and_Lock(False)


# For Trigger configuration during the reprocessing 08.
# Will allow to read LVL1 thresholds from COOL instead of from hard-coded XML file.
# For info: Sylvie Brunet, Joerg Stelzer
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.configForStartup= "HLTonline"
tf.configForStartup.lock()


from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022")
#Option: Remove Conditions for HECC Quadrant with broken LV Power supply
#larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("3:238,306,313,325,331,338,344,350,1001:1012,1021,1022")
larCondFlags.OFCShapeFolder.set_Value_and_Lock('5samples3bins17phases')
larCondFlags.SingleVersion.set_Value_and_Lock(False)
larCondFlags.useShape.set_Value_and_Lock(True)


#Switch off EM-Topo cluster corrections for 2nd round of 2008 reprocessing
from CaloClusterCorrection.CaloClusterCorrectionFlags import caloClusterCorrectionFlags
caloClusterCorrectionFlags.CaloTopoEMWhichCorrection.set_Value ("none")
