##-----------------------------------------------------------
## Name: DAOD_RPVLL.py
##
## Author: Jordi Duarte-Campderros (Tel Aviv University)
## Email: jorge.duarte.campderros@cern.ch
## 
## Description: This defines and creates the final Derived 
## Physics Data (DPD) in xAOD format, to be used for several
## analysis within the SUSY-RPVLL and Exotics UEH groups. 
## This script is intended to be used after the aplication
## of the DESDM_RPVLL filter (see the PhysDRAW_RPVLL.py file)
##
## Called by PrimaryDPDFlags.WriteDAOD_RPVLLStream 

#if this file is already included, don't include it again
include.block("LongLivedParticleDPDMaker/DAOD_RPVLL.py")

## for messaging
from AthenaCommon.Logging import logging
DAOD_RPVLLStream_msg = logging.getLogger( 'DAOD_RPVLLStream' )

    
## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
streamName = primDPD.WriteDAOD_RPVLLStream.StreamName 
fileName   = buildFileName( primDPD.WriteDAOD_RPVLLStream )

# activating the relevant collection
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.Electron = True
AODFlags.FastSimulation = False
AODFlags.FastTrackParticle = True
AODFlags.JetTag = False
AODFlags.McEventKeyStr = 'TruthEvent'
AODFlags.MissingEtTruth = False
AODFlags.Muon = True
AODFlags.MuonTrackSlimmer = False
AODFlags.ParticleJet = True
AODFlags.Photon = True
AODFlags.SpclMC = True
AODFlags.Streaming = False
AODFlags.Tau = True
AODFlags.Tau1p3p = True
AODFlags.TauRec = True
AODFlags.TauTrackSlimmer = False
AODFlags.TrackParticleLastHitAndPerigeeSlimmer = False
AODFlags.TrackParticleSlimmer = True
AODFlags.Trigger = True
AODFlags.TruthParticleJet = True
AODFlags.egammaTrackSlimmer = True

# Need to be activated because of the use of the 
# "RecExPers/RecoOutputAODList_jobOptions.py" file
rec.doWriteAOD = True 

## Mimicking RecExample/RecExCommon/share/RecExCommon_topOptions.py
## See from line 1305
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.Configurables import ThinningSvc
if not hasattr(svcMgr, 'ThinningSvc'):
    svcMgr += ThinningSvc(OutputLevel=INFO)
    svcMgr.ThinningSvc.Streams += [streamName]
    
# cannot redo the slimming if readAOD and writeAOD
if not rec.readAOD():
    if rec.doEgamma() and (AODFlags.Photon or AODFlags.Electron):
        doEgammaPhoton = AODFlags.Photon
        doEgammaElectron= AODFlags.Electron
        from egammaRec.egammaAODGetter import egammaAODGetter
        egammaAODGetter()
        if AODFlags.egammaTrackSlimmer:
            from egammaRec.egammaTrackSlimmer import egammaTrackSlimmer
            egammaTrackSlimmer()
        if AODFlags.TauTrackSlimmer:
            protectedInclude("tauRec/tauMerged_trackslim_jobOptions.py")

pdr.flag_domain('output')

##====================================================================
## Define the DAOD RPVLL output stream
##====================================================================
if primDPD.isVirtual() == False:
    DAOD_RPVLLStream_Augmented=MSMgr.NewPoolStream( streamName, fileName,asAlg = True )
if primDPD.isVirtual() == True:
    DAOD_RPVLLStream_Augmented=MSMgr.NewVirtualStream( streamName, fileName )

if rec.doFileMetaData():
    # Trigger tool
    if not hasattr(ToolSvc,"xAODMaker__TriggerMenuMetaDataTool/TriggerMenuMetaDataTool"):
        ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool")
    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]
    # EventFormat tool
    if not hasattr(ToolSvc,"xAODMaker__EventFormatMetaDataTool/EventFormatMetaDataTool"):
        ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool( "EventFormatMetaDataTool")
    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.EventFormatMetaDataTool ]
    # Put MetaData in AOD stream via AugmentedPoolStream_
    # Write all meta data containers
    DAOD_RPVLLStream_Augmented.AddMetaDataItem(dfMetadataItemList())
    # Metadata declared by the sub-systems:
    DAOD_RPVLLStream_Augmented.AddMetaDataItem( objKeyStore._store.metaData() )

## This line provides the 'old' StreamDAOD_RPVLL (which is the Event Stream only)
## for backward compatibility
DAOD_RPVLLStream=DAOD_RPVLLStream_Augmented.GetEventStream()

## Add TAG attribute list to payload data
try:
    DAOD_RPVLLStream_Augmented.GetEventStream().WritingTool.AttributeListKey = EventTagGlobal.AttributeList
except:
    logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

# -- Note that inside below script, assumes that a StreamAOD globals exist
StreamAOD = DAOD_RPVLLStream
protectedInclude( "RecExPers/RecoOutputAODList_jobOptions.py")
DAOD_RPVLLStream_Augmented.AddItem("SkimDecisionCollection#*")
#FIXME HACK remove faulty object
DAOD_RPVLLStream_Augmented.GetEventStream().ItemList = \
        [ e for e in DAOD_RPVLLStream_Augmented.GetEventStream().ItemList \
          if not e in [ 'CaloTowerContainer#HLT_TrigCaloTowerMaker'] ]

if AODFlags.TrackParticleSlimmer or AODFlags.TrackParticleLastHitAndPerigeeSlimmer:
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    if not hasattr(svcMgr, 'ThinningSvc'):
        svcMgr += createThinningSvc( svcName="ThinningSvc", outStreams=[StreamAOD] )

if rec.doHeavyIon():
    protectedInclude ("HIRecExample/heavyion_postOptionsAOD.py")

try:
    if rec.doFileMetaData(): #needed to have xAOD readable outside athena
        theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]
        DAOD_RPVLLStream_Augmented.AddMetaDataItem("xAOD::EventFormat#EventFormat")
except Exception:
    treatException("Problem with extra attributes for xAOD output")

# -- Needs to be removed if we want to exactly mimick the AOD stream
if len(filter(lambda x: x.find("AODSelectSeq") != -1,dir())) > 0:
    topSequence.remove(AODSelectSeq)

