# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigSteering.TrigSteeringConf import *
from TrigSteering.TrigSteeringConfig import TrigSteer_baseClass
from TrigNavigation.TrigNavigationConfig import TestingHLTNavigationOffline
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigSteering.TrigSteeringConfig import getConfigurableByConfigName

class TestingLvl1FromFile(HLT__Lvl1FromFile):
    __slots__ = []
    def __init__(self, name='Lvl1FromFile'):
        super( TestingLvl1FromFile, self ).__init__( name )

    def setDefaults(self, handle):
        # these values will override the C++ (i.e. developer) default values, not
        # any python (i.e. user) values
        handle.Lvl1File = 'Lvl1Results.txt'
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE

        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = TestingHLTResultAccessTool()


class TestingResultBuilder (HLT__ResultBuilder):
    __slots__ = []
    def __init__(self, name='ResultBuilder'):
        super( TestingResultBuilder, self ).__init__( name )

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE


class TestingLvl2Converter(HLT__Lvl2Converter):
    __slots__ = []
    def __init__(self, name="Lvl2Converter"):
        super(  TestingLvl2Converter, self ).__init__( name )

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = TestingHLTResultAccessTool()


class TestingLvl2ConverterFromPersistency(HLT__Lvl2ConverterFromPersistency):
    __slots__ = []
    def __init__(self, name="Lvl2ConverterFromPeristency"):
        super(  TestingLvl2Converter, self ).__init__( name )

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = TestingHLTResultAccessTool()


class TestingLoopbackConverterFromPersistency(HLT__LoopbackConverterFromPersistency):
    __slots__ = []
    def __init__(self, name="LoopbackConverter"):
        super( TestingLoopbackConverterFromPersistency, self ).__init__( name )

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = TestingHLTResultAccessTool()


class TestingLvl2ConverterFromPersistency(HLT__Lvl2ConverterFromPersistency):
    __slots__ = []
    def __init__(self, name="Lvl2Converter"):
        super( TestingLvl2ConverterFromPersistency, self ).__init__( name )

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = TestingHLTResultAccessTool()





class TestingLvl1Converter(HLT__Lvl1Converter):
    __slots__ = []
    def __init__(self, name="Lvl1Converter"):
        super(  TestingLvl1Converter, self ).__init__( name )

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = TestingHLTResultAccessTool()


