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
            self.PedEngine = "%s_Pedestal"%name
            self.DigiEngine =  "%s_Digitization"%name
            jobproperties.Digitization.rndmSeedList.addSeed( str(self.PedEngine), 8594832, 5736213 )
            jobproperties.Digitization.rndmSeedList.addSeed( str(self.DigiEngine), 8631309, 4492432) 
        else:
            rndmSvc = 'AtRanluxGenSvc'
            self.RndmSvc = rndmSvc
            self.PedEngine = "%s_Pedestal"%name
            self.DigiEngine =  "%s_Digitization"%name
          
            from AthenaCommon.AppMgr import ServiceMgr
            if not hasattr( ServiceMgr, rndmSvc):
                from AthenaServices.AthenaServicesConf import  AtRanluxGenSvc
                randomSvc = eval(rndmSvc)(rndmSvc)
                ServiceMgr += randomSvc
               
            # init random number seeds
            getattr(ServiceMgr, rndmSvc).Seeds += [ str(self.PedEngine) + " 8594832 5736213" ]
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
        self.PedestalValue = 32
        self.BaselineCorrection = True # pedestal correction
        self.ZeroSuppress = True
        
        from AthenaCommon import CfgMgr
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "LumiBlockMuTool"):
            ToolSvc += CfgMgr.LumiBlockMuTool("LumiBlockMuTool")

class Run2TriggerTowerMaker25ns(Run2TriggerTowerMaker):
    """ Run2 TriggerTower configuration for 25ns running """
    __slots__ = []
    def __init__(self, name = "Run2TriggerTowerMaker"):
        super(Run2TriggerTowerMaker25ns, self).__init__(name)

        # 25ns: EM auto-correlation filters - A.D. 2015/01/21
        self.FilterCoeffsEmElement0=[-2,0,14,3,-3]
        self.FilterCoeffsEmElement1=[-2,0,14,3,-3]
        self.FilterCoeffsEmElement2=[-2,-1,15,3,-3]
        self.FilterCoeffsEmElement3=[-2,-1,15,3,-4]
        self.FilterCoeffsEmElement4=[-2,-1,15,3,-4]
        self.FilterCoeffsEmElement5=[-2,-1,15,3,-4]
        self.FilterCoeffsEmElement6=[-2,-1,15,3,-4]
        self.FilterCoeffsEmElement7=[-2,0,14,3,-3]
        self.FilterCoeffsEmElement8=[-1,0,7,2,-2]
        self.FilterCoeffsEmElement9=[-1,0,7,2,-2]
        self.FilterCoeffsEmElement10=[-2,0,14,3,-3]
        self.FilterCoeffsEmElement11=[-2,0,14,3,-4]
        self.FilterCoeffsEmElement12=[-3,-1,15,3,-4]
        self.FilterCoeffsEmElement13=[-3,-1,15,3,-5]
        self.FilterCoeffsEmElement14=[-1,0,7,2,-2]
        self.FilterCoeffsEmElement15=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement16=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement17=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement18=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement19=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement20=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement21=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement22=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement23=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement24=[-2,-1,8,1,-3]
        self.FilterCoeffsEmElement25=[-1,-5,12,-2,-3]
        self.FilterCoeffsEmElement26=[-1,-5,12,-2,-3]
        self.FilterCoeffsEmElement27=[-1,-5,12,-2,-3]
        self.FilterCoeffsEmElement28=[-2,-3,10,0,-4]
        self.FilterCoeffsEmElement29=[1,-2,8,1,0]   # FCal1-1
        self.FilterCoeffsEmElement30=[3,-6,10,-3,0] # FCal1-2
        self.FilterCoeffsEmElement31=[3,-7,10,-3,0] # FCal1-3
        self.FilterCoeffsEmElement32=[3,-7,10,-3,0] # FCal1-4

        # 25ns: HAD auto-correlation filters - A.D. 2015/01/21
        self.FilterCoeffsHadElement0=[-2,4,10,5,0]
        self.FilterCoeffsHadElement1=[-2,4,10,5,1]
        self.FilterCoeffsHadElement2=[-2,4,10,5,0]
        self.FilterCoeffsHadElement3=[-2,4,10,5,0]
        self.FilterCoeffsHadElement4=[-2,4,10,5,0]
        self.FilterCoeffsHadElement5=[-2,4,10,5,0]
        self.FilterCoeffsHadElement6=[-3,4,10,5,0]
        self.FilterCoeffsHadElement7=[-3,4,10,5,0]
        self.FilterCoeffsHadElement8=[-3,4,10,5,0]
        self.FilterCoeffsHadElement9=[-3,4,10,5,0]
        self.FilterCoeffsHadElement10=[-3,4,10,5,0]
        self.FilterCoeffsHadElement11=[-3,4,10,5,-1]
        self.FilterCoeffsHadElement12=[-2,2,5,2,0]
        self.FilterCoeffsHadElement13=[-2,2,5,2,0]
        self.FilterCoeffsHadElement14=[-2,2,5,2,0]
        self.FilterCoeffsHadElement15=[-7,3,10,5,-1]
        self.FilterCoeffsHadElement16=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement17=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement18=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement19=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement20=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement21=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement22=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement23=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement24=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement25=[-10,4,13,2,-4]
        self.FilterCoeffsHadElement26=[-11,4,14,2,-5]
        self.FilterCoeffsHadElement27=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement28=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement29=[1,-4,8,1,0]    # FCal2-1
        self.FilterCoeffsHadElement30=[-2,-1,13,5,0]  # FCal3-1
        self.FilterCoeffsHadElement31=[3,-10,12,-3,0] # FCal2-2
        self.FilterCoeffsHadElement32=[0,-6,9,1,0]    # FCal3-2

        # 25ns: noise cuts for auto-correlation filters (DC14 - need updating!!!)
        self.EmThreshElement=[6182,6246,6506,6515,6536,6495,6449,5844,5959,5780,5476,5526,5677,5348,4807,5307,5248,5383,5316,5230,5219,5228,5122,4933,4633,9316,9034,8981,6087,4116,11645,28641,26263]
        self.HadThreshElement=[4929,4859,4944,4569,4566,4394,4203,4067,4534,3633,3497,3517,3744,3369,3126,2684,3515,3680,3726,3607,3511,3526,3491,3387,3281,5617,5527,5206,2941,5329,10229,3044,5274]

