#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
doWriteRDOTrigger = False
doWriteBS = False
doEmptyMenu = True
include("TriggerJobOpts/runHLT_standalone.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
#viewSeq = AthSequencer("AthViewSeq", Sequential=True, ModeOR=False, StopOverride=False)
#topSequence += viewSeq

#from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
#roiCollectionName =  mapThresholdToL1RoICollection("EM")  
# View maker alg
#from AthenaCommon import CfgMgr

signatureName = 'Cosmic'

#TODO switch once done
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection, mapThresholdToL1DecisionCollection

inputMakerAlg = EventViewCreatorAlgorithm("IM_%s"%signatureName)
inputMakerAlg.ViewFallThrough = True
inputMakerAlg.RoIsLink = "initialRoI" # -||-
inputMakerAlg.InViewRoIs = "%sInputRoIs"%signatureName # contract with the consumer
inputMakerAlg.Views = "%sViewRoIs"%signatureName
inputMakerAlg.RoITool = ViewCreatorInitialROITool()
inputMakerAlg.InputMakerInputDecisions = [  mapThresholdToL1DecisionCollection("FSNOSEED") ] #After L1Dec there is a filter producing decision, this maps the relevant RoI to that decision 
#inputMakerAlg.InputMakerOutputDecisions = [ 'OUTDEC' ]
inputMakerAlg.InputMakerOutputDecisions =  'DUMMYOUTDEC' 


print(inputMakerAlg)

VDV = None
from TrigInDetConfig.InDetSetup import makeInDetAlgs
viewAlgs, VDV = makeInDetAlgs(whichSignature=signatureName, rois=inputMakerAlg.InViewRoIs, doFTF= False )

#TODO add additional EFID tracking 
from AthenaCommon.CFElements import seqAND
#cosmicSequence = seqAND("%sSequence"%signatureName,viewAlgs)
#inputMakerAlg.ViewNodeName = cosmicSequence.name()

from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
trackingCosmicCuts  = ConfiguredNewTrackingCuts("Cosmics")

from TrigInDetConfig.EFIDTracking import makeInDetPatternRecognition
EFIDalgs = makeInDetPatternRecognition( signatureName,  NewTrackingCuts = trackingCosmicCuts )
viewAlgs.extend( EFIDalgs )
#EFIDseq = seqAND("%sEFIDSequence"%signatureName, EFIDalgs  )
#print len(EFIDalgs)

cosmicSequence = seqAND("%sSequence"%signatureName,viewAlgs)
inputMakerAlg.ViewNodeName = cosmicSequence.name()

#Cosmic seq in views
viewSequence = seqAND("%sViewSequence"%signatureName,  [ inputMakerAlg, cosmicSequence ] )

topSequence += viewSequence


  #Adding vertexing
  #from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
  ##TODO need to change the name of the output vertex collection to something recordable
  #vtxAlgs = makeVertices( "egamma", "HLT_IDTrack_FS_FTF", "HLT_xPrimVx"  )
  #allViewAlgorithms += vtxAlgs


  #from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
  ##Adding precision tracking
  #PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "egamma", inputFTFtracks="TrigFastTrackFinder_Tracks_FS" )
  #allViewAlgorithms += PTAlgs

#
#from TrigInDetConfig.InDetSetup import makeInDetAlgs
#
##hypo
#beamspotHypoAlg = TrigStreamerHypoAlgMT("BeamspotHypoAlg")
#beamspotHypoAlg.RuntimeValidation = False #Needed to avoid the ERROR ! Decision has no 'feature' ElementLink
#beamspotHypoToolGen= StreamerHypoToolMTgenerator
#beamspotViewsSequence = seqAND("beamspotViewsSequence", [ inputMakerAlg, beamspotSequence ])
#
#return  MenuSequence( Sequence    = beamspotViewsSequence,
#                          Maker       = inputMakerAlg,
#                          Hypo        = beamspotHypoAlg,
#                          HypoToolGen = beamspotHypoToolGen )
#
#
