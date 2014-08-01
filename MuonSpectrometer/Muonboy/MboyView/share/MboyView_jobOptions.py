#-----------------------------------------------------------------
#Muonboy View
#-----------------------------------------------------------------
#Control Flags
if 'doPersint' in dir():  
   if doPersint:
        doMboyView=True
   else:
        doMboyView=False

if not 'doMboyView' in dir():  
     doMboyView=False

#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "MboyView"]



from MboyView.MboyViewConf import MboyViewEventSelectionTool
ConfiguredMboyViewEventSelectionTool = MboyViewEventSelectionTool(
    name = "ConfiguredMboyViewEventSelectionTool")
ToolSvc += ConfiguredMboyViewEventSelectionTool
print ConfiguredMboyViewEventSelectionTool

from MboyView.MboyViewConf import MboyViewGenEventSelectionTool
ConfiguredMboyViewGenEventSelectionTool = MboyViewGenEventSelectionTool(
    name = "ConfiguredMboyViewGenEventSelectionTool")
ToolSvc += ConfiguredMboyViewGenEventSelectionTool
print ConfiguredMboyViewGenEventSelectionTool

from MboyView.MboyViewConf import MboyViewEventInfoTool
ConfiguredMboyViewEventInfoTool = MboyViewEventInfoTool(
    name = "ConfiguredMboyViewEventInfoTool")
ToolSvc += ConfiguredMboyViewEventInfoTool
print ConfiguredMboyViewEventInfoTool

from MboyView.MboyViewConf import MboyViewMuonRecDigiStoreTDSTool
ConfiguredMboyViewMuonRecDigiStoreTDSTool = MboyViewMuonRecDigiStoreTDSTool(
    name = "ConfiguredMboyViewMuonRecDigiStoreTDSTool")
ToolSvc += ConfiguredMboyViewMuonRecDigiStoreTDSTool
print ConfiguredMboyViewMuonRecDigiStoreTDSTool

from MboyView.MboyViewConf import MboyViewTrackRecordCollectionTool
ConfiguredMboyViewTrackRecordCollectionTool = MboyViewTrackRecordCollectionTool(
    name = "ConfiguredMboyViewTrackRecordCollectionTool")
ToolSvc += ConfiguredMboyViewTrackRecordCollectionTool
print ConfiguredMboyViewTrackRecordCollectionTool

from MboyView.MboyViewConf import MboyViewCaloTool
ConfiguredMboyViewCaloTool = MboyViewCaloTool(
    name = "ConfiguredMboyViewCaloTool")
ToolSvc += ConfiguredMboyViewCaloTool
print ConfiguredMboyViewCaloTool

from MboyView.MboyViewConf import MboyViewCombinedMuonContainerTool
ConfiguredMboyViewCombinedMuonContainerTool = MboyViewCombinedMuonContainerTool(
    name = "ConfiguredMboyViewCombinedMuonContainerTool")
ToolSvc += ConfiguredMboyViewCombinedMuonContainerTool
print ConfiguredMboyViewCombinedMuonContainerTool

from MboyView.MboyViewConf import MboyViewTrackParticleContainerTool
ConfiguredMboyViewTrackParticleContainerTool = MboyViewTrackParticleContainerTool(
    name = "ConfiguredMboyViewTrackParticleContainerTool")
ToolSvc += ConfiguredMboyViewTrackParticleContainerTool
print ConfiguredMboyViewTrackParticleContainerTool

from MboyView.MboyViewConf import MboyViewTrkTrackCollectionTool
ConfiguredMboyViewTrkTrackCollectionTool = MboyViewTrkTrackCollectionTool(
    name = "ConfiguredMboyViewTrkTrackCollectionTool")
ToolSvc += ConfiguredMboyViewTrkTrackCollectionTool
print ConfiguredMboyViewTrkTrackCollectionTool

from MboyView.MboyViewConf import MboyViewTrkSegmentCollectionTool
ConfiguredMboyViewTrkSegmentCollectionTool = MboyViewTrkSegmentCollectionTool(
    name = "ConfiguredMboyViewTrkSegmentCollectionTool")
ToolSvc += ConfiguredMboyViewTrkSegmentCollectionTool
print ConfiguredMboyViewTrkSegmentCollectionTool

from MboyView.MboyViewConf import MboyViewMcEventCollectionTool
ConfiguredMboyViewMcEventCollectionTool = MboyViewMcEventCollectionTool(
    name = "ConfiguredMboyViewMcEventCollectionTool")
ToolSvc += ConfiguredMboyViewMcEventCollectionTool
print ConfiguredMboyViewMcEventCollectionTool

from MboyView.MboyViewConf import MboyViewEgammaContainerTool
ConfiguredMboyViewEgammaContainerTool = MboyViewEgammaContainerTool(
    name = "ConfiguredMboyViewEgammaContainerTool")
ToolSvc += ConfiguredMboyViewEgammaContainerTool
print ConfiguredMboyViewEgammaContainerTool

#apolici
from MboyView.MboyViewConf import MboyViewJetContainerTool
ConfiguredMboyViewJetContainerTool = MboyViewJetContainerTool(
    name = "ConfiguredMboyViewJetContainerTool")
ToolSvc += ConfiguredMboyViewJetContainerTool
print ConfiguredMboyViewJetContainerTool

from MboyView.MboyViewConf import MboyViewMETContainerTool
ConfiguredMboyViewMETContainerTool = MboyViewMETContainerTool(
    name = "ConfiguredMboyViewMETContainerTool")
ToolSvc += ConfiguredMboyViewMETContainerTool
print ConfiguredMboyViewMETContainerTool

from MboyView.MboyViewConf import MboyViewL1RoIContainerTool
ConfiguredMboyViewL1RoIContainerTool = MboyViewL1RoIContainerTool(
    name = "ConfiguredMboyViewL1RoIContainerTool")
ToolSvc += ConfiguredMboyViewL1RoIContainerTool
print ConfiguredMboyViewL1RoIContainerTool
#apolici

from MboyView.MboyViewConf import MboyViewVertexContainerTool
ConfiguredMboyViewVertexContainerTool = MboyViewVertexContainerTool(
    name = "ConfiguredMboyViewVertexContainerTool")
ToolSvc += ConfiguredMboyViewVertexContainerTool
print ConfiguredMboyViewVertexContainerTool

from MboyView.MboyViewConf import MboyViewIDRecDigiTool
ConfiguredMboyViewIDRecDigiTool = MboyViewIDRecDigiTool(
    name = "ConfiguredMboyViewIDRecDigiTool")
ToolSvc += ConfiguredMboyViewIDRecDigiTool 
print ConfiguredMboyViewIDRecDigiTool

#-----------------------------------------------------------------
#MboyView: 
theApp.TopAlg += [ "MboyView" ]

MboyView = Algorithm( "MboyView" )

if 'readAOD' in dir():  
     if readAOD:
        MboyView.ViewDigits                 = 0
        MboyView.ViewTrackRecordCollection  = 0
        MboyView.ViewCombinedMuonContainer  = 0
        MboyView.ViewTrackParticleContainer = 1
        MboyView.ViewTrkTrackCollection     = 0
        MboyView.ViewTrkSegmentCollection   = 0
        MboyView.ViewMcEventCollection      = 0
        MboyView.ViewEgammaContainer        = 0
#apolici
        MboyView.ViewJetContainer           = 0
        MboyView.ViewMETContainer           = 0
        MboyView.ViewL1RoIContainer         = 1
#apolici

if doMboyView:
   MboyView.SwitchOff = 0
   MboySvc = Service( "MboySvc" )
   MboySvc.GiveRsLineToDigit  = 1

