# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# -------------------------------------------------------------
# L1 Getter of the result
# -------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.Enums import Format

from RecExConfig.RecFlags import rec

from RecExConfig.Configured import Configured
from RecExConfig.ObjKeyStore import objKeyStore

class Lvl1ResultBuilderGetter(Configured):

    def configure(self):

        if ConfigFlags.Input.Format is Format.BS:
            from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
            CAtoGlobalWrapper(LVL1CaloRun2ReadBSCfg, ConfigFlags)

        if rec.doTrigger():
            if (rec.doESD() or rec.doAOD()) and (not(rec.readAOD() or \
                                                         rec.readESD())):
                if jobproperties.Global.InputFormat() == 'bytestream':
                    # Decode ROIB::RoIBResult from ByteStream
                    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamDecodersRecExSetup
                    L1ByteStreamDecodersRecExSetup()
                from AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig import RoIBResultToxAODCfg
                CAtoGlobalWrapper(RoIBResultToxAODCfg, ConfigFlags)

        from TrigEDMConfig.TriggerEDM import getLvl1ESDList
        objKeyStore.addManyTypesStreamESD(getLvl1ESDList())
        from TrigEDMConfig.TriggerEDM import getLvl1AODList
        objKeyStore.addManyTypesStreamAOD(getLvl1AODList())

        return True
