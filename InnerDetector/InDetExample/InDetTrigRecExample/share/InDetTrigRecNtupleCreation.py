#this file is included by RecExCommon - 

#TrkParticleAlgsOutputLevel= WARNING

from TrkValTools.TrkValToolsConf import Trk__BasicValTrkParticleNtupleTool
BasicValTrkParticleNtupleTool = Trk__BasicValTrkParticleNtupleTool("BasicValTrkParticleNtupleTool")
#BasicValTrkParticleNtupleTool.OutputLevel=VERBOSE
print BasicValTrkParticleNtupleTool
ToolSvc+=BasicValTrkParticleNtupleTool

from TrigTrkValTools.TrigTrkValToolsConf import TrigL2ValidationNtupleTool
L2ValidationNtupleTool= TrigL2ValidationNtupleTool("TrigL2ValidationNtupleTool")
print L2ValidationNtupleTool
ToolSvc+=L2ValidationNtupleTool
#L2ValidationNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel


from TrigTrkValAlgs.TrigTrkValAlgsConf import Trk__TrigTrkValidationNtupleWriter


TrigTrkValNtupleWriter = Trk__TrigTrkValidationNtupleWriter(name               = 'InDetTrigValNtupleWriter',
                                                            ValTrkParticleNtupleTool = BasicValTrkParticleNtupleTool,
                                                            TrigL2ValidationNtupleTool = L2ValidationNtupleTool,
                                                            NtupleFileName       = 'TRKVAL',
                                                            NtupleDirName        = 'Validation',
                                                            ChainName            = ".*",
                                                            RoiDescrName         = ".*",
                                                            #InDetTrackTrigCollection = ["InDetTrack"],
                                                            #InDetTrackTrigCollection = ["TrigIDSCAN_Cosmics"],
                                                            EFTrackParticleCollection = ["InDetTrigParticleCreation_Tau_EFID"],
                                                            #EFTrackParticleCollection = ["HLT_InDetTrigParticleCreation_CosmicsN_EFID"],
                                                            #TrackParticleCollection = ["TrackParticleCandidate"],
                                                            doRoI = True
                                                            )

#TrigTrkValNtupleWriter.OutputLevel = VERBOSE
print TrigTrkValNtupleWriter
topSequence += TrigTrkValNtupleWriter

#----------------------------------------------------------
# Tools which writes event property information
#----------------------------------------------------------

from TrkValTools.TrkValToolsConf import Trk__EventPropertyNtupleTool
EventPropertyNtupleTool = Trk__EventPropertyNtupleTool(name='EventPropertyNtupleTool')
#EventPropertyNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventPropertyNtupleTool
print EventPropertyNtupleTool

from TrkValTools.TrkValToolsConf import Trk__EventToTrackLinkNtupleTool
EventToTrackParticleLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrackParticleLinkNtupleTool', CollectionType = "Rec::TrackParticle")
#EventToTrackParticleLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventToTrackParticleLinkNtupleTool
print EventToTrackParticleLinkNtupleTool

EventToTrackParticleEFLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrackParticleEFLinkNtupleTool', CollectionType = "Rec::TrackParticle_Trig")
#EventToTrackParticleEFLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventToTrackParticleEFLinkNtupleTool
print EventToTrackParticleEFLinkNtupleTool

EventToTrigInDetTrackLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrigInDetTrackLinkNtupleTool', CollectionType = "InDetTrack_Trig")
#EventToTrigInDetTrackLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += EventToTrigInDetTrackLinkNtupleTool
print EventToTrigInDetTrackLinkNtupleTool

TrigTrkValNtupleWriter.EventPropertyNtupleTools =[EventPropertyNtupleTool, EventToTrackParticleLinkNtupleTool, EventToTrackParticleEFLinkNtupleTool, EventToTrigInDetTrackLinkNtupleTool]



#------------------------------------------------------------------------------------
# Tools which writes RoI information
#------------------------------------------------------------------------------------

from TrigTrkValTools.TrigTrkValToolsConf import Trk__RoIToTrackLinkNtupleTool
RoIToTrackParticleLinkNtupleTool = Trk__RoIToTrackLinkNtupleTool(name='RoIToTrackParticleLinkNtupleTool')
#RoIToTrackParticleLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += RoIToTrackParticleLinkNtupleTool
print RoIToTrackParticleLinkNtupleTool

RoIToTrigInDetTrackLinkNtupleTool = Trk__RoIToTrackLinkNtupleTool(name='RoIToTrigInDetTrackLinkNtupleTool', trackToolType='TrigInDetTrack')
#RoIToTrigInDetTrackLinkNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel
ToolSvc += RoIToTrigInDetTrackLinkNtupleTool
print RoIToTrigInDetTrackLinkNtupleTool
    
from TrigTrkValTools.TrigTrkValToolsConf import Trk__RoIPropertyNtupleTool
RoIPropertyNtupleTool = Trk__RoIPropertyNtupleTool(name='RoIPropertyNtupleTool')
#RoIPropertyNtupleTool.OutputLevel = TrkParticleAlgsOutputLevel

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


# # To read files with trigger config stored as in-file meta-data,
# # i.e. 13.0.40 and above: ds
# # To read AOD produced with 13.0.30 you need to change ds to aod:
# from TriggerJobOpts.TriggerFlags import TriggerFlags
# TriggerFlags.configurationSourceList = ['ds']

# # set up trigger config service
# from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
# cfg =  TriggerConfigGetter()

#origWriter = topSequence.InDetValNtupleWriter
#TrigTrkValNtupleWriter = Trk__TrigTrkValidationNtupleWriter(name               = 'InDetTrigValNtupleWriter')
#for i in origWriter.properties():
#    prop = origWriter.__slots__[i]
#    TrigTrkValNtupleWriter.__slots__[i] = prop
    
