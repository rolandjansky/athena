# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigT1TGC.TrigT1TGCConf import *
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags
from IOVDbSvc.CondDB import conddb
from AthenaCommon.AlgSequence import AthSequencer
from MuonCondSvc.MuonCondSvcConf import TGCTriggerDbAlg

class TrigT1TGCConfig (LVL1TGCTrigger__LVL1TGCTrigger):
    
    def __init__(self,name = "LVL1TGCTrigger" ):
        super(TrigT1TGCConfig ,self).__init__(name)

        self.InputData_perEvent = "TGC_DIGITS" 
        self.ASDOutDataLocation = "ASDOutDataLocation"
        self.MuonTrigConfig     = "/Run/MuonTrigConfig"
        self.MuCTPIInput_TGC    = "L1MuctpiStoreTGC"
        self.MaskFileName       = "TrigT1TGCMaskedChannel.db"
        self.MaskFileName12     = "TrigT1TGCMaskedChannel._12.db"
        
job = AlgSequence()
TrigT1TGC = TrigT1TGCConfig()
job += TrigT1TGC


# Configuration of TGC Coincidence Window (default is setK defined in LVL1TGCTrigger constructor)
from TriggerJobOpts.TriggerFlags import TriggerFlags
triggerMenuSetup = TriggerFlags.triggerMenuSetup()
if triggerMenuSetup.startswith('MC_pp_v2'): # setH (the lowest threshold is L1_MU0) is used for MC_pp_v2 menus. 
    job.LVL1TGCTrigger.VersionCW='setH'
    job.LVL1TGCTrigger.MaskFileName12="TrigT1TGCMaskedChannel._12.MC_pp_v2.db"
    print 'TrigT1TGCConfig.py: TGC Coincidence Window setH will be used for %s menu.' % triggerMenuSetup
elif triggerMenuSetup.startswith('MC_pp_v3'): # setI (MU4' is the lowest, for 2011 autumn pp runs/MC_pp_v3 menus) 
    job.LVL1TGCTrigger.VersionCW='setI'
    print 'TrigT1TGCConfig.py: TGC Coincidence Window setI will be used for %s menu.' % triggerMenuSetup
elif triggerMenuSetup.startswith('MC_HI_v1'): # setJ (for 2011 Heavy Ion runs) is used for MC_HI_v1 menus. 
    job.LVL1TGCTrigger.VersionCW='setJ'
    print 'TrigT1TGCConfig.py: TGC Coincidence Window setJ will be used for %s menu.' % triggerMenuSetup
elif triggerMenuSetup.startswith('MC_HI_v2'): # setL (for 2013 Heavy Ion runs) is used for MC_HI_v2 menus. 
    job.LVL1TGCTrigger.VersionCW='setL'
    print 'TrigT1TGCConfig.py: TGC Coincidence Window setL will be used for %s menu.' % triggerMenuSetup
elif triggerMenuSetup.startswith('Physics_lumi1E34'): # setE (PT=0,6,10,15,20,40) is used for high lumi MC
    job.LVL1TGCTrigger.VersionCW='setE'
    print 'TrigT1TGCConfig.py: TGC Coincidence Window setE will be used for %s menu.' % triggerMenuSetup

if globalflags.DataSource() == 'geant4':

    conddb.addFolder("TGC_OFL","/TGC/TRIGGER/CW_EIFI",className='CondAttrListCollection')
    conddb.addFolder("TGC_OFL","/TGC/TRIGGER/CW_BW",className='CondAttrListCollection')
    conddb.addFolder("TGC_OFL","/TGC/TRIGGER/CW_TILE",className='CondAttrListCollection')

    condSequence = AthSequencer("AthCondSeq")
    condSequence += TGCTriggerDbAlg("TGCTriggerDbAlg")
