include("RecExCommission/MinimalCommissioningSetup.py")


## tempory fix to get TCT to run whil bug 51007 is being fixed
#DQMonFlags.doMuonTrackMon.set_Value_and_Lock(False)
#DQMonFlags.doMuonSegmentMon.set_Value_and_Lock(False)
#DQMonFlags.doMuonAlignMon.set_Value_and_Lock(False)
#DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)

#Temprary workaround for bug 51068
#DQMonFlags.doTauMon.set_Value_and_Lock(False) 

# needed to get TRT track extension working
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers.set_Value(True)

# setup trigger reading from COOL (obo Clemencia)
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.configForStartup="HLTonline"
tf.configForStartup.lock()

from RecExConfig.RecFlags  import rec
rec.ScopingLevel.set_Value_and_Lock(2)

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.SingleVersion=False
larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022")
larCondFlags.OFCShapeFolder.set_Value_and_Lock('5samples3bins17phases') 
