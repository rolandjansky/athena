# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigSteering.TrigSteeringConf import *
from TrigNavigation.TrigNavigationConfig import *
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

class RandomScaler( HLT__RandomScaler ):
    """Random scaler"""

    def __init__(self, name, seed=1857585203, luxury=None):
        super( RandomScaler, self ).__init__( name )

        self._seed = seed
        self._luxury = luxury
        
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'AtRanluxGenSvc'):
            from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
            svcMgr += AtRanluxGenSvc()

    def config(self, seed, luxury=None):
        """Configure non-default seed and luxury"""

        self._seed = seed
        self._luxury = luxury        
        self._updateStream(seed,luxury)
        
    def _updateStream(self, seed, luxury, oldJobOptName=None):
        """Update stream definitions in RNG service"""

        # Remove previous stream definitions
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr        
        if oldJobOptName:
            svcMgr.AtRanluxGenSvc.Seeds = filter(lambda s:s.startswith(oldJobOptName+' ')==False,
                                                 svcMgr.AtRanluxGenSvc.Seeds)
        
        stream = self.getJobOptName()
        svcMgr.AtRanluxGenSvc.Seeds = filter(lambda s:s.startswith(stream+' ')==False,
                                             svcMgr.AtRanluxGenSvc.Seeds)

        # Add updated definition
        if luxury==None:  # This is also for backwards compatibility with AtRanLuxGenSvc
            svcMgr.AtRanluxGenSvc.Seeds += ['%s %d %d' %
                                            (stream, seed, seed) ]
        else:
            svcMgr.AtRanluxGenSvc.Seeds += ['%s LUXURY %d %d %d' %
                                            (stream, luxury, seed, seed) ]
        
    def setParent( self, parentName ):
        """If parent changed, update the stream configuration"""

        oldJobOptName = self._jobOptName
        super( RandomScaler, self).setParent(parentName)
        
        if self._jobOptName != oldJobOptName:
            self._updateStream(self._seed, self._luxury, oldJobOptName)
        
            
class ScalerSvc( HLT__ScalerSvc ):
    __slots__ = []
    def __init__(self, name="ScalerSvc"):
        super( ScalerSvc, self ).__init__( name )
        from AthenaCommon.AppMgr import ToolSvc
        
        toolName = 'HLT::RandomScaler' # Do not change. ScalerSvc relies on this name.
        ToolSvc += RandomScaler(toolName)
        getattr(ToolSvc,toolName).config(seed=1857585203)
        
    def setDefaults(self, handle):
        pass

class Lvl1FromFile(HLT__Lvl1FromFile):
    __slots__ = []
    def __init__(self, name='Lvl1FromFile'):
        super( Lvl1FromFile, self ).__init__( name )

    def setDefaults(self, handle):
        # these values will override the C++ (i.e. developer) default values, not
        # any python (i.e. user) values
        handle.Lvl1File = 'Lvl1Results.txt'
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = HLTResultAccessTool()


class ResultBuilder (HLT__ResultBuilder):
    __slots__ = []
    def __init__(self, name='ResultBuilder'):
        super( ResultBuilder, self ).__init__( name )

    def setDefaults(self, handle):
        pass


class Lvl2Converter(HLT__Lvl2Converter):
    __slots__ = []
    def __init__(self, name="Lvl2Converter"):
        super(  Lvl2Converter, self ).__init__( name )

    def setDefaults(self, handle):
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = HLTResultAccessTool()

class Lvl2ConverterFromPersistency(HLT__Lvl2ConverterFromPersistency):
    __slots__ = []
    def __init__(self, name="Lvl2ConverterFromPersistency"):
        super(  Lvl2ConverterFromPersistency, self ).__init__( name )

    def setDefaults(self, handle):
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = HLTResultAccessTool()

class Lvl1ConsistencyChecker(Lvl1ConsistencyChecker):
    __slots__ = []
    def __init__(self, name="Lvl1ConsistencyChecker"):
        super( Lvl1ConsistencyChecker, self ).__init__( name )
        self.thresholdsToCheck = ['EM','TAU','MUON','JET','FJET','FJ','JF','JB','JE','TE','XE','XS']

