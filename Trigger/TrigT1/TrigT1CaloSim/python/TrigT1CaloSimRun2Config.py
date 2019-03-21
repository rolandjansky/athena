# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2TriggerTowerMaker

class Run2TriggerTowerMakerBase (LVL1__Run2TriggerTowerMaker):
    __slots__ = []
    def __init__(self, name):
        super( Run2TriggerTowerMakerBase, self ).__init__( name )
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.digitize.LVL1_on():
            from Digitization.DigitizationFlags import jobproperties
            self.RndmSvc=jobproperties.Digitization.rndmSvc.get_Value()
            self.DigiEngine =  "%s_Digitization"%name
            jobproperties.Digitization.rndmSeedList.addSeed( str(self.DigiEngine), 8631309, 4492432) 
        else:
            rndmSvc = 'AtRanluxGenSvc'
            self.RndmSvc = rndmSvc
            self.DigiEngine =  "%s_Digitization"%name
          
            from AthenaCommon.AppMgr import ServiceMgr
            if not hasattr( ServiceMgr, rndmSvc):
                from AthenaServices.AthenaServicesConf import  AtRanluxGenSvc
                randomSvc = eval(rndmSvc)(rndmSvc)
                ServiceMgr += randomSvc
               
            # init random number seeds
            getattr(ServiceMgr, rndmSvc).Seeds += [ str(self.DigiEngine) + " 8631309 4492432" ]
            getattr(ServiceMgr, rndmSvc).OutputLevel=4 # suppress 1 per event INFO messages.
    
class Run2TriggerTowerMaker(Run2TriggerTowerMakerBase):
    """ Baseline Run2 TriggerTower configuration:
      - with pedestal correction
      - noise cuts
    """
    __slots__ = []
    def __init__(self, name):
        super(Run2TriggerTowerMaker, self).__init__(name)
        self.CellType = 3 # TTL1 input
        self.ZeroSuppress = True

        from SGComps.AddressRemappingSvc import addInputRename
        addInputRename ( 'xAOD::TriggerTowerContainer', 'xAODTriggerTowers_rerun', 'xAODTriggerTowers')
        
        from AthenaCommon import CfgMgr
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "LumiBlockMuTool"):
            ToolSvc += CfgMgr.LumiBlockMuTool("LumiBlockMuTool")

class Run2TriggerTowerMaker25ns(Run2TriggerTowerMaker):
    """ Run2 TriggerTower configuration for 25ns running """
    __slots__ = []
    def __init__(self, name = "Run2TriggerTowerMaker"):
        super(Run2TriggerTowerMaker25ns, self).__init__(name)

class Run2TriggerTowerMaker50ns(Run2TriggerTowerMaker):
    """ Run2 TriggerTower configuration for 50ns running """
    __slots__ = []
    def __init__(self, name = "Run2TriggerTowerMaker"):
        super(Run2TriggerTowerMaker50ns, self).__init__(name)
