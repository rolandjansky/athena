# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigT1TGC.TrigT1TGCConf import LVL1TGCTrigger__LVL1TGCTrigger
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags
from IOVDbSvc.CondDB import conddb
from AthenaCommon.AlgSequence import AthSequencer
from MuonCondSvc.MuonCondSvcConf import TGCTriggerDbAlg
from TGCTriggerCondSvc.TGCTriggerCondSvcConf import TGCTriggerCondAlg
from PathResolver import PathResolver

class TrigT1TGCConfig (LVL1TGCTrigger__LVL1TGCTrigger):
    
    def __init__(self,name = "LVL1TGCTrigger" ):
        super(TrigT1TGCConfig ,self).__init__(name)

        self.InputData_perEvent = "TGC_DIGITS" 
        self.MuCTPIInput_TGC    = "L1MuctpiStoreTGC"
        self.MaskFileName12     = "TrigT1TGCMaskedChannel._12.db"
        
job = AlgSequence()
TrigT1TGC = TrigT1TGCConfig()
job += TrigT1TGC

if globalflags.DataSource() == 'geant4':

    conddb.addFolder("TGC_OFL","/TGC/TRIGGER/CW_EIFI",className='CondAttrListCollection')
    conddb.addFolder("TGC_OFL","/TGC/TRIGGER/CW_BW",className='CondAttrListCollection')
    conddb.addFolder("TGC_OFL","/TGC/TRIGGER/CW_TILE",className='CondAttrListCollection')

    # DB files will be put in COOOLDB
    bwCW_Run3_filePath=PathResolver.FindCalibFile("TrigT1TGC_CW/BW/CW_BW_Run3.v01.db")
    conddb.addFolder(bwCW_Run3_filePath,"/TGC/TRIGGER/CW_BW_RUN3 <tag>TgcTriggerCwBwRun3-01</tag>", className='CondAttrListCollection')

    condSequence = AthSequencer("AthCondSeq")
    condSequence += TGCTriggerDbAlg("TGCTriggerDbAlg")
    condSequence += TGCTriggerCondAlg("TGCTriggerCondAlg")