class Run2TriggerTowerMaker50ns(Run2TriggerTowerMaker):
    """ Run2 TriggerTower configuration for 50ns running """
    __slots__ = []
    def __init__(self, name = "Run2TriggerTowerMaker"):
        super(Run2TriggerTowerMaker50ns, self).__init__(name)

        # 50ns: EM auto-correlation filters - A.D. 2015/01/21
        self.FilterCoeffsEmElement0=[0,2,12,4,0]
        self.FilterCoeffsEmElement1=[0,2,12,4,0]
        self.FilterCoeffsEmElement2=[0,1,6,2,0]
        self.FilterCoeffsEmElement3=[0,1,6,2,0]
        self.FilterCoeffsEmElement4=[0,1,6,2,0]
        self.FilterCoeffsEmElement5=[0,1,6,2,0]
        self.FilterCoeffsEmElement6=[0,1,6,2,0]
        self.FilterCoeffsEmElement7=[0,2,12,4,0]
        self.FilterCoeffsEmElement8=[1,2,12,4,0]
        self.FilterCoeffsEmElement9=[0,2,12,4,0]
        self.FilterCoeffsEmElement10=[0,2,12,4,0]
        self.FilterCoeffsEmElement11=[0,2,12,4,0]
        self.FilterCoeffsEmElement12=[-1,2,13,3,-1]
        self.FilterCoeffsEmElement13=[-1,1,14,3,-2]
        self.FilterCoeffsEmElement14=[0,1,6,2,0]
        self.FilterCoeffsEmElement15=[-2,1,14,3,-2]
        self.FilterCoeffsEmElement16=[-2,1,14,3,-2]
        self.FilterCoeffsEmElement17=[-2,0,15,2,-3]
        self.FilterCoeffsEmElement18=[-2,1,14,2,-3]
        self.FilterCoeffsEmElement19=[-2,0,15,2,-3]
        self.FilterCoeffsEmElement20=[-2,0,15,2,-3]
        self.FilterCoeffsEmElement21=[-3,0,15,2,-3]
        self.FilterCoeffsEmElement22=[-3,0,15,2,-3]
        self.FilterCoeffsEmElement23=[-3,0,15,2,-3]
        self.FilterCoeffsEmElement24=[-3,0,15,2,-4]
        self.FilterCoeffsEmElement25=[0,-1,5,-1,-1]
        self.FilterCoeffsEmElement26=[0,-1,5,-1,-1]
        self.FilterCoeffsEmElement27=[0,-1,5,-1,-1]
        self.FilterCoeffsEmElement28=[-1,0,4,0,-1]
        self.FilterCoeffsEmElement29=[2,-1,15,1,0]   # FCal1-1
        self.FilterCoeffsEmElement30=[2,-4,10,-6,0]  # FCal1-2
        self.FilterCoeffsEmElement31=[1,-4,14,-10,0] # FCal1-3
        self.FilterCoeffsEmElement32=[1,-4,14,-10,0] # FCal1-4

        #50ns: HAD auto-correlation filters - A.D. 2015/01/21
        self.FilterCoeffsHadElement0=[0,4,8,5,2]
        self.FilterCoeffsHadElement1=[0,4,8,5,2]
        self.FilterCoeffsHadElement2=[0,4,8,5,2]
        self.FilterCoeffsHadElement3=[0,2,4,3,1]
        self.FilterCoeffsHadElement4=[0,4,8,5,2]
        self.FilterCoeffsHadElement5=[0,4,8,5,2]
        self.FilterCoeffsHadElement6=[0,4,8,5,2]
        self.FilterCoeffsHadElement7=[0,4,8,5,2]
        self.FilterCoeffsHadElement8=[-1,4,9,5,2]
        self.FilterCoeffsHadElement9=[0,2,4,3,1]
        self.FilterCoeffsHadElement10=[0,2,4,3,1]
        self.FilterCoeffsHadElement11=[0,4,8,5,2]
        self.FilterCoeffsHadElement12=[0,5,9,5,2]
        self.FilterCoeffsHadElement13=[0,4,8,5,2]
        self.FilterCoeffsHadElement14=[0,4,8,5,2]
        self.FilterCoeffsHadElement15=[-1,2,4,3,1]
        self.FilterCoeffsHadElement16=[-2,4,9,5,1]
        self.FilterCoeffsHadElement17=[-3,4,9,5,1]
        self.FilterCoeffsHadElement18=[-3,4,9,5,1]
        self.FilterCoeffsHadElement19=[-3,4,9,5,1]
        self.FilterCoeffsHadElement20=[-3,4,9,5,1]
        self.FilterCoeffsHadElement21=[-3,4,9,5,1]
        self.FilterCoeffsHadElement22=[-3,4,9,5,1]
        self.FilterCoeffsHadElement23=[-3,4,9,5,1]
        self.FilterCoeffsHadElement24=[-3,4,9,5,1]
        self.FilterCoeffsHadElement25=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement26=[-2,1,3,1,-1]
        self.FilterCoeffsHadElement27=[-4,2,6,2,-2]
        self.FilterCoeffsHadElement28=[-1,2,4,3,1]
        self.FilterCoeffsHadElement29=[1,-2,15,2,0]  # FCal2-1
        self.FilterCoeffsHadElement30=[1,2,12,6,0]   # FCal3-1
        self.FilterCoeffsHadElement31=[3,-8,12,-4,0] # FCal2-2
        self.FilterCoeffsHadElement32=[0,-3,8,1,0]   # FCal3-2

        # 50ns: noise cuts for auto-correlation filters (DC14 - need updating!!!)
        self.EmThreshElement=[6182,6246,6506,6515,6536,6495,6449,5844,5959,5780,5476,5526,5677,5348,4807,5307,5248,5383,5316,5230,5219,5228,5122,4933,4633,9316,9034,8981,6087,4116,11645,28641,26263]
        self.HadThreshElement=[4929,4859,4944,4569,4566,4394,4203,4067,4534,3633,3497,3517,3744,3369,3126,2684,3515,3680,3726,3607,3511,3526,3491,3387,3281,5617,5527,5206,2941,5329,10229,3044,5274]
