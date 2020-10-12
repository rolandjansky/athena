# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
 StandardJetConstits: A module defining standard definitions for jet  constituents.
 These can be copied and modified by users who want something a bit   
 different from what is provided.                                     
                                                                      
 Author: TJ Khoo, P-A Delsart                                                      
 * Written for the time being to work in R21, but with R22 in mind    
                                                                      

"""

########################################################################
from .JetDefinition import xAODType,  JetConstitSeq, JetInputDef, JetConstitModifier, JetConstitSource


# Prepare dictionnaries to hold all of our standard definitions.
# They will be filled from the list below
from .Utilities import ldict
jetconstitdic = ldict()
jetconstitmoddic = ldict()
jetinputdefdic = ldict()


# This module contains the helper functions needed to instantiate the input container external
# to Jet domain
import JetRecConfig.JetInputConfig as inputcfg

def _isMCTrue(flags):
    """A simple filter function for  testing if we're running in MC
    (probably worth re-allocating somehere else)"""
    return flags.Input.isMC, "Not MC input"


########################################################################
## List of standard input sources for jets.

_stdInputList = [
    # Format is :
    # JetInputDef( containername , containerType, ...optionnal parameters... )

    # *****************************
    JetInputDef("CaloCalTopoClusters", xAODType.CaloCluster),

    # *****************************
    JetInputDef("JetETMissParticleFlowObjects", xAODType.ParticleFlow),

    # *****************************
    JetInputDef("JetSelectedTracks",  xAODType.TrackParticle, algoBuilder = inputcfg.buildJetSelectedTracks),
    JetInputDef("JetTrackVtxAssoc",   xAODType.TrackParticle, algoBuilder = inputcfg.buildJetTrackVertexAssoc),

    # *****************************
    JetInputDef("EventDensity", "EventShape", algoBuilder = inputcfg.buildEventShapeAlg,
                prereqs = lambda jetdef : ["input:"+jetdef.inputdef.name] # this will force the input to be build *before* the EventDensity alg.
    ),
    JetInputDef("HLT_EventDensity", "EventShape", algoBuilder = inputcfg.buildEventShapeAlg,
                prereqs = lambda jetdef : ["input:"+jetdef.inputdef.name], # this will force the input to be build *before* the EventDensity alg.
                specs = 'HLT_'
    ),

    # *****************************
    JetInputDef("MuonSegments", "MuonSegment",),


    # *****************************
    # Truth particles from the hard scatter vertex prior to Geant4 simulation.
    # Neutrinos and muons are omitted; all other stable particles are included.
    JetInputDef("JetInputTruthParticles",  xAODType.TruthParticle,
                algoBuilder = inputcfg.buildJetInputTruth, filterfn=_isMCTrue ),

    # Truth particles from the hard scatter vertex prior to Geant4 simulation.
    # Prompt electrons, muons and neutrinos are excluded, all other stable particles
    # are included, in particular leptons and neutrinos from hadron decays.
    JetInputDef("JetInputTruthParticlesNoWZ",  xAODType.TruthParticle,
                algoBuilder = inputcfg.buildJetInputTruth, filterfn=_isMCTrue,specs="NoWZ"),
]


_truthFlavours = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
               "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
               "TausFinal",
               "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
               "Partons",]
for label in  _truthFlavours:    
    # re-use the main truth input definition : 
    _stdInputList.append( JetInputDef("TruthLabel"+label, xAODType.TruthParticle,
                                      algoBuilder = inputcfg.buildLabelledTruth,
                                      filterfn=_isMCTrue, specs = label ) )



# Fill the jetinputdefdic from the above list 
for ji in _stdInputList:
    ji._locked = True # lock the definitions so we have unmutable references !
    jetinputdefdic[ji.name] = ji





########################################################################
## List of standard constituent modifiers 

def _getPFOTool(constitSeq):
    """One Property of the CorrectPFO constit modifier is a tool. 
    we use this function as a placeholder, allowing to delay the intantiation of this property tool
    to the time the modifier itself is instantiated.
    """
    from AthenaConfiguration.ComponentFactory import CompFactory
    return CompFactory.getComp("CP::WeightPFOTool")("weightPFO")
    

vtxKey = "PrimaryVertices"
tvaKey = "JetTrackVtxAssoc"
_stdModList = [
    # Format is :
    # JetConstitModifier( name , toolType, dictionnary_of_tool_properties )
    # (see JetDefinition.py for more details)
    
    JetConstitModifier("Origin", "CaloClusterConstituentsOrigin", ),
    JetConstitModifier("EM",     "ClusterAtEMScaleTool", ),
    JetConstitModifier("LC",     "", ),
    # Particle flow
    JetConstitModifier("CorrectPFO", "CorrectPFOTool",
                       dict(VertexContainerKey=vtxKey,
                            WeightPFOTool= _getPFOTool ) ), 
              
    JetConstitModifier("CHS",    "ChargedHadronSubtractionTool",
                       dict(VertexContainerKey=vtxKey, TrackVertexAssociation=tvaKey) ),
    
    # Pileup suppression
    JetConstitModifier("Vor",    "VoronoiWeightTool", dict(doSpread=False, nSigma=0) ),
    JetConstitModifier("CS",     "ConstituentSubtractorTool", dict(MaxEta=5. ) ),
    JetConstitModifier("SK",     "SoftKillerWeightTool",),
                           
]

# Fill the jetconstitmoddic from the above list 
for ji in _stdModList:
    ji._locked = True
    jetconstitmoddic[ji.name] = ji
    

    



## ***************************************
## List of standard constituent sequences
##  This sequences uses the above constit modifiers     
_stdSeqList = [
    # Format is :
    # JetConstitSeq( name , input_cont_type, list_of_modifiers, inputcontainer, outputcontainer )
    # or
    # JetConstitSource( name, input_cont_type, containername)
    # see JetDefinition.py for details.

    # *****************************
    # Cluster constituents 
    JetConstitSeq("EMTopoOrigin", xAODType.CaloCluster, ["EM","Origin"], "CaloCalTopoClusters", "EMOriginTopoClusters"),
    JetConstitSeq("LCTopoOrigin",xAODType.CaloCluster, ["LC","Origin"],"CaloCalTopoClusters", "LCOriginTopoClusters"),
    JetConstitSeq("LCTopoCSSK",  xAODType.CaloCluster, ["LC","Origin","CS","SK"],"CaloCalTopoClusters", "LCOriginTopoCSSK"),
    


    
    # *****************************
    # EM-scale particle flow objects with charged hadron subtraction
    # For now we don't specify a scale, as only one works well, but
    # this could be incorporated into the naming scheme and config
    JetConstitSeq("EMPFlow", xAODType.ParticleFlow,["CorrectPFO", "CHS"] , 'JetETMissParticleFlowObjects', 'CHSParticleFlowObjects'),

    # Particle Flow Objects with Constituent Subtraction + SoftKiller
    JetConstitSeq("EMPFlowCSSK", xAODType.ParticleFlow,["CorrectPFO",  "CS","SK", "CHS"] , 'JetETMissParticleFlowObjects', 'CSSKParticleFlowObjects'),


    # *****************************
    # Track constituents
    JetConstitSource("Track", xAODType.TrackParticle,'JetSelectedTracks'),
    
    # Track particles from the primary vertex
    JetConstitSeq("PV0Track", xAODType.TrackParticle,["PV0"],'JetSelectedTracks', 'PV0JetSelectedTracks',
                prereqs= ["input:JetTrackVtxAssoc"], ),

    # *****************************
    # Muon segments. Only used as ghosts
    JetConstitSource("MuonSegment", "MuonSegment", "MuonSegments" ),

    
    # *****************************
    # Truth particles (see JetInputDef declarations above for more details)
    JetConstitSource("Truth", xAODType.TruthParticle, "JetInputTruthParticles" ),
    
    JetConstitSource("TruthWZ", xAODType.TruthParticle, "JetInputTruthParticlesNoWZ" ),
]

for label in  _truthFlavours:    
    _stdSeqList.append( JetConstitSource(label, xAODType.TruthParticle, "TruthLabel"+label ) )

# Fill the jetconstitdic from the above list 
for jc in _stdSeqList:
    jc._locked = True
    jetconstitdic[jc.name] = jc
