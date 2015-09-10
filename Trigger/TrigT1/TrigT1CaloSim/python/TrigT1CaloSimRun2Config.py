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

        # 25ns: noise cuts for auto-correlation filters
        # MC15-like <mu>=20 w/ pedestal correction enabled
        self.EmThreshElementJEP=[6545, 6708, 6729, 6715, 6823, 6713, 6591, 6197,
                                 6135, 5908, 5857, 5846, 5788, 5206, 4693, 4909, 4848, 5473, 5143, 5149,
                                 5005, 4976, 5001, 4889, 4556, 8678, 8580, 8486, 5464, 3606, 15776,
                                 24490, 23520]
        self.HadThreshElementJEP=[4466, 4403, 4273, 4125, 4051, 3910, 3836,
                                  3696, 4007, 3090, 2883, 2936, 3088, 2707, 2456, 2191, 2827, 3137, 3182,
                                  3075, 2892, 2773, 2719, 2605, 2566, 5491, 5077, 4366, 2225, 4738, 2398,
                                  9324, 4639]

        # CP Thresholds are given for a Digit Scale of 1 GeV/count
        self.EmThreshElementCP=[4000]*33
        self.HadThreshElementCP=[4000]*33


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

        # 50ns: noise cuts for auto-correlation filters
        # MC15-like <mu>=20 w/ pedestal correction enabled
        self.EmThreshElementJEP=[5483, 5651, 5705, 5640, 5785, 5695, 5576, 5215,
                                 5027, 4828, 4878, 4847, 4847, 4454, 3939, 4111, 4120, 4769, 4414, 4485,
                                 4374, 4327, 4391, 4285, 3970, 8213, 8194, 8070, 4869, 3237, 17761,
                                 22763, 23574]
        self.HadThreshElementJEP=[3900, 3848, 3763, 3595, 3560, 3451, 3349,
                                  3165, 3331, 2741, 2624, 2606, 2642, 2462, 2164, 1688, 2314, 2497, 2522,
                                  2488, 2414, 2280, 2205, 1801, 2009, 4779, 4401, 3604, 1716, 4054, 2210,
                                  7687, 3900]

        # CP thresholds are given for a Digit Scale of 1 GeV/count
        self.EmThreshElementCP=[4000]*33
        self.HadThreshElementCP=[4000]*33
