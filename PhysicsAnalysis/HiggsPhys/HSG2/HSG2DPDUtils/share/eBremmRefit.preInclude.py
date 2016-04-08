from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


include( "RecExCond/RecExCommon_flags.py" )
DetFlags.ID_setOn()
DetFlags.LAr_setOn()
include ("RecExCond/AllDet_detDescr.py")
include ("InDetRecExample/InDetRec_jobOptions.py")
include( "egammaBremRec/D2PDMakerElectronPostProcessing.py")

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.addStreamAOD("ElectronContainer","GSFElectronAODCollection")
