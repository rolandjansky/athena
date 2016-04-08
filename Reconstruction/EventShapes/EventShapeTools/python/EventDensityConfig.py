# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from EventShapeTools.EventShapeToolsConf import EventDensityTool, EventShapeCopier, EventDensityAthAlg
import logging
edLogger = logging.getLogger( "EventDensityConfig" )   

def configEventDensityTool( name, pjGetter, radius, **options ):
    """ options can be used to pass any EventDensityTool properties 
    """
    # Set default and passed properties for the EventDensityTool
    toolProperties = dict(
        JetAlgorithm        = "Kt",
        JetRadius           = radius,
        JetInput            = pjGetter,
        AbsRapidityMin      = 0.0,
        AbsRapidityMax      = 2.0,
        AreaDefinition      = "Voronoi",
        VoronoiRfact        = 0.9,
        OutputContainer     = "Kt"+str(int(10*radius))+pjGetter.Label + "EventShape",
        UseFourMomArea      = True,
        )
    # Override properties with user-supplied options.
    toolProperties.update( options)
    # Build the tool :
    return EventDensityTool(name, **toolProperties)


def configEventShapeCopierAlg( input ):
    """ Returns an Athena alg copying EventShape objects with old key/names to objects with new key/names
    """
    def buildTool( alg):         
        from AthenaCommon.AppMgr import ToolSvc
        t= EventShapeCopier( input+alg+"EvtShapeCopier",
                                 InputEventShape=input+"EventShape",
                                 OutputEventShape=alg+input+"EventShape",
                                 EventDensityName = "DensityForJetsR" + alg[-1])
        ToolSvc +=t
        return t
    return EventDensityAlg(input+"EventShapeCopierAlg", EventDensityTool = [ buildTool("Kt4"), buildTool("Kt6") ] )



## EventDensity Alg for Athena

def EventDensityAlg(name, EventDensityTool=None, **args):
    edLogger.warning("When instantiating "+name+" : call of EventDensityAlg is deprecated")
    edLogger.warning("  please use EventDensityAthAlg (from  EventShapeTools.EventShapeToolsConf import EventDensityAthAlg) ")
    alg = EventDensityAthAlg(name,EventDensityTool=EventDensityTool, **args)
    return alg

                    
## import AthenaCommon.SystemOfUnits as Units
## import AthenaPython.PyAthena as PyAthena
## from AthenaPython.PyAthena import StatusCode
## class EventDensityAlg (PyAthena.Alg):
##     'put some documentation here'
##     def __init__(self, name='EventDensityAlg', **kw):
##         ## init base class
##         kw['name'] = name
##         super(EventDensityAlg, self).__init__(**kw)

##         self.EventDensityTool = kw.get('EventDensityTool', None)
##         ## properties and data members
##         #self.foo = kw.get('foo', 10) # default value
##         return

##     def initialize(self):
##         self.msg.info('==> initialize...')
##         tools = self.EventDensityTool
##         if not isinstance( tools, list):
##             tools = [tools]

##         self.edTools = [PyAthena.py_tool(t.getFullName(), iface='IEventShapeTool') for t in  tools ]
##         self.msg.info(" using density tools : %s"%( self.edTools, ) )

##         return StatusCode.Success

##     def execute(self):
##         self.msg.debug('==> executing ...')
##         for t in self.edTools:
##             sc = t.fillEventShape()
##             if not sc.isSuccess():
##                 self.msg.error(" Error while computing density with tool %s "%(t.name(),))
##                 return StatusCode.Recoverable
##         return StatusCode.Success


##     def finalize(self):
##         self.msg.info('==> finalize...')
##         return StatusCode.Success
