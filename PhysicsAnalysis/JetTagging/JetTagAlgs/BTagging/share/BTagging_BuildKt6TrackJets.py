# jobO fragment to build jets from TrackParticles
# seuster@cern.ch

print "Building Jets from tracks. Use it if no other JetCollection is to be tagged. Dedicated calibrations are missing"
from AthenaCommon.AppMgr import ToolSvc
from JetRec.JetGetters import *
from JetRec.JetRecConf import *
from JetRecTools.JetRecToolsConf import *

UseDetailedTrackSelector = True

# The list of tools for the jet algorithm
trackjettools = []

if UseDetailedTrackSelector:
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    AtlasExtrapolator = AtlasExtrapolator()
    ToolSvc += AtlasExtrapolator
    if BTaggingFlags.OutputLevel < 3:
      print AtlasExtrapolator
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    TrackJetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "TrackJetTrackSelectorTool",
                                                                      pTMin = 0.,
                                                                      IPd0Max = 9999.,
                                                                      d0MaxPreselection = 9999.,
                                                                      IPz0Max = 10., #only cut here : require a track not too far from PVX in z
                                                                      sigIPd0Max = 9999.,
                                                                      sigIPz0Max = 9999.,
                                                                      etaMax = 9999.,
                                                                      useTrackSummaryInfo = False,
                                                                      nHitBLayer = 0,
                                                                      nHitPix = 0,
                                                                      nHitSct = 0,
                                                                      nHitSi = 0,
                                                                      nHitTrt = 0,
                                                                      nHitTrtHighE = 0,
                                                                      useSharedHitInfo = False,
                                                                      useTrackQualityInfo = False,
                                                                      fitChi2OnNdfMax = 9999,
                                                                      TrackSummaryTool = None,
                                                                      Extrapolator = AtlasExtrapolator)
    
    ToolSvc += TrackJetTrackSelectorTool
    if BTaggingFlags.OutputLevel < 3:
      print TrackJetTrackSelectorTool
else:
    TrackJetTrackSelectorTool = None

# Initial Track selection
trjet_initialC = JetTrackParticleSelectorTool(
    name                = 'TracksForTrackJetSelectorTool',
    ignoreMultivertices = True, #False : for example, use only tracks used in the PVX... maybe too tight
    TrackSelector       = TrackJetTrackSelectorTool,
    OutputLevel         = BTaggingFlags.OutputLevel
    )
trjet_inC = JetInputTool(
    name               = "JetInputSelector",
    InputCollectionKey = BTaggingFlags.TrackParticleCollectionName,
    InputSelector      = trjet_initialC,
    OutputLevel        = BTaggingFlags.OutputLevel
    )
trackjettools += [trjet_inC]

# Jet Finder
trackjettools += getStandardFinderTools('Kt',0.6)

# Jet final selection
trjet_finalT = JetSignalSelectorTool('JetFinalEtCut')
trjet_finalT.UseTransverseEnergy = True
trjet_finalT.MinimumSignal       = 7*GeV
trackjettools += [trjet_finalT]

# Sort jets by decreasing Et
trjet_sortT = JetSorterTool('JetSorter')
trjet_sortT.SortOrder="ByEtDown"
trackjettools += [trjet_sortT]

## Some trick to have the proper jet kinematic (related to SignalState)
trackjettools += [JetSetCalibStateTool("SetCalibState")]

# prepare the dictionary of jet alg attributes :
my_att_dict = dict(jet_attribute_dict) # copy the default dict (it only contains keys)
my_att_dict['_finderType']   = 'Kt'
my_att_dict['_finderSuff']   = ''
my_att_dict['_finderParams'] = [0.6]
my_att_dict['_inputType']    = 'Track'
my_att_dict['_inputColl']    = [ BTaggingFlags.TrackParticleCollectionName ]

# Step 3 call the builder with the attribute dict and the tool list as parameter.
mygetter3 = make_customJetGetter(my_att_dict, trackjettools)

