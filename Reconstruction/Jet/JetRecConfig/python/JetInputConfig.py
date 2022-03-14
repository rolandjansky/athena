# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
# JetInputConfig: A helper module providing function to setup algorithms
# in charge of preparing input sources to jets (ex: EventDensity algo, track
# or truth selection,...)
#
# Author: P-A Delsart                                              #
"""


from AthenaConfiguration.ComponentFactory import CompFactory
from JetRecConfig.JetRecConfig import isAthenaRelease

def _buildJetAlgForInput(suffix, tools ):
    jetalg = CompFactory.JetAlgorithm("jetalg_"+suffix,
                                      Tools = tools,
    )
    return jetalg

def buildJetTrackUsedInFitDeco( parentjetdef, inputspec ):
    from JetRecTools import JetRecToolsConfig
    # Jet track used-in-fit decoration
    return _buildJetAlgForInput("JetUsedInFitDeco",
                                tools = [ JetRecToolsConfig.getTrackUsedInFitTool(parentjetdef.context) ]
    )

    
def buildJetInputTruth(parentjetdef, truthmod):
    truthmod = truthmod or ""
    from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthJetParticles
    return _buildJetAlgForInput("truthpartcopy_"+truthmod,
                                tools = [ getCopyTruthJetParticles(truthmod) ]
    )

def buildLabelledTruth(parentjetdef, truthmod):
    from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthLabelParticles
    tool = getCopyTruthLabelParticles(truthmod)
    return _buildJetAlgForInput("truthlabelcopy_"+truthmod,
                                tools = [ tool ]
    )

def buildPV0TrackSel(parentjetdef, spec):
    from JetRecConfig.StandardJetContext import jetContextDic
    from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
    
    trkOptions = jetContextDic[parentjetdef.context]
    tvaTool = getTTVAToolForReco("trackjetTVAtool", 
                                 returnCompFactory = True,
                                 addDecoAlg = isAthenaRelease(),
                                 WorkingPoint = "Nonprompt_All_MaxWeight",
                                 TrackContName = trkOptions['JetTracksQualityCuts'],
                                 VertexContName = trkOptions['Vertices'],
                                 )
    alg = CompFactory.PV0TrackSelectionAlg("pv0tracksel_trackjet", 
                                           InputTrackContainer = trkOptions['JetTracksQualityCuts'],
                                           VertexContainer = trkOptions['Vertices'],
                                           OutputTrackContainer = "PV0"+trkOptions['JetTracks'],
                                           TVATool = tvaTool,
                                           )
    return alg


def buildPFlowSel(parentjetdef, spec):
    return  CompFactory.JetPFlowSelectionAlg( "pflowselalg",
                                              electronID = "LHMedium",
                                              ChargedPFlowInputContainer  = "JetETMissChargedParticleFlowObjects",
                                              NeutralPFlowInputContainer  = "JetETMissNeutralParticleFlowObjects",
                                              ChargedPFlowOutputContainer = "GlobalPFlowChargedParticleFlowObjects",
                                              NeutralPFlowOutputContainer = "GlobalPFlowNeutralParticleFlowObjects"
                                             )

########################################################################
def getEventShapeName( jetDefOrLabel, inputspec):
    """ Get the name of the event shape container for a given event shape alg """

    from .JetDefinition import JetDefinition
    nameprefix = inputspec or ""
    if isinstance(jetDefOrLabel, JetDefinition):
        label = jetDefOrLabel.inputdef.label
    else:
        label = jetDefOrLabel.replace('_','')
    return nameprefix+"Kt4"+label+"EventShape"


def buildEventShapeAlg(jetOrConstitdef, inputspec, voronoiRf = 0.9, radius = 0.4, suffix = None ):
    """Function producing an EventShapeAlg to calculate
     median energy density for pileup correction"""

    from .JetDefinition import JetInputConstit
    
    if isinstance(jetOrConstitdef, JetInputConstit):
        label = jetOrConstitdef.label + '' if suffix is None else f'_{suffix}'
    else:
        label = jetOrConstitdef.inputdef.label + '' if suffix is None else f'_{suffix}'

    rhokey = getEventShapeName(label, inputspec)
    nameprefix = inputspec or ""
    rhotoolname = "EventDensity_"+nameprefix+"Kt4"+label
    
    rhotool = CompFactory.EventDensityTool(
        rhotoolname,
        InputContainer = "PseudoJet"+label, # same as in PseudoJet algs
        OutputContainer = rhokey,
        JetRadius = radius,
        UseFourMomArea = True,
        VoronoiRfact = voronoiRf,
        JetAlgorithm = "Kt",)
    
    eventshapealg = CompFactory.EventDensityAthAlg(
        "{0}{1}Alg".format(nameprefix,rhotoolname),
        EventDensityTool = rhotool )

    return eventshapealg

