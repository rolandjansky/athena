include("RecExCommission/MinimalCommissioningSetup.py")

# not needed anymore (RS, Dec. 6th 2010)
# needed to get TRT track extension working
# from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
# TrkDetFlags.TRT_BuildStrawLayers.set_Value(True)

# setup trigger reading from COOL (obo Clemencia)
if rec.doTrigger():
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.configForStartup="HLTonline"
    tf.configForStartup.lock()

from RecExConfig.RecFlags  import rec
rec.ScopingLevel.set_Value_and_Lock(1)
# done in the trf
# rec.Production=True

#from LArConditionsCommon.LArCondFlags import larCondFlags
#larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022")
#larCondFlags.OFCShapeFolder.set_Value_and_Lock('5samples3bins17phases') 
