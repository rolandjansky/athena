# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from EventShapeTools.EventShapeToolsConf import EventDensityTool, EventShapeCopier, EventDensityAthAlg
import logging
edLogger = logging.getLogger( "EventDensityConfig" )   

def configEventDensityTool( name, inputlabel, radius, **options ):
    """ options can be used to pass any EventDensityTool properties 
    """
    # Set default and passed properties for the EventDensityTool
    toolProperties = dict(
        JetAlgorithm        = "Kt",
        JetRadius           = radius,
        InputContainer      = "PseudoJet"+inputlabel,
        AbsRapidityMin      = 0.0,
        AbsRapidityMax      = 2.0,
        AreaDefinition      = "Voronoi",
        VoronoiRfact        = 0.9,
        OutputContainer     = "Kt"+str(int(10*radius))+inputlabel + "EventShape",
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
    edLogger.warning("When instantiating %s : call of EventDensityAlg is deprecated", name)
    edLogger.warning("  please use EventDensityAthAlg (from  EventShapeTools.EventShapeToolsConf import EventDensityAthAlg) ")
    alg = EventDensityAthAlg(name,EventDensityTool=EventDensityTool, **args)
    return alg

