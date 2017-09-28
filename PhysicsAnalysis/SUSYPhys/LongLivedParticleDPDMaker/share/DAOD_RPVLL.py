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
# rec.doWriteAOD.set_Value_and_Lock(True)

include ("RecExCond/RecExCommon_flags.py")
include ("RecExCommon/RecExCommon_topOptions.py")

MSMgr.Print()


## Mimicking RecExample/RecExCommon/share/RecExCommon_topOptions.py
## See from line 1305
# from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# from AthenaServices.Configurables import ThinningSvc
# if not hasattr(svcMgr, 'ThinningSvc'):
#     svcMgr += ThinningSvc(OutputLevel=INFO)
#     svcMgr.ThinningSvc.Streams += [streamName]

# # cannot redo the slimming if readAOD and writeAOD
# if not rec.readAOD():
#     if rec.doEgamma() and (AODFlags.Photon or AODFlags.Electron):
#         doEgammaPhoton = AODFlags.Photon
#         doEgammaElectron= AODFlags.Electron
#         from egammaRec.egammaAODGetter import egammaAODGetter
#         egammaAODGetter()
#         if AODFlags.egammaTrackSlimmer:
#             from egammaRec.egammaTrackSlimmer import egammaTrackSlimmer
#             egammaTrackSlimmer()
#         if AODFlags.TauTrackSlimmer:
#             protectedInclude("tauRec/tauMerged_trackslim_jobOptions.py")

# pdr.flag_domain('output')

##====================================================================
## Define the DAOD RPVLL output stream
##====================================================================
if primDPD.isVirtual() == False:
    DAOD_RPVLLStream_Augmented=MSMgr.NewPoolRootStream( streamName, fileName,asAlg = True )
if primDPD.isVirtual() == True:
    DAOD_RPVLLStream_Augmented=MSMgr.NewVirtualStream( streamName, fileName )

# if rec.doFileMetaData():
#     # Trigger tool
#     if not hasattr(ToolSvc,"xAODMaker__TriggerMenuMetaDataTool/TriggerMenuMetaDataTool"):
#         ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool")
#     svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]
#     # EventFormat tool
#     if not hasattr(ToolSvc,"xAODMaker__EventFormatMetaDataTool/EventFormatMetaDataTool"):
#         ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool( "EventFormatMetaDataTool")
#     svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.EventFormatMetaDataTool ]
#     # Put MetaData in AOD stream via AugmentedPoolStream_
#     # Write all meta data containers
#     DAOD_RPVLLStream_Augmented.AddMetaDataItem(dfMetadataItemList())
#     # Metadata declared by the sub-systems:
#     DAOD_RPVLLStream_Augmented.AddMetaDataItem( objKeyStore._store.metaData() )

## This line provides the 'old' StreamDAOD_RPVLL (which is the Event Stream only)
## for backward compatibility
# DAOD_RPVLLStream_Augmented = StreamAOD_Augmented
DAOD_RPVLLStream=DAOD_RPVLLStream_Augmented.GetEventStream()


## Add TAG attribute list to payload data
try:
    DAOD_RPVLLStream_Augmented.GetEventStream().WritingTool.AttributeListKey = EventTagGlobal.AttributeList
except:
    logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

# -- Note that inside below script, assumes that a global StreamAOD and StreamAOD_Augmented exist
StreamAOD = DAOD_RPVLLStream
StreamAOD_Augmented = DAOD_RPVLLStream_Augmented

# protectedInclude( "RecExPers/RecoOutputAODList_jobOptions.py")
# DAOD_RPVLLStream_Augmented.AddItem("SkimDecisionCollection#*")
# #FIXME HACK remove faulty object
# DAOD_RPVLLStream.ItemList = \
#         [ e for e in DAOD_RPVLLStream.ItemList \
#           if not e in [ 'CaloTowerContainer#HLT_TrigCaloTowerMaker'] ]



from AthenaCommon.Resilience import treatException,protectedInclude


# from AthenaCommon.Logging import logging
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )

# from RecExConfig.RecFlags import rec

from AthenaCommon.KeyStore import CfgItemList

# Start with an empty list:
fullAODList = CfgItemList( "AodList" )

# EventInfo stuff
fullItemList = ["xAOD::EventInfo#*","xAOD::EventAuxInfo#*"]

try:
    # include ( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py")
    fullAODList += CfgItemList( "EventAthenaPool_RPVLL",
                            items = fullItemList,
                            allowWildCard = True )
except Exception:
    treatException("Could not include EventAthenaPool/EventAthenaPoolItemList_joboptions.py")

# RawInfoSummaryForTag
try:
    fullItemList = []
    protectedInclude ( "EventTagAthenaPOOL/EventTagAthenaPOOLItemList_joboptions.py")
    fullAODList += CfgItemList( "EventTagAthenaPOOL_RPVLL",
                                items = fullItemList,
                                allowWildCard = True )
except Exception:
    treatException("Could not include EventTagAthenaPOOL/EventTagAthenaPOOLItemList_joboptions.py")

