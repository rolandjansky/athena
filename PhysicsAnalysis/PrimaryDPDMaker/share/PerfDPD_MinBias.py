##-----------------------------------------------------------------------------
## Name: PrimaryDPD_MinBiasStream.py
##
## Author: Eric Feng (Chicago), Michiru Kaneda (Tokyo)
## Email:  Eric.Feng@cern.ch, Michiru.Kaneda@cern.ch
##
## Based on:  PrimaryDPD_PhotonJetStream.py by Karsten Koeneke (DESY)
##
## Description: This defines the content of the Minimum Bias DPD output stream.
##
## $Id: PerfDPD_MinBias.py,v 1.2 2009-05-11 16:07:15 mkaneda Exp $
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_MinBias.py")


## for messaging
from AthenaCommon.Logging import logging
minbiasDPDStream_msg = logging.getLogger( 'PrimaryDPD_MinBiasStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_MinBiasStream import primMinBiasDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 


##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    minbiasDPDStream_msg.info('Values of all PerfDPD_MinBias flags:')
    print primMinBiasDPD
    pass


     
# Define the some lists
algsToBookkeep = []




# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdMinBiasSequence = CfgMgr.AthSequencer( 'DESD_MBIAS_Sequence',
                                           StopOverride = True )
topSequence += desdMinBiasSequence




##====================================================================
## Setup TrigDecisionTool for the MinBias DPD output stream
##====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")


from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler




##============================================================================
## Define the skimming (event selection) for the DESD_MBIAS output stream
##============================================================================
## SingleTracks

# Make ExtrapolateTrackToCalo tool
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
eopExtrapolator=AtlasExtrapolator("eopExtrapolator")
eopExtrapolator.DoCaloDynamic=False
ToolSvc+=eopExtrapolator

from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
exToCalo = ExtrapolateToCaloTool(name="ExtrapolateToCaloTool", Extrapolator=eopExtrapolator)
ToolSvc+=exToCalo

from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
exToVertex = Reco__TrackToVertex( name = "ExtrapolateToVertexTool", Extrapolator=eopExtrapolator )
ToolSvc += exToVertex

## SingleTracks end


include("TrackIsolationTools/TrackIsolationTool_jobOptions.py")

from PrimaryDPDMaker.PrimaryDPDMakerConf import TrackFilterAlg
desdMinBiasSequence += TrackFilterAlg("MinBiasStream_TrackFilter")
desdMinBiasSequence.MinBiasStream_TrackFilter.cutPtMinTr         = primMinBiasDPD.TrackFilter.TrackPtCut
desdMinBiasSequence.MinBiasStream_TrackFilter.cutSumPtRatio      = primMinBiasDPD.TrackFilter.SumPtRatioCut
desdMinBiasSequence.MinBiasStream_TrackFilter.cutEtaMax          = primMinBiasDPD.TrackFilter.TrackEtaCut
desdMinBiasSequence.MinBiasStream_TrackFilter.cutChi2DOF         = primMinBiasDPD.TrackFilter.Chi2DOFCut
desdMinBiasSequence.MinBiasStream_TrackFilter.cutBLayerHits      = primMinBiasDPD.TrackFilter.NBLayerHitCut
desdMinBiasSequence.MinBiasStream_TrackFilter.cutHoles           = primMinBiasDPD.TrackFilter.NHoleCut
desdMinBiasSequence.MinBiasStream_TrackFilter.cutSiHits          = primMinBiasDPD.TrackFilter.NSiHitCut
desdMinBiasSequence.MinBiasStream_TrackFilter.doNoTrackIsol      = primMinBiasDPD.TrackFilter.TrackIsolCut
desdMinBiasSequence.MinBiasStream_TrackFilter.noTrackIsol        = primMinBiasDPD.TrackFilter.NTrackIsolCut
desdMinBiasSequence.MinBiasStream_TrackFilter.doCaloIsol         = primMinBiasDPD.TrackFilter.CaloIsolCut
desdMinBiasSequence.MinBiasStream_TrackFilter.Enable             = primMinBiasDPD.TrackFilter.Enable
desdMinBiasSequence.MinBiasStream_TrackFilter.TrackIsolationTool = TrackIsolationTool

