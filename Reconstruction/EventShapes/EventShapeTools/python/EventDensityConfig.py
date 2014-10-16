# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from EventShapeTools.EventShapeToolsConf import EventDensityTool

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
        )
    # Override properties with user-supplied options.
    toolProperties.update( options)
    # Build the tool :
    return EventDensityTool(name, **toolProperties)

## EventDensity Alg for Athena
import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
class EventDensityAlg (PyAthena.Alg):
    'put some documentation here'
    def __init__(self, name='EventDensityAlg', **kw):
        ## init base class
        kw['name'] = name
        super(EventDensityAlg, self).__init__(**kw)

        self.EventDensityTool = kw.get('EventDensityTool', None)
        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.edTool = PyAthena.py_tool(self.EventDensityTool.getFullName(), iface='IEventShapeTool')
        return StatusCode.Success

    def execute(self):
        self.msg.debug('==> executing ...')
        return self.edTool.fillEventShape()


    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success