# MC Event Collection. Should be moved to a separate jobO
if rec.doTruth():
    McTruthAODList=["xAOD::TruthEventContainer#TruthEvents",
                    "xAOD::TruthEventAuxContainer#TruthEventsAux.",
                    "xAOD::TruthParticleContainer#TruthParticles",
                    "xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension",
                    "xAOD::TruthVertexContainer#TruthVertices",
                    "xAOD::TruthVertexAuxContainer#TruthVerticesAux.",
                    "xAOD::TruthPileupEventContainer#TruthPileupEvents",
                    "xAOD::TruthPileupEventAuxContainer#TruthPileupEventsAux."]
    fullAODList += CfgItemList( "McTruthAod_RPVLL",
                                items = McTruthAODList )

# Inner Detector
if DetFlags.detdescr.ID_on():
    try:
        include ("InDetRecExample/WriteInDetAOD.py")
        from InDetRecExample.InDetKeys import InDetKeys
        fullAODList+= CfgItemList(
                     "InDetAod_RPVLL",
                     items = InDetAODList
                     )
    except Exception:
       treatException("Could not include InDetRecExample/WriteInDetAOD.py" )


# FTK
if DetFlags.detdescr.FTK_on():
    protectedInclude ("FTK_RecExample/WriteFTK_AOD.py")
    fullAODList += CfgItemList( "FTKAod_RPVLL", items = FtkAODList )

# Calorimeters
if DetFlags.detdescr.Calo_on():
    try:
       include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py")
       fullAODList += CfgItemList( "CaloAod_RPVLL", items = CaloAODList )
    except Exception:
       treatException("Could not include CaloRecEx/CaloRecOutputItemList_jobOptions.py" )

# Muon Spectrometer
if DetFlags.detdescr.Muon_on():
    try:
        include("MuonRecExample/MuonRecOutputItemList_jobOptions.py")
        fullAODList += CfgItemList( "MuonAod_RPVLL", items = MuonAODList )

    except Exception:
        treatException("Could not MuonRecExample/MuonRecOutputItemList_jobOptions.py")

if DetFlags.detdescr.ALFA_on():
    protectedInclude("ForwardRec/ALFARec_OuputItemList_jobOptions.py")
    fullAODList += CfgItemList("ALFA_RPVLL", items = AlfaItemList)

if DetFlags.detdescr.AFP_on():
    include ("ForwardRec/AFP_Rec_OutputItemList_jobOptions.py")
    fullAODList += CfgItemList("AFP_RPVLL",items = AFP_ItemList)

# Tau:
try:
    protectedInclude ("tauRec/TauAODList.py")
    fullAODList += CfgItemList( "TauAod_RPVLL", items = TauAODList )
except Exception:
    treatException("Could not tauRec/TauAODList.py")

# MissingET:
try:
    include ("METReconstruction/METReconstructionOutputAODList_jobOptions.py")
    fullAODList += CfgItemList( "MissingETAod_RPVLL", items = MissingETAODList )
except Exception:
    treatException("Could not load MET item list")

# Jet classes.
try:
   from JetRec.JetRecUtils import retrieveAODList
   jetAODList = retrieveAODList()
   fullAODList += CfgItemList( "jetAOD_RPVLL", items = jetAODList )
except Exception:
   treatException("Could not load jet item list")

#BTagging
try:
    include("BTagging/BTaggingReconstructionOutputAODList_jobOptions.py")
    fullAODList += CfgItemList( "BTaggingAod_RPVLL", items = BTaggingAODList, allowWildCard = True )
except Exception:
    treatException("Could not load BTagging item list")

#isolation, EventShape containers for ED correction
try:
    include("IsolationAlgs/IsoEventShapeOutputItemList_jobOptions.py")
    fullAODList += CfgItemList( "Isolation_RPVLL", items = IsoAODESList)
except Exception:
    treatException("Could not load IsoEventShape item list")

if rec.doEgamma():
    try:
        include ( "egammaRec/egammaOutputItemList_jobOptions.py" )
        fullAODList += CfgItemList( "egammaAod_RPVLL", items = egammaAODList )
    except Exception:
        treatException("Could not load egammaRec/egammaOutputItemList_jobOptions.py" )

#eflow
try:
    include("eflowRec/eflowRecAODList.py")
    fullAODList += CfgItemList("eflowAod_RPVLL",items=eflowRecAODList)
except:
    treatException("Could not load eflow AOD item list")
    pass


# Muon Spectrometer
if DetFlags.detdescr.Muon_on() or DetFlags.detdescr.Calo_on():
    try:
        include("MuonCombinedRecExample/MuonCombined_OutputItemsAOD.py")
        fullAODList += CfgItemList( "MuonCombinedAod_RPVLL", items = MuonCombinedAODList )
    except Exception:
        treatException("Could not load MCRE/MuonCombined_OutputItemsAOD.py" )

