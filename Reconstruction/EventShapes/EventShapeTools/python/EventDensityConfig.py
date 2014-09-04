# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from EventShapeTools.EventShapeToolsConf import EventDensityTool

def configEventDensityTool( name, pjGetter, radius, **options ):
    """ option can be used to pass other EventDensityTool property
    such as EtaRange or UseAbsoluteEta """
    from JetRec.FastJetInterfaceConfig import getFastJetInterfaceConfig

    # Prepare default properties of the EventDensityTool

    defaultProperties = dict(
        PseudoJetGetter     = pjGetter,
        EventShapeName      = pjGetter.Label+"EventShape",
        EtaRange            = [-2,2],
        UseAbsoluteEta      = True,
        UseAreaFourMomentum = True,
        VariableName = "DensityForJetsR"+str(int(10*radius) )
        )

    # special treatment for the FastJetTool
    if "FastJetTool" not in options:
        fjTool = getFastJetInterfaceConfig("EventDensityArea"+str(int(radius*10)),
                                           Algorithm = "Kt",
                                           JetAreaMethod = "VoronoiArea",
                                           CalculateJetArea = True,
                                           VoronoiEffectiveRfact = 0.9,
                                           Radius = radius )
        defaultProperties["FastJetTool"] = fjTool

    # override by user-given options if any :
    defaultProperties.update( options)
    # Build the tool :
    return EventDensityTool( name, 
                             **defaultProperties)




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

    # class EventDensityAlg
