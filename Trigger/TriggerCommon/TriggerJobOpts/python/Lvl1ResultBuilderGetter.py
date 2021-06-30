# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# -------------------------------------------------------------
# L1 Getter of the result
# -------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.Include import include  # to include old style job options
from AthenaCommon.AppMgr import theApp

from RecExConfig.RecFlags  import rec


from RecExConfig.Configured import Configured
from RecExConfig.ObjKeyStore import objKeyStore

class Lvl1ResultBuilderGetter(Configured):

    def configure(self):

        if jobproperties.Global.InputFormat() == 'bytestream':
            theApp.Dlls += [ "TrigT1Calo" ]
            include("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()


        if rec.doTrigger():
            if (rec.doESD() or rec.doAOD()) and (not(rec.readAOD() or \
                                                         rec.readESD())):
                if jobproperties.Global.InputFormat() == 'bytestream':
                    # Decode ROIB::RoIBResult from ByteStream
                    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamDecodersRecExSetup
                    L1ByteStreamDecodersRecExSetup()
                from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToxAOD
                topSequence += RoIBResultToxAOD()
                pass
            pass

        from TrigEDMConfig.TriggerEDM import getLvl1ESDList
        objKeyStore.addManyTypesStreamESD(getLvl1ESDList())
        from TrigEDMConfig.TriggerEDM import getLvl1AODList
        objKeyStore.addManyTypesStreamAOD(getLvl1AODList())

        return True