class Lvl1Converter(HLT__Lvl1Converter):
    __slots__ = []
    def __init__(self, name="Lvl1Converter"):
        super(  Lvl1Converter, self ).__init__( name )

    def setDefaults(self, handle):
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = HLTResultAccessTool()
        if not hasattr( handle, 'Lvl1ResultAccessTool' ):
            handle.Lvl1ResultAccessTool = Lvl1ResultAccessTool()
        if not hasattr( handle, 'Lvl1ConsistencyChecker' ):
            handle.Lvl1ConsistencyChecker = Lvl1ConsistencyChecker()

class LoopbackConverterFromPersistency(HLT__LoopbackConverterFromPersistency):
    """ Special converter for rerunning single Trigger level """
    __slots__ = []
    def __init__(self, name="LoopbackConverter"):
        super( LoopbackConverterFromPersistency, self ).__init__( name )

    def setDefaults(self, handle):
        if not hasattr( handle, 'HLTResultAccessTool' ):
            handle.HLTResultAccessTool = HLTResultAccessTool()

class HLTResultAccessTool (HLT__HLTResultAccessTool):
    __slots__ = []
    def __init__(self, name = "HLTResultAccessTool"):
        super( HLTResultAccessTool, self ).__init__( name )

    def setDefaults(self,handle):
        pass

class Lvl1ResultAccessTool ( HLT__Lvl1ResultAccessTool ) :
    __slots__ = []
    def __init__(self, name = "Lvl1ResultAccessTool"):
        super( Lvl1ResultAccessTool, self ).__init__( name )

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

        from AthenaCommon.AppMgr import ServiceMgr

        # set LVL1Config svc if not already done
        # from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
        # self.LVL1ConfigSvc = SetupTrigConfigSvc().GetConfigurable()


    def setDefaults(self,handle):
        pass



class EventInfoAccessTool ( HLT__EventInfoAccessTool ):
    __slots__ = []
    def __init__(self, name = "EventInfoAccessTool"):
        super( EventInfoAccessTool, self ).__init__( name )

    def setDefaults(self,handle):
        pass


class TrigSteer_baseClass( HLT__TrigSteer ):

    def __init__(self, name):
        super(TrigSteer_baseClass, self).__init__(name)

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

        # set TrigConfig svc if not already done
        #from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
        #self.ConfigSvc = SetupTrigConfigSvc().GetConfigurable()

        # ScalerSvc        
        if not hasattr( svcMgr, 'ScalerSvc' ):
            log.debug( "Adding ScalerSvc to ServiceMgr" )
            svcMgr += ScalerSvc('ScalerSvc')

        self.OPIScaler = RandomScaler('OPIScaler')
        
    def pruneSteeringMonTools(self, targets):
        """ Will leave only this tools which are mentioned in the target list """

        targetSet = set(targets)

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( self.name() )

        tools_to_remove = []
        for tool in self.MonTools:
            #print "checking - (configuredTarget = "+str(t)+") against tool: ", tool
            if len(set(tool.target()).intersection(targetSet)) == 0:
                tools_to_remove += [tool]

        for t in tools_to_remove:
            log.info("removed "+t.name()+" because its target is: "+str(t.target()))
            self.MonTools.remove(t)

        tools_to_remove = []
        for tool in self.OPITools:
            # print "checking - (configuredTarget = "+str(t)+") against tool: ", tool
            if len(set(tool.target()).intersection(targetSet)) == 0:
                tools_to_remove += [tool]

        for t in tools_to_remove:
            log.info("removed "+t.name()+" because its target is: "+str(t.target()))
            self.OPITools.remove(t)

        for tool in self.MonTools:
            log.info("will keep MonTool: "+tool.name())

        for tool in self.OPITools:
            log.info("will keep OPITool: "+tool.name())

def getConfigurableByConfigName(alg):
    """ Retrives configurable instance given name of algorithm as given in XML config i.e. class/instancename

    Returns either configurable instance object or None
    """
    from AthenaCommon.Configurable import Configurable
    longname = alg
    #    print Configurable.allConfigurables.keys()
    #    print "longname = ", longname
    if Configurable.allConfigurables.has_key(longname):
        return Configurable.allConfigurables[longname]

    shortname = alg.split('/')[1]
    #    print "shortname = ", shortname
    if Configurable.allConfigurables.has_key(shortname):
        return Configurable.allConfigurables[shortname]

    return None