desdMinBiasSequence += TrackFilterAlg("MinBiasStream_LowPtTrackFilter")
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutPtMinTr         = primMinBiasDPD.LowPtTrackFilter.TrackPtCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutSumPtRatio      = primMinBiasDPD.LowPtTrackFilter.SumPtRatioCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutEtaMax          = primMinBiasDPD.LowPtTrackFilter.TrackEtaCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutChi2DOF         = primMinBiasDPD.LowPtTrackFilter.Chi2DOFCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutBLayerHits      = primMinBiasDPD.LowPtTrackFilter.NBLayerHitCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutHoles           = primMinBiasDPD.LowPtTrackFilter.NHoleCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.cutSiHits          = primMinBiasDPD.LowPtTrackFilter.NSiHitCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.doNoTrackIsol      = primMinBiasDPD.LowPtTrackFilter.TrackIsolCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.noTrackIsol        = primMinBiasDPD.LowPtTrackFilter.NTrackIsolCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.doCaloIsol         = primMinBiasDPD.LowPtTrackFilter.CaloIsolCut
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.Enable             = primMinBiasDPD.LowPtTrackFilter.Enable
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackIsolationTool = TrackIsolationTool

## SingleTracks
desdMinBiasSequence.MinBiasStream_TrackFilter.UseEPFilter            = primMinBiasDPD.TrackFilter.UseEPFilter
desdMinBiasSequence.MinBiasStream_TrackFilter.TrackAtCaloEta         = primMinBiasDPD.TrackFilter.TrackAtCaloEta
desdMinBiasSequence.MinBiasStream_TrackFilter.TrackD0                = primMinBiasDPD.TrackFilter.TrackD0
desdMinBiasSequence.MinBiasStream_TrackFilter.TrackZ0                = primMinBiasDPD.TrackFilter.TrackZ0
desdMinBiasSequence.MinBiasStream_TrackFilter.TrackNumberOfSCTHits   = primMinBiasDPD.TrackFilter.TrackNumberOfSCTHits
desdMinBiasSequence.MinBiasStream_TrackFilter.TrackNumberOfPixelHits = primMinBiasDPD.TrackFilter.TrackNumberOfPixelHits
desdMinBiasSequence.MinBiasStream_TrackFilter.NumberOfPrimaryVertexes = primMinBiasDPD.TrackFilter.NumberOfPrimaryVertexes
desdMinBiasSequence.MinBiasStream_TrackFilter.trackIsoDR             = primMinBiasDPD.TrackFilter.trackIsoDR

desdMinBiasSequence.MinBiasStream_TrackFilter.ExtrapolateToCaloTool = exToCalo
desdMinBiasSequence.MinBiasStream_TrackFilter.ExtrapolateToVertexTool = exToVertex

desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.UseEPFilter            = primMinBiasDPD.LowPtTrackFilter.UseEPFilter
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackAtCaloEta         = primMinBiasDPD.LowPtTrackFilter.TrackAtCaloEta
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackD0                = primMinBiasDPD.LowPtTrackFilter.TrackD0
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackZ0                = primMinBiasDPD.LowPtTrackFilter.TrackZ0
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackNumberOfSCTHits   = primMinBiasDPD.LowPtTrackFilter.TrackNumberOfSCTHits
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackNumberOfPixelHits = primMinBiasDPD.LowPtTrackFilter.TrackNumberOfPixelHits
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.NumberOfPrimaryVertexes = primMinBiasDPD.LowPtTrackFilter.NumberOfPrimaryVertexes
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.trackIsoDR             = primMinBiasDPD.LowPtTrackFilter.trackIsoDR

desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.ExtrapolateToCaloTool = exToCalo
desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.ExtrapolateToVertexTool = exToVertex
## SingleTracks end

if rec.readAOD() :
    from __main__ import ToolSvc
    include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
    # Configure the isolation tool properly for AOD
    from TrackIsolationTools.TrackIsolationToolsConf import TrackIsolationTool
    MinBiasFilter_TrackIsolationTool = TrackIsolationTool('MinBiasFilter_TrackIsolationTool')

    from TrackInCaloTools.TrackInCaloToolsConf import TrackInCaloTools
    MinBiasFilter_TrackInCaloTools = TrackInCaloTools('MinBiasFilter_TrackInCaloTools')
    MinBiasFilter_TrackInCaloTools.CaloCellContainer = "AODCellContainer"        
    ToolSvc += MinBiasFilter_TrackInCaloTools
    MinBiasFilter_TrackIsolationTool.ExtrapolTrackToCaloTool = MinBiasFilter_TrackInCaloTools
    ToolSvc += MinBiasFilter_TrackIsolationTool
        
    desdMinBiasSequence.MinBiasStream_TrackFilter.TrackIsolationTool = MinBiasFilter_TrackIsolationTool
    desdMinBiasSequence.MinBiasStream_LowPtTrackFilter.TrackIsolationTool = MinBiasFilter_TrackIsolationTool
    pass

# Define the prescales
desdMinBiasSequence += PrimaryDPDPrescaler( "MinBiasStream_PrescaledTrackFilter",
                                            AcceptAlgs = ["MinBiasStream_TrackFilter"],
                                            Prescale = primMinBiasDPD.TrackFilter.Prescale )
