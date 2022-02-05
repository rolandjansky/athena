rec.doTrigger.set_Value_and_Lock(False)
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doESDReconstruction.set_Value_and_Lock(True)
rec.doHIP.set_Value_and_Lock(True)
import HIJetRec.HIJetRecConfig
HIJetRec.HIJetRecConfig.SetHIPMode()
rec.UserAlgs+=["TileRecAlgs/MBTSTimeDiffEventInfoAlg_jobOptions.py",
               "HIGlobal/HIGlobal_jobOptions.py",
               "HIJetRec/HIJetRec_jobOptions.py"]
