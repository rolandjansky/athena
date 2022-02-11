
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
This module defines the standard JetModifier tools used in jet reco

Definitions are grouped in a dictionary of tool configurations using the helpers defined
in package configs.
This dict maps a modifier alias to the JetModifier config object
that in turn will be responsible for generating a configured tool.

The JetModifier config class is defined in JetDefinition.py 

 Args to the JetModifier constructor are:
   1. Tool Type (ignored if the helper is a custom one)
   2. Tool Name (ignored if the helper is a custom one)
   3. createfn : helper function which build the actual tool. If none, we just instantiate the tool type. 
   4. prereqs : Prerequisites  (default to []). Can also be a function which returns a list
   X. all other keyword arguments are directly interpreted as Property of the tool. 
       for ex, passing 'PtMin=10.' will configure the tool as in 'tool.PtMin = 10'
       we can pass function as the value : 
         'JetContainerName=nameFunc' will configure as in 'tool.JetContainerName=nameFunc(jetdef, modspec)'

        --> should this be by default? prefer to avoid ignored args
"""
from .JetDefinition import JetModifier
from .Utilities import ldict
from AthenaConfiguration.ComponentFactory import CompFactory


stdJetModifiers = ldict()

########################################################################
# Define the simple modifier setups here -- those defined in JetRec.
stdJetModifiers.update( 
    Sort   = JetModifier("JetSorter","jetsort"),
    Filter = JetModifier("JetFilterTool","jetptfilter_{modspec}",
                         # we give a function as PtMin : it will be evaluated when instantiating the tool (modspec will come alias usage like "Filter:10000" --> PtMin=100000) 
                         PtMin = lambda _,modspec: int(modspec) ) #
)

########################################################################
# Below, we populate the stdJetModifiers with modifier definitions for tools
# that are defined in other packages.
# When necessary, the helper functions 'createfn' for tools from package 'PackageName' live in modules called
# PackageName.PackageConfig (modules to be moved)

# Calibration
from JetCalibTools import JetCalibToolsConfig
stdJetModifiers.update(
    Calib = JetModifier("JetCalibrationTool","jetcalib_jetcoll_calibseq",
                        createfn=JetCalibToolsConfig.getJetCalibToolFromString,
                        prereqs=JetCalibToolsConfig.getJetCalibToolPrereqs)
)

# TBD:
# All items below in principle will support decoration mode, rather
# than only non-const modification. Mode of operation should be
# determined by interface called from parent tool/alg.


# Many JetMoment tools need to know the name of the container they operate on.
# We set the function below as the 'JetContainer' property so the config system
# can assign the right name to the c++ tool.
def _jetname(jetdef,modspec):
    return jetdef.fullname()

def isMC(flags):
    """A simple filter function for  testing if we're running in MC
    returns (bool, str) where the str contains an explanation of why the bool is False.
    (probably worth re-allocating somehere else)"""
    return flags.Input.isMC, "Input file is not MC"


# Standard jet moments
from JetMomentTools import JetMomentToolsConfig
stdJetModifiers.update(

    # Easy cases, no special config or prereqs, just default tool config 
    ClusterMoments =  JetModifier("JetClusterMomentsTool", "clsmoms", JetContainer = _jetname),
    ECPSFrac =        JetModifier("JetECPSFractionTool", "ecpsfrac", JetContainer = _jetname),
    Width =           JetModifier("JetWidthTool", "width", JetContainer = _jetname),

    # More complex cases here
    CaloEnergies =    JetModifier("JetCaloEnergies", "jetens", 
                                  prereqs=["mod:EMScaleMom"], JetContainer = _jetname,
                                  ),
    CaloQuality =     JetModifier("JetCaloQualityTool", "caloqual",
                                  TimingCuts = [5,10],
                                  Calculations = ["LArQuality", "N90Constituents", "FracSamplingMax",  "NegativeE", "Timing", "HECQuality", "Centroid", "AverageLArQF", "BchCorrCell"],JetContainer = _jetname),

    ConstitFourMom =  JetModifier("JetConstitFourMomTool", "constitfourmom_basename",
                                  createfn=JetMomentToolsConfig.getConstitFourMomTool,),
    EMScaleMom =      JetModifier("JetEMScaleMomTool", "emscalemom_basename",
                                  createfn=JetMomentToolsConfig.getEMScaleMomTool,
                                  JetContainer = _jetname),

    JVF =             JetModifier("JetVertexFractionTool", "jvf",
                                   createfn=JetMomentToolsConfig.getJVFTool,
                                   prereqs = ["mod:TrackMoments"] ,JetContainer = _jetname),
    JVT =             JetModifier("JetVertexTaggerTool", "jvt",
                                   createfn=JetMomentToolsConfig.getJVTTool,
                                   prereqs = [ "mod:JVF" ],JetContainer = _jetname),
    LArHVCorr =       JetModifier("JetLArHVTool", "larhvcorr",
                                   prereqs = ["mod:EMScaleMom"],JetContainer = _jetname),
    OriginSetPV =     JetModifier("JetOriginCorrectionTool", "origin_setpv",
                                   prereqs = [ "mod:JVF" ],JetContainer = _jetname),
    TrackMoments =    JetModifier("JetTrackMomentsTool", "trkmoms",
                                  createfn=JetMomentToolsConfig.getTrackMomentsTool,
                                  prereqs = [ "input:JetTrackVtxAssoc","ghost:Track" ],JetContainer = _jetname),
    
    TrackSumMoments = JetModifier("JetTrackSumMomentsTool", "trksummoms",
                                  createfn=JetMomentToolsConfig.getTrackSumMomentsTool,
                                  prereqs = [ "input:JetTrackVtxAssoc","ghost:Track" ],JetContainer = _jetname),
    Charge =          JetModifier("JetChargeTool", "jetcharge", 
                                  prereqs = [ "ghost:Track" ]),
)

# Truth labelling moments
from ParticleJetTools import ParticleJetToolsConfig
stdJetModifiers.update(
    # Easy cases, no special config or prereqs, just default tool config
    PartonTruthLabel = JetModifier("Analysis::JetPartonTruthLabel","partontruthlabel",
                                    prereqs=["ghost:Partons"]),

    # More complex cases here
    TruthPartonDR =    JetModifier("Analysis::JetConeLabeling","truthpartondr",
                                   filterfn=isMC,
                                   JetTruthMatchTool = lambda *l : CompFactory.getComp("Analysis::JetQuarkLabel")("jetquarklabel", McEventCollection='TruthEvents'),
                                   ),

                                    
    JetDeltaRLabel =   JetModifier("ParticleJetDeltaRLabelTool","jetdrlabeler_jetptmin",
                                   createfn=ParticleJetToolsConfig.getJetDeltaRLabelTool,
                                   prereqs=["ghost:BHadronsFinal",
                                            "ghost:CHadronsFinal",
                                            "ghost:TausFinal"]
                                   ),


    JetGhostLabel =    JetModifier("ParticleJetGhostLabelTool","jetghostlabeler",
                                   createfn=ParticleJetToolsConfig.getJetGhostLabelTool,
                                   prereqs=["ghost:BHadronsFinal",
                                            "ghost:CHadronsFinal",
                                            "ghost:TausFinal"]
                                   )
)




# Substructure tools 
stdJetModifiers.update( 
    nsubjettiness = JetModifier( "NSubjettinessTool", "nsubjettiness",Alpha = 1.0),
    nsubjettinessR = JetModifier( "NSubjettinessRatiosTool", "nsubjettinessR",),

    
    ktdr       = JetModifier("KtDeltaRTool", "ktdr", JetRadius = 0.4),

    ktsplitter = JetModifier( "KTSplittingScaleTool", "ktsplitter"),
    
    angularity = JetModifier( "AngularityTool", "angularity"),
    
    dipolarity = JetModifier( "DipolarityTool", "dipolarity",SubJetRadius = 0.3),
    
    planarflow = JetModifier( "PlanarFlowTool", "planarflow"),

    ktmassdrop = JetModifier( "KtMassDropTool", "ktmassdrop"),

    ecorr      = JetModifier( "EnergyCorrelatorTool", "ecorr", Beta = 1.0),
    ecorrR     = JetModifier( "EnergyCorrelatorRatiosTool", "ecorrR", ),

    ecorrgeneral = JetModifier( "EnergyCorrelatorGeneralizedTool", "ecorrgeneral"),
    ecorrgeneralratios = JetModifier( "EnergyCorrelatorGeneralizedRatiosTool", "ecorrgeneralratios"),

    comshapes = JetModifier( "CenterOfMassShapesTool","comshapes"),

    pull      = JetModifier("JetPullTool", "pull",  UseEtaInsteadOfY = False, IncludeTensorMoments = True ),

    charge    = JetModifier( "JetChargeTool", "charge", K=1.0),

    qw = JetModifier( "QwTool", "qw"),
    #showerdec = JetModifier( "  ShowerDeconstructionTool"),
)
