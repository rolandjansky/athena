# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# -------------------------------------------------------------
# L1 Getter of the result
# -------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.Include import include  # to include old style job options
from AthenaCommon.AppMgr import theApp

from RecExConfig.RecFlags  import rec
from RecExConfig.RecAlgsFlags import recAlgs


from RecExConfig.Configured import Configured
from RecExConfig.ObjKeyStore import objKeyStore

class Lvl1ResultBuilderGetter(Configured):
    #_output = {"CTP_Decision":"CTP_Decision", "LVL1_ROI":"LVL1_ROI"}

    def configure(self):
        #         from AthenaServices.AthenaServicesConf import AthenaOutputStream
        #         from AthenaCommon.AppMgr import ServiceMgr
        #         from AthenaCommon.AlgSequence import AlgSequence
        #         topSequence = AlgSequence()

        #         ### the following was gotten from the LVL1TriggerGetter not sure if needed when not simulation
        #         if not (TriggerFlags.fakeLVL1() or TriggerFlags.doLVL1()):
        #            DetFlags.readRIOPool.LVL1_setOn()

        #         # ***Temp : needed to enable required combination of thresholds
        #         #           mu4,mu6,mu10,mu11,mu20,mu40
        #         # Will be replaced by config. from Lvl1ConfigSvc
        #         if hasattr(ServiceMgr,'RPCcablingSimSvc'):
        #             ServiceMgr.RPCcablingSimSvc.HackFor1031 = True
        if jobproperties.Global.InputFormat() == 'bytestream':
            theApp.Dlls += [ "TrigT1Calo" ]
            include("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
        #         ###

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()


        if recAlgs.doTrigger():
            if (rec.doESD() or rec.doAOD()) and (not(rec.readAOD() or \
                                                         rec.readESD())):
                if jobproperties.Global.InputFormat() == 'bytestream':
                    # Decode ROIB::RoIBResult from ByteStream
                    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamDecodersRecExSetup
                    L1ByteStreamDecodersRecExSetup()
                from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import \
                    RoIBResultToAOD
                topSequence += RoIBResultToAOD("RoIBResultToxAOD")
                pass
            pass

        from TrigEDMConfig.TriggerEDM import getLvl1ESDList
        objKeyStore.addManyTypesStreamESD(getLvl1ESDList())
        from TrigEDMConfig.TriggerEDM import getLvl1AODList
        objKeyStore.addManyTypesStreamAOD(getLvl1AODList())

        return True