# Add the per-event bookkeeping of this filer
algsToBookkeep.append("MinBiasStream_PrescaledTrackFilter")

desdMinBiasSequence += PrimaryDPDPrescaler( "MinBiasStream_PrescaledLowPtTrackFilter",
                                            AcceptAlgs = ["MinBiasStream_LowPtTrackFilter"],
                                            Prescale = primMinBiasDPD.LowPtTrackFilter.Prescale )
# Add the per-event bookkeeping of this filer
algsToBookkeep.append("MinBiasStream_PrescaledLowPtTrackFilter")

from PrimaryDPDMaker.DummyFilter import DummyFilter
desdMinBiasSequence += DummyFilter("MinBiasStream_DummyFilter")
desdMinBiasSequence += PrimaryDPDPrescaler( "MinBiasStream_FlatPrescaleFilter",
                                            AcceptAlgs = ["MinBiasStream_DummyFilter"],
                                            Prescale = primMinBiasDPD.SimplePrescaleFilter.Prescale )
# Add the per-event bookkeeping of this filer
algsToBookkeep.append("MinBiasStream_FlatPrescaleFilter")






# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
thinSeqDESD_MBIAS = AthSequencer("DESD_MBIAS_ThinningSequence")
desdMinBiasSequence += thinSeqDESD_MBIAS

# Create the combined decission
thinSeqDESD_MBIAS += LogicalFilterCombiner( "StreamDESD_MBIAS_AcceptEvent",
                                            cmdstring = "MinBiasStream_PrescaledTrackFilter or MinBiasStream_PrescaledLowPtTrackFilter or MinBiasStream_FlatPrescaleFilter" )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_MBIAS_AcceptEvent" )



##====================================================================
## Define the thinning for the Minimum Bias DPD output stream
##====================================================================
if primDPD.ApplyThinning() and primMinBiasDPD.ApplyThinning() and not primDPD.WriteMinBiasStream.isVirtual and not primDPD.isVirtual() :
    from PrimaryDPDMaker.ThinObjects import *
    
    if primMinBiasDPD.ApplyTrackParticleThinning() :
        thinSeqDESD_MBIAS += ThinObjects("ThinTrackParticlesInMinBiasStream")
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.thinCollectionType = 'Rec::TrackParticleContainer'
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.thinCollectionName = 'TrackParticleCandidate'
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.jetCollectionNames = ["AntiKt4TowerJets", "AntiKt4TopoEMJets"]
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.thinSvc = 'ThinningSvc/MinBiasStreamThinning'
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.keepObjectsNearLvl1Rois  = True
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.Lvl1RoiNames             = [ 'EM3', 'EM7' ]
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.keepObjectsNearElectrons = True
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.minEtElectron            = 0.0*Units.GeV
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.maxDeltaRElectronObject  = 0.6
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.keepObjectsNearPhotons   = True
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.minEtPhoton              = 0.0*Units.GeV
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.maxDeltaRPhotonObject    = 0.6
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.keepObjectsNearMuons     = False
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.keepObjectsNearJets      = True
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.minEtJet                 = 0.0*Units.GeV
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.maxDeltaRJetObject       = 0.6
        thinSeqDESD_MBIAS.ThinTrackParticlesInMinBiasStream.keepIsolatedObjects      = False
        pass

    pass  # End: if primDPD.ApplyThinning()



##====================================================================
## Define the Minimum Bias DPD output stream
##====================================================================
streamName = primDPD.WriteMinBiasStream.StreamName
fileName   = buildFileName( primDPD.WriteMinBiasStream )
if primDPD.WriteMinBiasStream.isVirtual or primDPD.isVirtual() :
    MinBiasStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    MinBiasStream = MSMgr.NewPoolStream( streamName, fileName )
    MinBiasStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPD.ApplySkimming() and primMinBiasDPD.ApplySkimming() :
    # Schedule these prescaled algorithms as filters in OR
    MinBiasStream.AcceptAlgs( [ "StreamDESD_MBIAS_AcceptEvent" ] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
#MinBiasStream.Stream.TakeItemsFromInput = True
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)




##====================================================================
## Finalize the thinning of the containers for this stream
##====================================================================
if primDPD.ApplyThinning() and primMinBiasDPD.ApplyThinning() and not primDPD.WriteMinBiasStream.isVirtual and not primDPD.isVirtual() :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    # Thin the TrackParticles
    if primMinBiasDPD.ApplyTrackParticleThinning() :
        augStream=MSMgr.GetStream( primDPD.WriteMinBiasStream.StreamName )
        evtStream=augStream.GetEventStream()
        svcMgr += createThinningSvc( svcName="MinBiasStreamThinning", outStreams=[evtStream] )
        pass
    
    pass  # End: if primDPD.ApplyThinning()
    
