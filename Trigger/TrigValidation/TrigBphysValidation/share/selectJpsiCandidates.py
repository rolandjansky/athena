## ------------------------------------------------------------
## selectCandidates.py
## ------------------------------------------------------------
##
## Initialize JpsiFinder tools needed to search for J/psi
##
## Daniel Scheirich (daniel.scheirich@cern.ch)
##
## THIS J.O. FRAGMENT MUST BE INCLUDED INTO RunTriggerJpsiAnalysis.py
## ------------------------------------------------------------

from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiAlg
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool

##-------------------------------------------------------------
## InDetTrackSelectorTool
##-------------------------------------------------------------

## For initial running and testing we want to take all the tracks
## This can be achieved by
##  * setting all kinematic cuts very loose
##  * disabling cuts on hits: useTrackSummaryInfo = False
##  * disabling cuts on shared hits: useSharedHitInfo = False (is False by default)
##  * disabling cuts on track quality: useTrackQualityInfo = False
##  * setting useEtaDepententMinHitTrt = False (not quite sure what this does)

## Tracks for J/psi candidates

## cuts according to Combined Muon Performance group baseline
## see http://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=91416

#InDetTrackSelectorTool_Jpsi = InDet__InDetDetailedTrackSelectorTool(name = "InDetTrackSelectorTool_Jpsi",
                                                                    #pTMin                = 500.0,
                                                                    #IPd0Max              = 20.0,
                                                                    #IPz0Max              = 1000.0,
                                                                    #z0Max                = 1000.0,
                                                                    #sigIPd0Max           = 1.0,
                                                                    #sigIPz0Max           = 5.0,
                                                                    #d0significanceMax    = -1.,
                                                                    #z0significanceMax    = -1.,
                                                                    #etaMax               = 9999.,
                                                                    #useTrackSummaryInfo  = True,
                                                                    #nHitBLayer           = 0,
                                                                    #nHitPix              = 0,
                                                                    #nHitBLayerPlusPix    = 1,
                                                                    #nHitSct              = 2,
                                                                    #nHitSi               = 0,
                                                                    #nHitTrt              = 0,
                                                                    #nHitTrtHighEFractionMax = 0,
                                                                    #useSharedHitInfo     = False,
                                                                    #useTrackQualityInfo  = True,
                                                                    #fitChi2OnNdfMax      = 3.5,
                                                                    #TrtMaxEtaAcceptance  = 1.9,
                                                                    #TrackSummaryTool     = InDetTrackSummaryTool,
                                                                    #Extrapolator         = InDetExtrapolator
                                                                   #)



## relaxed cuts

InDetTrackSelectorTool_Jpsi = InDet__InDetDetailedTrackSelectorTool(name = "InDetTrackSelectorTool_Jpsi",
                                                                    pTMin                     = 100.,
                                                                    pMin                      = 0.,
                                                                    IPd0Max                   = 1000.,
                                                                    IPz0Max                   = 1000.,
                                                                    z0Max                     = 1000.,
                                                                    sigIPd0Max                = 1000.,
                                                                    sigIPz0Max                = 1000.,
                                                                    d0significanceMax         = -1.,
                                                                    z0significanceMax         = -1.,
                                                                    etaMax                    = 1000.,
                                                                    useTrackSummaryInfo       = False,
                                                                    useSharedHitInfo          = False,
                                                                    useTrackQualityInfo       = False,
                                                                    useEtaDepententMinHitTrt  = False,
                                                                    TrackSummaryTool          = InDetTrackSummaryTool,
                                                                    Extrapolator              = InDetExtrapolator
                                                                  )

ToolSvc += InDetTrackSelectorTool_Jpsi
print      InDetTrackSelectorTool_Jpsi

##-------------------------------------------------------------
## Select J/psi
##-------------------------------------------------------------

## JpsiFinder tool

jpsiFinder = Analysis__JpsiFinder(  name                       = "jpsiFinder",
                                    OutputLevel                = INFO,
                                    muAndMu                    = True,
                                    muAndTrack                 = False,
                                    TrackAndTrack              = False,
                                    assumeDiMuons              = True,    # True: uses PDG values
                                    track1Mass                 = 105.66,  # Ignored if above is true
                                    track2Mass                 = 105.66,
                                    thresholdPt                = 0.0,
                                    higherPt                   = 0.0,
                                    invMassUpper               = 3097. + 2000.,
                                    invMassLower               = 3097. - 2000.,
                                    collAngleTheta             = 0.0,
                                    collAnglePhi               = 0.0,
                                    Chi2Cut                    = 1000.,
                                    oppChargesOnly             = True,
                                    sameChargesOnly            = False,
                                    allChargeCombinations      = False,
                                    allMuons                   = True,
                                    combOnly                   = False,
                                    atLeastOneComb             = False,
                                    muonCollectionKey          = "StacoMuonCollection",
                                    TrackParticleCollection    = "TrackParticleCandidate",
                                    V0VertexFitterTool         = TrkV0Fitter,             # V0 vertex fitter
                                    useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
                                    TrkVertexFitterTool        = TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                    #TrkVertexFitterTool        = InDetFullVxFitterTool,   # Full Billoir vertex fitter
                                    #TrkVertexFitterTool        = InDetFastVxFitterTool,   # Fast Billoir vertex fitter
                                    TrackSelectorTool          = InDetTrackSelectorTool_Jpsi,
                                    ConversionFinderHelperTool = InDetConversionHelper,
                                    VertexPointEstimator       = VtxPointEstimator)

ToolSvc += jpsiFinder
print      jpsiFinder

## jpsi calling algorithm
jpsiAlg = JpsiAlg(  name                      = 'jpsiAlg',
                    OutputLevel               = INFO,
                    JpsiCandidatesOutputName  = "JpsiCandidates",
                    JpsiFinderName            = jpsiFinder )

