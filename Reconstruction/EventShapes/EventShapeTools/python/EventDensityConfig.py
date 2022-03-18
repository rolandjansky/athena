# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import logging
edLogger = logging.getLogger( "EventDensityConfig" )   
from AthenaConfiguration.ComponentFactory import CompFactory

def getEventShapeName( defOrLabel, nameprefix="", suffix=None, radius=0.4):
    """ Get the name of the event shape container for a given jet def or jet constit def.

    It's typically like "Kt4EMPFlowEventShape"
    But there can be many variations : 
      - nameprefix can be "HLT_"
      - suffix can be EMPFlowPUSB for pu side-band

    We normalize all variations here.
    """

    from JetRecConfig.JetDefinition import JetDefinition,JetInputConstit

    if isinstance(defOrLabel, JetDefinition):
        label = defOrLabel.inputdef.label
    elif isinstance(defOrLabel, JetInputConstit):
        label = defOrLabel.label
    else:
        label = defOrLabel.replace('_','')

    R = str(int(10*radius))
    suffix =  '_'+suffix  if suffix else ''
    nameprefix = nameprefix or "" # just in case we are passed None
    
    return f"{nameprefix}Kt{R}{label}{suffix}EventShape"


def configEventDensityTool( name, jetOrConstitdef, radius=0.4, **options ):
    """Returns an EventDensityTool configured with input and output names build according to the given jet (or jet constituent) definition.

    jetOrConstitdef can be a JetDefinition, JetInputConstit. The input PseudoJetContainer name and output EventShape name are build 
    using helper functions shared in other parts of the config (jet, trigger, egamma,...)

    It is still possible to override any properties of the tool in the optionnal additionnal arguments.
    """
    from JetRecConfig.JetRecConfig import getPJContName

    # Set default and passed properties for the EventDensityTool
    toolProperties = dict(
        JetAlgorithm        = "Kt",
        JetRadius           = radius,
        InputContainer      = getPJContName( jetOrConstitdef), #"PseudoJet"+inputlabel,
        AbsRapidityMin      = 0.0,
        AbsRapidityMax      = 2.0,
        AreaDefinition      = "Voronoi",
        VoronoiRfact        = 0.9,
        OutputContainer     = getEventShapeName( jetOrConstitdef) , #"Kt"+str(int(10*radius))+inputlabel + "EventShape",
        UseFourMomArea      = True,
    )
    # Override properties with user-supplied options.
    toolProperties.update( options)
    # Build the tool :
    return CompFactory.EventDensityTool(name, **toolProperties)


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
    return CompFactory.EventDensityAlg(input+"EventShapeCopierAlg", EventDensityTool = [ buildTool("Kt4"), buildTool("Kt6") ] )



## EventDensity Alg for Athena

def EventDensityAlg(name, EventDensityTool=None, **args):
    edLogger.warning("When instantiating %s : call of EventDensityAlg is deprecated", name)
    edLogger.warning("  please use EventDensityAthAlg (from  EventShapeTools.EventShapeToolsConf import EventDensityAthAlg) ")
    alg = CompFactory.EventDensityAthAlg(name,EventDensityTool=EventDensityTool, **args)
    return alg

