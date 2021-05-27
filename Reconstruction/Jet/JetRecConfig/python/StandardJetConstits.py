
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
 StandardJetConstits: A module defining standard definitions for jet inputs : external container and 
 constituents.
 These can be copied and modified by users who want something a bit   
 different from what is provided.                                     
                                                                      
 Author: TJ Khoo, P-A Delsart                                                      
 * Written for the time being to work in R21, but with R22 in mind    
                                                                      

"""

########################################################################
from .JetDefinition import xAODType,  JetInputConstitSeq, JetInputExternal, JetConstitModifier, JetInputConstit


# Prepare dictionnaries to hold all of our standard definitions.
# They will be filled from the list below
from .Utilities import ldict
stdInputExtDic = ldict()
stdConstitDic = ldict()
stdContitModifDic = ldict()


# This module contains the helper functions needed to instantiate the input container external
# to Jet domain
import JetRecConfig.JetInputConfig as inputcfg
import JetRecTools.JetRecToolsConfig as jrtcfg

def isMC(flags):
    """A simple filter function for  testing if we're running in MC
    returns (bool, str) where the str contains an explanation of why the bool is False.
    (probably worth re-allocating somehere else)"""
    return flags.Input.isMC, "Input file is not MC"



########################################################################
## List of standard input sources for jets.

_stdInputList = [
    # Format is :
    # JetInputExternal( containername , containerType, ...optionnal parameters... )
    #  When defined, algoBuilder is a function returning the actual alg building the input.
    #  it will be called as : algoBuilder(jetdef, spec) where jetdef is the parent JetDefinition 
    
    # *****************************
    JetInputExternal("CaloCalTopoClusters", xAODType.CaloCluster),

    # *****************************
    JetInputExternal("JetETMissParticleFlowObjects", xAODType.ParticleFlow),

    # *****************************
    JetInputExternal("JetSelectedTracks",     xAODType.TrackParticle, algoBuilder = lambda jdef,_ : jrtcfg.getTrackSelAlg(jdef.context ) ),
    JetInputExternal("JetTrackUsedInFitDeco", xAODType.TrackParticle, algoBuilder = inputcfg.buildJetTrackUsedInFitDeco),
    JetInputExternal("JetTrackVtxAssoc",      xAODType.TrackParticle, algoBuilder = inputcfg.buildJetTrackVertexAssoc,
                     prereqs = ["input:JetTrackUsedInFitDeco"] ),

    # *****************************
    JetInputExternal("EventDensity", "EventShape", algoBuilder = inputcfg.buildEventShapeAlg,
                containername = lambda jetdef, specs : (specs or "")+"Kt4"+jetdef.inputdef.label+"EventShape",
                prereqs = lambda jetdef : ["input:"+jetdef.inputdef.name] # this will force the input to be build *before* the EventDensity alg.
    ),
    JetInputExternal("HLT_EventDensity", "EventShape", algoBuilder = inputcfg.buildEventShapeAlg,
                containername = lambda jetdef, specs : (specs or "")+"Kt4"+jetdef.inputdef.label+"EventShape",
                prereqs = lambda jetdef : ["input:"+jetdef.inputdef.name], # this will force the input to be build *before* the EventDensity alg.
                specs = 'HLT_'
    ),

    # *****************************
    JetInputExternal("MuonSegments", "MuonSegment",),


    # *****************************
    # Truth particles from the hard scatter vertex prior to Geant4 simulation.
    # Neutrinos and muons are omitted; all other stable particles are included.
    JetInputExternal("JetInputTruthParticles",  xAODType.TruthParticle,
                algoBuilder = inputcfg.buildJetInputTruth, filterfn=isMC ),

    # Truth particles from the hard scatter vertex prior to Geant4 simulation.
    # Prompt electrons, muons and neutrinos are excluded, all other stable particles
    # are included, in particular leptons and neutrinos from hadron decays.
    JetInputExternal("JetInputTruthParticlesNoWZ",  xAODType.TruthParticle,
                algoBuilder = inputcfg.buildJetInputTruth, filterfn=isMC,specs="NoWZ"),
]


_truthFlavours = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                  "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                  "TausFinal",
                  "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                  "Partons",]
for label in  _truthFlavours:    
    # re-use the main truth input definition : 
    _stdInputList.append( JetInputExternal("TruthLabel"+label, xAODType.TruthParticle,
                                      algoBuilder = inputcfg.buildLabelledTruth,
                                      filterfn=isMC, specs = label ) )



# Fill the stdInputExtDic from the above list 
for ji in _stdInputList:
    ji._locked = True # lock the definitions so we have unmutable references !
    stdInputExtDic[ji.name] = ji




    



## ***************************************
## List of standard constituent sequences
##  This sequences uses the above constit modifiers     
_stdSeqList = [
    # Format is typically :
    # JetInputConstitSeq( name , input_cont_type, list_of_modifiers, inputcontainer, outputcontainer )
    # or
    # JetInputConstit( name, input_cont_type, containername)
    # see JetDefinition.py for details.

    # *****************************
    # Cluster constituents 
    JetInputConstitSeq("EMTopoOrigin", xAODType.CaloCluster, ["EM","Origin"],
                  "CaloCalTopoClusters", "EMOriginTopoClusters", jetinputtype="EMTopo"),
    JetInputConstitSeq("LCTopoOrigin",xAODType.CaloCluster, ["LC","Origin"],
                  "CaloCalTopoClusters", "LCOriginTopoClusters", jetinputtype="LCTopo"),
    JetInputConstitSeq("LCTopoCSSK",  xAODType.CaloCluster, ["LC","Origin","CS","SK"],
                  "CaloCalTopoClusters", "LCOriginTopoCSSK", jetinputtype="LCTopo"),
    


    
    # *****************************
    # EM-scale particle flow objects with charged hadron subtraction
    # For now we don't specify a scale, as only one works well, but
    # this could be incorporated into the naming scheme and config
    JetInputConstitSeq("EMPFlow", xAODType.ParticleFlow,["CorrectPFO", "CHS"] , 'JetETMissParticleFlowObjects', 'CHSParticleFlowObjects'),

    # Particle Flow Objects with Constituent Subtraction + SoftKiller
    JetInputConstitSeq("EMPFlowCSSK", xAODType.ParticleFlow,["CorrectPFO",  "CS","SK", "CHS"] ,
                  'JetETMissParticleFlowObjects', 'CSSKParticleFlowObjects', jetinputtype="EMPFlow"),


    # *****************************
    # Track constituents
    JetInputConstit("Track", xAODType.TrackParticle,'JetSelectedTracks'),
    
    # Track particles from the primary vertex
    JetInputConstitSeq("PV0Track", xAODType.TrackParticle,["PV0"],'JetSelectedTracks', 'PV0JetSelectedTracks',
                  prereqs= ["input:JetTrackUsedInFitDeco","input:JetTrackVtxAssoc"], ),

    # *****************************
    # Muon segments. Only used as ghosts
    JetInputConstit("MuonSegment", "MuonSegment", "MuonSegments" ),

    
    # *****************************
    # Truth particles (see JetInputExternal declarations above for more details)
    JetInputConstit("Truth", xAODType.TruthParticle, "JetInputTruthParticles" ),
    
    JetInputConstit("TruthWZ", xAODType.TruthParticle, "JetInputTruthParticlesNoWZ", jetinputtype="TruthWZ"),
]

for label in  _truthFlavours:    
    _stdSeqList.append( JetInputConstit(label, xAODType.TruthParticle, "TruthLabel"+label ) )

# Fill the stdConstitDic from the above list 
for jc in _stdSeqList:
    jc._locked = True
    stdConstitDic[jc.name] = jc



########################################################################
## List of standard constituent modifiers 

def _getPFOTool(*l):
    """One Property of the CorrectPFO constit modifier is a tool. 
    we use this function as a placeholder, allowing to delay the intantiation of this property tool
    to the time the modifier itself is instantiated.
    """
    from AthenaConfiguration.ComponentFactory import CompFactory
    return CompFactory.getComp("CP::WeightPFOTool")("weightPFO")
    

from JetRecConfig.StandardJetContext import propFromContext

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
                       dict(VertexContainerKey=propFromContext("Vertices"),
                            WeightPFOTool= _getPFOTool ) ), 
              
    JetConstitModifier("CHS",    "ChargedHadronSubtractionTool",
                       # get the track properties from the context with wich jet will be configured with propFromContext
                       # See StandardJetContext.py for the default values.
                       dict(VertexContainerKey=propFromContext("Vertices"),
                            TrackVertexAssociation=propFromContext("TVA"))),
    
    # Pileup suppression
    JetConstitModifier("Vor",    "VoronoiWeightTool", dict(doSpread=False, nSigma=0) ),
    JetConstitModifier("CS",     "ConstituentSubtractorTool", dict(MaxEta=5. ) ),
    JetConstitModifier("SK",     "SoftKillerWeightTool",),
                           
]

# Fill the stdContitModifDic from the above list 
for ji in _stdModList:
    ji._locked = True
    stdContitModifDic[ji.name] = ji