class TestingHLTResultAccessTool (HLT__HLTResultAccessTool):
    __slots__ = []
    def __init__(self, name = "HLTResultAccessTool"):
        super( TestingHLTResultAccessTool, self ).__init__( name )

    def setDefaults(self,handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE


class TestingLvl1ResultAccessTool ( HLT__Lvl1ResultAccessTool ) :
    __slots__ = []
    def __init__(self, name = "Lvl1ResultAccessTool"):
        super( TestingLvl1ResultAccessTool, self ).__init__( name )

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

        # set LVL1Config svc if not already done
        # from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
        # self.LVL1ConfigSvc = SetupTrigConfigSvc().GetConfigurable()

    def setDefaults(self,handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE


#class SteeringTiming(TrigTimeHistToolConfig):
#    def __init__ (self, name="SteeringTiming"):



class TestingTrigSteer( TrigSteer_baseClass ):
    __slots__ = []
    def __init__(self, name):
        """name must be *_<level>"""
        super(TestingTrigSteer, self).__init__(name)

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )
       
        self.HLTLevel = name[name.rfind('_')+1:]
        if not self.HLTLevel in ['L2', 'EF', 'HLT']:
            raise RuntimeError("Wrong steering level %s, name of steering instance must end in _L2, _EF, or _HLT" % self.HLTLevel)

        # get handle to HLTConfigSvc:
        from AthenaCommon.AppMgr import ServiceMgr
        from AthenaCommon.Constants import VERBOSE,DEBUG
        ServiceMgr.ScalerSvc.OutputLevel = VERBOSE

        # merging the menus in case of level=HLT
        ServiceMgr.HLTConfigSvc.doMergedHLT = ( self.HLTLevel == 'HLT' )


        # add all PESA algorithms (of the correct lvl) to this instance of TrigSteer :
        l2algs, efalgs = ServiceMgr.HLTConfigSvc.getAlgorithmsByLevel()
        log.info( "will add %s algorithms to: %s" % (self.HLTLevel,name) )

        algs = {"L2" : l2algs, "EF" : efalgs, "HLT" : l2algs+efalgs }

        for alg in algs[ self.HLTLevel ]:
            instance = getConfigurableByConfigName(alg)
            if instance == None:
                log.warning( "can not add algorithm : "+alg+" because configurable for it does not exist")
            else:
                if instance not in self.getChildren():
                    self += instance
                    log.info( "added algorithm to the configuration: "+alg)

        # adding the steering monitoring tools
        try:
            from TrigSteerMonitor.TrigSteerMonitorLegacyConfig import TrigSteerMonitorToolList
            self.MonTools += TrigSteerMonitorToolList
        except:
            log.warning("Attempt to add monitoring tools failed, will continue without them")

        # adding the cost tools
        try:
            from TrigCostMonitor.TrigCostMonitorConfig import TrigCostToolsList
            self.OPITools.extend(TrigCostToolsList)
        except:
            log.info("Attempt to add trigger cost monitoring tools failed, will continue without them")

        time = TrigTimeHistToolConfig("%sSteeringTime" % self.HLTLevel )
        self.MonTools += [ time ]

        self.OutputLevel=DEBUG
        self.doOperationalInfo=DEBUG
        self.ExecutionOrderStrategy.OutputLevel=VERBOSE
        self.Navigation.OutputLevel=DEBUG



    def setDefaults(self, handle):
        # these values will override the C++ (i.e. developer) default values, not
        # any python (i.e. user) values
        handle.doTiming = True
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        # make sure one tool is always selected
        if not hasattr( handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = TestingLvl1FromFile()
        if not hasattr( handle, 'Navigation' ):
            handle.Navigation = TestingHLTNavigationOffline()
        if not hasattr( handle, 'ResultBuilder' ):
            handle.ResultBuilder= TestingResultBuilder()


class TestingTrigSteer_L2( TrigSteer_baseClass ):
    __slots__ = []
    def __init__(self, name):
        super(TestingTrigSteer_L2, self).__init__(name)

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )
        self.HLTLevel = "L2"

        # get handle to HLTConfigSvc:
        from AthenaCommon.AppMgr import ServiceMgr
        
        from AthenaCommon.Constants import VERBOSE
        ServiceMgr.ScalerSvc.OutputLevel = VERBOSE

        ServiceMgr.HLTConfigSvc.doMergedHLT = False

        # add all PESA algorithms (of the correct lvl) to this instance of TrigSteer :
        l2algs, efalgs = ServiceMgr.HLTConfigSvc.getAlgorithmsByLevel()
        log.info( "will add L2 algorithms to: "+name)

        for alg in l2algs:
            instance = getConfigurableByConfigName(alg)
            if instance == None:
                log.warning( "can not add algorithm : "+alg+" because configurable for it does not exist")
            else:
                if instance not in self.getChildren():
                    self += instance
                    log.info( "added algorithm to the configuration: "+alg)
        try:
            from TrigSteerMonitor.TrigSteerMonitorLegacyConfig import TrigSteerMonitorToolList
            self.MonTools += TrigSteerMonitorToolList
        except:
            log.warning("Attempt to add monitoring tools failed, will continue without them")

        try:
            from TrigCostMonitor.TrigCostMonitorConfig import TrigCostToolsList
            self.OPITools.extend(TrigCostToolsList)
        except:
            log.info("Attempt to add trigger cost monitoring tools failed, will continue without them")

        time = TrigTimeHistToolConfig("L2SteeringTime")
        self.MonTools += [ time ]

    def setDefaults(self, handle):
        # these values will override the C++ (i.e. developer) default values, not
        # any python (i.e. user) values
        handle.doTiming = True
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        # make sure one tool is always selected
        if not hasattr( handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = TestingLvl1FromFile()
        if not hasattr( handle, 'Navigation' ):
            handle.Navigation = TestingHLTNavigationOffline()
        if not hasattr( handle, 'ResultBuilder' ):
            handle.ResultBuilder= TestingResultBuilder()

class TestingTrigSteer_EF( TrigSteer_baseClass ) :
    __slots__ = []
    def __init__(self, name):
        super(TestingTrigSteer_EF, self).__init__(name)

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )
        self.HLTLevel = "EF"

        # get handle to HLTConfigSvc:
        from AthenaCommon.AppMgr import ServiceMgr

        from AthenaCommon.Constants import VERBOSE
        ServiceMgr.ScalerSvc.OutputLevel = VERBOSE

        ServiceMgr.HLTConfigSvc.doMergedHLT = False

        # log.info(ServiceMgr)

        # add all PESA algorithms (of the correct lvl) to this instance of TrigSteer :
        l2algs, efalgs = ServiceMgr.HLTConfigSvc.getAlgorithmsByLevel()
        log.info( "will add EF algorithms to: "+name)

        for alg in efalgs:
            instance = getConfigurableByConfigName(alg)
            if instance == None:
                log.warning( "can not add algorithm : "+alg+" because configurable for it does not exist")
            else:
                if instance not in self.getChildren():
                    self += instance
                    log.info( "added algorithm to the configuration: "+alg)


        try:
            from TrigSteerMonitor.TrigSteerMonitorLegacyConfig import TrigSteerMonitorToolList
            self.MonTools += TrigSteerMonitorToolList
        except:
            log.warning("Attempt to add monitoring tools failed, will continue without them")

        try:
            from TrigCostMonitor.TrigCostMonitorConfig import TrigCostToolsList
            self.OPITools.extend(TrigCostToolsList)
        except:
            log.info("Attempt to add trigger cost monitoring tools failed, will continue without them")
            
        time = TrigTimeHistToolConfig("EFSteeringTime")
        self.MonTools += [ time ]

        # setup exeption svc and prepare it for the RECOVERABLE returned by steering
#         if not hasattr( ServiceMgr, 'ExceptionSvc' ):
#             from GaudiSvc.GaudiSvcConf import ExceptionSvc
#             ex = ExceptionSvc()
#             ServiceMgr += ex
#         ServiceMgr.ExceptionSvc.Catch = "LIST"
#         ServiceMgr.ExceptionSvc.Algorithms += [ self.getName()+"=SUCCESS" ]

    def setDefaults(self, handle):
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
        handle.doTiming = True
        if not hasattr( handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = TestingLvl2Converter()
        if not hasattr( handle, 'Navigation' ):
            handle.Navigation = TestingHLTNavigationOffline()
        if not hasattr( handle, 'ResultBuilder' ):
            handle.ResultBuilder= TestingResultBuilder()

#############################################################
#specific instcenes of the configuration
#############################################################

class TestingReruningTrigSteer_L2(TestingTrigSteer_L2):
    def __init__(self, name, lvl1File=""):
        super(TestingReruningTrigSteer_L2, self).__init__( name, lvl1File)

    def setDefaults(self, handle):
        if not hasattr(handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = TestingLoopbackConverterFromPersistency()
        if not hasattr(handle, 'doHypoOnly' ):
            handle.doFex = False


class TestingReruningTrigSteer_EF(TestingTrigSteer_EF):
    def __init__(self, name):
        super(TestingReruningTrigSteer_EF, self).__init__( name)

    def setDefaults(self, handle):
        if not hasattr(handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = TestingLoopbackConverterFromPersistency()
        if not hasattr(handle, 'doHypoOnly' ):
            handle.doFex = false


class TestingReruningAfterL2TrigSteer_EF(TestingTrigSteer_EF):
    def __init__(self, name):
        super(TestingReruningAfterL2TrigSteer_EF, self).__init__( name)

    def setDefaults(self, handle):
        if not hasattr(handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = TestingLvl2ConverterFromPersistency()
        if not hasattr(handle, 'doHypoOnly' ):
            handle.doFex = false
