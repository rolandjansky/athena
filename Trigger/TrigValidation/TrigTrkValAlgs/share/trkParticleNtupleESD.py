#--------------------------------------------------------------
# Private Application Configuration option
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = WARNING

TrkParticleAlgsOutputLevel= WARNING
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#--------------------------------------------------------------
theApp.EvtMax = 200
theApp.SkipEvents= 0
 
import AthenaPoolCnvSvc.ReadAthenaPool 
from PartPropSvc.PartPropSvcConf import PartPropSvc 

#--------------------------------------------------------------
# Read list of files
#--------------------------------------------------------------
import os
listOfFiles=[]

dataPath ="/data/ESD/Cosmics/data09_calophys.00135736.express_express.recon.ESD.f157"

import os
for filename in os.listdir(dataPath):
    listOfFiles.append(dataPath+"/"+filename)

ServiceMgr.EventSelector.InputCollections = listOfFiles

#----------------------------------------------------------------
# Tool which writes TrkParticle data into Ntuple
#----------------------------------------------------------------

from TrkValTools.TrkValToolsConf import Trk__BasicValTrkParticleNtupleTool
BasicValTrkParticleNtupleTool = Trk__BasicValTrkParticleNtupleTool("BasicValTrkParticleNtupleTool")
print BasicValTrkParticleNtupleTool
ToolSvc+=BasicValTrkParticleNtupleTool

from TrigTrkValTools.TrigTrkValToolsConf import TrigL2ValidationNtupleTool
L2ValidationNtupleTool= TrigL2ValidationNtupleTool("TrigL2ValidationNtupleTool")
print L2ValidationNtupleTool
ToolSvc+=L2ValidationNtupleTool
L2ValidationNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
#--------------------------------------------------------
# Main Algorithm
#--------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topSequence=AlgSequence() 

from TrigTrkValAlgs.TrigTrkValAlgsConf import Trk__TrigTrkValidationNtupleWriter
TrigTrkValNtupleWriter = Trk__TrigTrkValidationNtupleWriter(name               = 'InDetTrigValNtupleWriter',
                                                            ValTrkParticleNtupleTool = BasicValTrkParticleNtupleTool,
                                                            TrigL2ValidationNtupleTool = L2ValidationNtupleTool,
                                                            NtupleFileName       = 'TRKVAL',
                                                            NtupleDirName        = 'Validation',
                                                            ChainName            = ".*",
                                                            RoiDescrName         = ".*",
                                                            #InDetTrackTrigCollection = ["InDetTrack"],
                                                            InDetTrackTrigCollection = ["TrigIDSCAN_Cosmics", "TrigSiTrack_Cosmics"],
                                                            EFTrackParticleCollection = ["InDetTrigParticleCreationIOTRT_CosmicsN_EFID"],
                                                            #EFTrackParticleCollection = ["HLT_InDetTrigParticleCreation_CosmicsN_EFID"],
                                                            TrackParticleCollection = ["TrackParticleCandidate"],
                                                            doRoI = True
                                                            )

TrigTrkValNtupleWriter.OutputLevel = TrkParticleAlgsOutputLevel
print TrigTrkValNtupleWriter
topSequence += TrigTrkValNtupleWriter

#----------------------------------------------------------
# Tools which writes event property information
#----------------------------------------------------------

from TrkValTools.TrkValToolsConf import Trk__EventPropertyNtupleTool
EventPropertyNtupleTool = Trk__EventPropertyNtupleTool(name='EventPropertyNtupleTool', FillTrtPhase = True, FillTrigger=True)
EventPropertyNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventPropertyNtupleTool
print EventPropertyNtupleTool

from TrkValTools.TrkValToolsConf import Trk__EventToTrackLinkNtupleTool
EventToTrackParticleLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrackParticleLinkNtupleTool', CollectionType = "Rec::TrackParticle")
EventToTrackParticleLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventToTrackParticleLinkNtupleTool
print EventToTrackParticleLinkNtupleTool

EventToTrackParticleEFLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrackParticleEFLinkNtupleTool', CollectionType = "Rec::TrackParticle_Trig")
EventToTrackParticleEFLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventToTrackParticleEFLinkNtupleTool
print EventToTrackParticleEFLinkNtupleTool

EventToTrigInDetTrackLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrigInDetTrackLinkNtupleTool', CollectionType = "InDetTrack_Trig")
EventToTrigInDetTrackLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventToTrigInDetTrackLinkNtupleTool
print EventToTrigInDetTrackLinkNtupleTool

TrigTrkValNtupleWriter.EventPropertyNtupleTools =[EventPropertyNtupleTool, EventToTrackParticleLinkNtupleTool, EventToTrackParticleEFLinkNtupleTool, EventToTrigInDetTrackLinkNtupleTool]

#------------------------------------------------------------------------------------
# Tools which writes RoI information
#------------------------------------------------------------------------------------

from TrigTrkValTools.TrigTrkValToolsConf import Trk__RoIToTrackLinkNtupleTool
RoIToTrackParticleLinkNtupleTool = Trk__RoIToTrackLinkNtupleTool(name='RoIToTrackParticleLinkNtupleTool')
RoIToTrackParticleLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += RoIToTrackParticleLinkNtupleTool
print RoIToTrackParticleLinkNtupleTool

RoIToTrigInDetTrackLinkNtupleTool = Trk__RoIToTrackLinkNtupleTool(name='RoIToTrigInDetTrackLinkNtupleTool')
RoIToTrigInDetTrackLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += RoIToTrigInDetTrackLinkNtupleTool
print RoIToTrigInDetTrackLinkNtupleTool
    
from TrigTrkValTools.TrigTrkValToolsConf import Trk__RoIPropertyNtupleTool
RoIPropertyNtupleTool = Trk__RoIPropertyNtupleTool(name='RoIPropertyNtupleTool')
RoIPropertyNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel

ToolSvc += RoIPropertyNtupleTool
print RoIPropertyNtupleTool

TrigTrkValNtupleWriter.RoIPropertyNtupleTools = [ RoIPropertyNtupleTool, RoIToTrackParticleLinkNtupleTool, RoIToTrigInDetTrackLinkNtupleTool ]
print TrigTrkValNtupleWriter.RoIPropertyNtupleTools

#-------------------------------------------------------------------------------------
# Trigger Decision Tool
#-------------------------------------------------------------------------------------

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

#-------------------------------------------------------------------------------------
# Trigger Configuration
#-------------------------------------------------------------------------------------
# flags needed for TriggerConfigGetter
from RecExConfig.RecFlags  import rec
rec.readAOD=False
rec.readRDO=False
rec.readESD=True
rec.doWriteAOD=False
rec.doWriteESD=False

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod:
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

#-----------------------------------------------------------------------
# Root Services
#----------------------------------------------------------------------- 
# setup TTree registration Service 
# save ROOT histograms and Ntuple
theApp.HistogramPersistency = 'ROOT'
from GaudiSvc.GaudiSvcConf import THistSvc 
ServiceMgr += THistSvc() 
ServiceMgr.THistSvc.Output = [ "TRKVAL DATAFILE='TrkParticleNtuple.root' TYPE='ROOT' OPT='RECREATE'" ]
theApp.Dlls += [ 'RootHistCnv' ]




