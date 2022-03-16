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
from EventShapeTools.EventDensityConfig import configEventDensityTool, getEventShapeName

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
                                              ChargedPFlowOutputContainer = "GlobalChargedParticleFlowObjects",
                                              NeutralPFlowOutputContainer = "GlobalNeutralParticleFlowObjects"
                                             )

########################################################################

def buildEventShapeAlg(jetOrConstitdef, inputspec, voronoiRf = 0.9, radius = 0.4, suffix = None ):
    """Function producing an EventShapeAlg to calculate
     median energy density for pileup correction"""

    from .JetRecConfig import getPJContName

    
    pjContName = getPJContName(jetOrConstitdef,suffix)
    nameprefix = inputspec or ""

    print("AAAAAA , ",pjContName, inputspec)
    
    rhotool = configEventDensityTool(
        f"EventDensity_{nameprefix}Kt4{pjContName}",
        jetOrConstitdef,
        InputContainer = pjContName, 
        OutputContainer = getEventShapeName(jetOrConstitdef, nameprefix=inputspec, suffix=suffix, radius=radius),
        JetRadius = radius,
        VoronoiRfact = voronoiRf,
        )
    
    eventshapealg = CompFactory.EventDensityAthAlg(
        f"EventDensity_{nameprefix}Kt4{pjContName}Alg",
        EventDensityTool = rhotool )

    return eventshapealg

