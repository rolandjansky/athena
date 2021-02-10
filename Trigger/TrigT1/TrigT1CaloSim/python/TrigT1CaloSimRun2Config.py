# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2TriggerTowerMaker

class Run2TriggerTowerMakerBase (LVL1__Run2TriggerTowerMaker):
    __slots__ = []
    def __init__(self, name):
        super( Run2TriggerTowerMakerBase, self ).__init__( name )
        from AthenaCommon.DetFlags import DetFlags
        from AthenaCommon.Constants import WARNING
        if DetFlags.digitize.LVL1_on():
            from Digitization.DigitizationFlags import jobproperties
            self.RndmSvc=jobproperties.Digitization.rndmSvc.get_Value()
            self.DigiEngine =  "%s_Digitization"%name
            jobproperties.Digitization.rndmSeedList.addSeed( str(self.DigiEngine), 8631309, 4492432) 
        else:
            self.RndmSvc = 'AtRanluxGenSvc'
            self.DigiEngine = "%s_Digitization" % name
          
            from AthenaCommon.AppMgr import ServiceMgr
            if not hasattr( ServiceMgr, 'AtRanluxGenSvc'):
                from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
                ServiceMgr += AtRanluxGenSvc()
               
            # init random number seeds
            ServiceMgr.AtRanluxGenSvc.Seeds += [ str(self.DigiEngine) + " 8631309 4492432" ]
            ServiceMgr.AtRanluxGenSvc.OutputLevel = WARNING # suppress 1 per event INFO messages.
    
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

def Run2TriggerTowerMakerCfg(flags, name):
    '''
    Basic setup of tower maker cfg for new JO 
    WARNING: need to add dependencies on digi flags (as above) that are missing as of now
    '''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc = ComponentAccumulator()
    # TODO this is only needed when re-running 
    #    from SGComps.AddressRemappingConfig import InputRenameCfg
    #    acc.merge(InputRenameCfg('xAOD::TriggerTowerContainer', 'xAODTriggerTowers_rerun', 'xAODTriggerTowers'))

    alg = CompFactory.LVL1.Run2TriggerTowerMaker(name,
                                                 RndmSvc = 'AtRanluxGenSvc',
                                                 DigiEngine = "{}_Digitization".format(name),
                                                 # TODO make these settings flags dependent
                                                 CellType = 3,
                                                 inputTTLocation = 'unused',
                                                 TriggerTowerLocationRerun = 'also_unused',
                                                 ZeroSuppress = True, 
                                                 #ExtraInputs = ['LArTTL1Container#LArTTL1EM', 'LArTTL1Container#LArTTL1HAD', 'TileTTL1Container#TileTTL1Cnt']
                                                 )
    acc.addPublicTool(CompFactory.LumiBlockMuTool('LumiBlockMuTool'))
    from RngComps.RandomServices import Ranlux64
    acc.merge(Ranlux64( str(alg.DigiEngine) + ' 8631309 4492432') )
    acc.addEventAlgo(alg)


    return acc