trackParticleCellAssociationList=["xAOD::CaloClusterContainer#InDetTrackParticlesAssociatedClusters",
                                  "xAOD::CaloClusterAuxContainer#InDetTrackParticlesAssociatedClustersAux.",
                                  "CaloClusterCellLinkContainer#InDetTrackParticlesAssociatedClusters_links",
                                  "xAOD::TrackParticleClusterAssociationContainer#InDetTrackParticlesClusterAssociations",
                                  "xAOD::TrackParticleClusterAssociationAuxContainer#InDetTrackParticlesClusterAssociationsAux."]
fullAODList += CfgItemList("trackParticleCellAssoAod_RPVLL",items=trackParticleCellAssociationList)

# Atlfast, note ESD and AOD lists are equal

from RecExConfig.RecAlgsFlags import recAlgs
from ParticleBuilderOptions.AODFlags import AODFlags
if recAlgs.doAtlfast() or AODFlags.FastSimulation :
    try:
        include ("AtlfastAlgs/AtlfastOutputList_jobOptions.py")
        fullAODList += CfgItemList( "AtlfastAod_RPVLL", items = AtlfastAODList)
    except Exception:
        treatException("could not import AtlfastAlgs/AtlfastOutputList_jobOptions.py")


# Common items: tracks and cells for e/mu/tau
fullAODList += CfgItemList( "CommonAod_RPVLL",
                             #items = [ "TrackCollection#Tracks", "CaloCellContainer#AODCellContainer" ] ) # Turned off for DC14 (no slimming yet)
                            items = [ "CaloCellContainer#AODCellContainer" ] )


# Request for W. Ehrenfeld: Add Calibration hits to ESD/AOD files if present in input RDO
if rec.doWriteCalibHits():
    fullAODList += CfgItemList( "CalibrationHits_RPVLL",
                                items=["CaloCalibrationHitContainer#LArCalibrationHitActive",
                                       "CaloCalibrationHitContainer#LArCalibrationHitInactive",
                                       "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial",
                                       "CaloCalibrationHitContainer#TileCalibrationDMHitCnt",
                                       "CaloCalibrationHitContainer#TileCalibrationCellHitCnt",
                                       ]
                                )



# Heavy Ion:
if rec.doHeavyIon() or rec.doHIP():
    try:
        include ("HIRecExample/HIRecOutputAODList_jobOptions.py")
        fullAODList += CfgItemList( "HeavyIonsAod", items = HIAODItemList )
    except Exception:
        treatException("Could not load HIRecExample/HIRecOutputAODList_jobOptions.py")

# ring-shaped calorimetry
if rec.doCaloRinger():
    try:
        include ( "CaloRingerAlgs/CaloRingerOutputItemList_jobOptions.py" )
        fullAODList += CfgItemList( "caloRingerAod_RPVLL", items = caloRingerAODList )
        StreamAOD_Augmented.AddMetaDataItem( caloRingerMetaDataList )
    except Exception:
        treatException("Could not load CaloRingerAlgs/CaloRingerOutputItemList_jobOptions.py" )


# now merge the explicit AOD list to the one coming from ObjKeyStore
# (more and more will be taken from ObjKeyStore)
from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
theCKS=CfgKeyStore("KeyStore")
CILMergeAOD=CfgItemList("AODMerge",allowWildCard=True)
CILMergeAOD.add(fullAODList())
CILMergeAOD.add(theCKS.streamAOD())

#now prune the itemlist from the object which are soft linked

# this doe not work CILMergeAOD.remove("DataVector<INavigable4Momentum>")
#this does
for k in CILMergeAOD('DataVector<INavigable4Momentum>'):
    CILMergeAOD.removeItem(k)

# The only items declared in StreamAOD are those written here.
# Note the use of "=" instead of "+=" is deliberate

DAOD_RPVLLStream.ItemList = CILMergeAOD()


DAOD_RPVLLStream.ItemList+=['xAOD::TrackParticleContainer#*',
                            'xAOD::TrackParticleAuxContainer#*',
                            'xAOD::VertexContainer#*',
                            'xAOD::VertexAuxContainer#*'    ]



# if AODFlags.TrackParticleSlimmer or AODFlags.TrackParticleLastHitAndPerigeeSlimmer:
#     from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#     from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#     if not hasattr(svcMgr, 'ThinningSvc'):
#         svcMgr += createThinningSvc( svcName="ThinningSvc", outStreams=[StreamAOD] )

# if rec.doHeavyIon():
#     protectedInclude ("HIRecExample/heavyion_postOptionsAOD.py")

try:
    if rec.doFileMetaData(): #needed to have xAOD readable outside athena
        theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]
        DAOD_RPVLLStream_Augmented.AddMetaDataItem("xAOD::EventFormat#EventFormat")
except Exception:
    treatException("Problem with extra attributes for xAOD output")

# -- Needs to be removed if we want to exactly mimick the AOD stream
# if len(filter(lambda x: x.find("AODSelectSeq") != -1,dir())) > 0:
#     topSequence.remove(AODSelectSeq)