from TrigTimeMonitor.TrigTimeMonitorConf import TrigTimeHistTool
class SteeringTimingTool(TrigTimeHistTool):
    __slots__ =  [ ]
    def __init__(self,name):
        super(TrigTimeHistTool, self).__init__(name)
        self.ExcludeName='.*sequence.*|.*:(%s)' % SteeringTimingTool_short.hists
        
    def target(self):
        return ['Time', 'Online', 'Validation']

class SteeringTimingTool_short(TrigTimeHistTool):
    __slots__ =  [ ]
    hists = 'MonitoringTime|ResultBuilderTime|LvlConverterTime'
    def __init__(self,name):
        super(TrigTimeHistTool, self).__init__(name)
        self.IncludeName='.*:(%s)' % self.hists
        self.TimerHistLimits = [0,25]
        self.NumberOfHistBins = 25
        
    def target(self):
        return ['Time', 'Online', 'Validation']
    
class TrigSteer_HLT( TrigSteer_baseClass ):
    __slots__ = []
    def __init__(self, name, lvl1File = "", hltFile = ""):
        super(TrigSteer_HLT, self).__init__(name)
        self.HLTLevel = "HLT"

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

        from AthenaCommon.AppMgr import ServiceMgr

        self.ExcutionOrderSrtategy = HLT__DefaultExecutionOrderStrategy()        

        # add all algorithms
        hltalgs = ServiceMgr.HLTConfigSvc.getAlgorithmsRun2()
        log.info( "will add HLT algorithms to: "+name)

        from AthenaCommon.Configurable import Configurable
        for alg in hltalgs:
            instance = getConfigurableByConfigName(alg)
            if instance == None:
                log.warning( "can not add algorithm : "+alg+" because configurable for it does not exist")
            else:
                if instance not in self.getChildren():
                    self += instance
                    log.info( "added algorithm to the configuration: "+alg)
       
        try:
            from TrigSteerMonitor.TrigSteerMonitorConfig import TrigSteerMonitorToolList
            self.MonTools += TrigSteerMonitorToolList
        except Exception as e:
            log.warning("Exception while adding monitoring tools ('%s'). Will continue without them." % e)

        try:
            from TrigCostMonitor.TrigCostMonitorConfig import TrigCostToolsList
            self.OPITools += [ tool for tool in TrigCostToolsList if tool.name().count('HLT') > 0]

        except Exception as e:
            log.info("Exception while adding cost monitoring tools ('%s'). Will continue without them." % e)            

           
        time = SteeringTimingTool("HLTSteeringTime")
        time.NumberOfHistBins = 500
        time.TimerHistLimits = [0,5000]
        time.GroupedTimers={"TrigSteer_HLT_Chains": "TrigSteer_HLT_Chain_.+"}
        self.MonTools += [time, SteeringTimingTool_short("HLTSteeringTime_short")]
       

        # setup exeption svc and prepare it for the RECOVERABLE returned by steering
#         if not hasattr( ServiceMgr, 'ExceptionSvc' ):
#             from GaudiSvc.GaudiSvcConf import ExceptionSvc
#             ex = ExceptionSvc()
#             ServiceMgr += ex
#         ServiceMgr.ExceptionSvc.Catch = "LIST"
#         ServiceMgr.ExceptionSvc.Algorithms += [ self.getName()+"=SUCCESS" ]


    def setDefaults(self, handle):
        # these values will override the C++ (i.e. developer) default values, not
        # any python (i.e. user) values
        #handle.doTiming = False

        # make sure one tool is always selected
        if not hasattr( handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = Lvl1Converter()
        if not hasattr( handle, 'Navigation' ):
            handle.Navigation = HLTNavigationOffline()
        if not hasattr( handle, 'ResultBuilder' ):
            handle.ResultBuilder= ResultBuilder()
        if not hasattr( handle, 'EventInfoAccessTool' ):
            handle.EventInfoAccessTool= EventInfoAccessTool()

            
#specific instances
class ReruningTrigSteer_HLT(TrigSteer_HLT):
    def __init__(self, name, lvl1File, hltFile):
        super(ReruningTrigSteer_HLT, self).__init__( name, lvl1File, hltFile)

    def setDefaults(self, handle):
        if not hasattr(handle, 'LvlConverterTool' ):
            handle.LvlConverterTool = LoopbackConverterFromPersistency()
        if not hasattr(handle, 'doHypoOnly' ):
            handle.doFex = False
            
def pruneSteeringMonTools(steering, targets):
    steering.pruneSteeringMonTools( targets )
