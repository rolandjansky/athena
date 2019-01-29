
monManJetTag = AthenaMonManager(name="JetTagMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                DataType            = DQMonFlags.monManDataType())

from BTagging.BTaggingConfiguration_CommonTools import toolBTagTrackToVertexTool as toolBTagTrackToVertexTool
trackToVertexTool = toolBTagTrackToVertexTool('BTagTrackToVertexTool')
options = {}
options.setdefault('trackToVertexTool', trackToVertexTool)
options.setdefault('name', "Analysis::TrackSelector")
options.setdefault('nHitBLayer', 0)
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
trackSelectorTool = Analysis__TrackSelector (**options)
ToolSvc += trackSelectorTool
#trackSelectorTool.OutputLevel = VERBOSE

from JetTagMonitoring.JetTagMonitoringConf import JetTagMonitoring
jetTagMonTool = JetTagMonitoring (
    name           = "jetTagMonTool",
    )
# remove events with non-collision BCIDs from your monitoring tools 
# see https://indico.cern.ch/getFile.py/access?contribId=1&resId=1&materialId=slides&confId=135968
from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
jetTagMonTool.FilterTools += [ GetFilledBunchFilterTool() ]

# remove events with intolerable LAr defects
from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
jetTagMonTool.FilterTools += [ GetLArBadLBFilterTool() ] 

jetTagMonTool.OutputLevel = INFO
jetTagMonTool.JetContainer = "AntiKt4EMTopoJets"
jetTagMonTool.TrackParticleContainer = "InDetTrackParticles"
jetTagMonTool.PrimaryVertexContainer = "PrimaryVertices"
jetTagMonTool.ElectronContainer = "Electrons"
jetTagMonTool.MuonContainer = "Muons"

jetTagMonTool.DQcuts = True
jetTagMonTool.PriVtxTrkMinCut = 4
jetTagMonTool.D0_min_cuts = 0
jetTagMonTool.D0_max_cuts = 1
jetTagMonTool.JetPt_cuts = 15
jetTagMonTool.JetEta_cuts = 2.5
jetTagMonTool.nTrk_cuts = 1

jetTagMonTool.MV_algorithmName = "MV2c10"

# taken from webpage (https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarksRelease20) 14 june 2017
jetTagMonTool.SV1IP3D_weight_cut = 0.0
jetTagMonTool.MV_60_cut = 0.934906
jetTagMonTool.MV_70_cut = 0.8244273
jetTagMonTool.MV_77_cut = 0.645925
jetTagMonTool.MV_85_cut = 0.1758475

if not rec.doInDet:
    jetTagMonTool.UseTrackSelector = False
else:
    jetTagMonTool.UseTrackSelector = True

# Setting up the trigger decision tool
# for trigger-aware monitoring

jetTagMonTool.UseTrigDecisionTool = True # added by SARA
if DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
    print "jetTagMonTool will use TrigDecisionTool instance: %s" % DQMonFlags.nameTrigDecTool()
#    jetTagMonTool.TrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
    jetTagMonTool.UseTrigDecisionTool = True
else:
    print "WARNING!!! jetTagMonTool will NOT use TrigDecisionTool."
    jetTagMonTool.UseTrigDecisionTool = False

if (rec.triggerStream()=='express'): # added by SARA # don't require trigger if running on express stream
    jetTagMonTool.UseTrigDecisionTool = False # added by SARA

jetTagMonTool.ElectronTrigger_2016 = "HLT_e26_lhtight_nod0_ivarloose"; # added by SARA
jetTagMonTool.MuonTrigger_2016 = "HLT_mu26_ivarmedium"; # added by SARA
jetTagMonTool.JetTrigger_2016 = "HLT_j15"; # added by SARA
jetTagMonTool.ElectronTrigger_2017 = "HLT_e28_lhtight_nod0_ivarloose"; # added by SARA
jetTagMonTool.MuonTrigger_2017 = "HLT_mu26_ivarmedium"; # added by SARA
jetTagMonTool.JetTrigger_2017 = "HLT_j15"; # added by SARA

#ToolSvc += jetTagMonTool
monManJetTag.AthenaMonTools += [ jetTagMonTool ]
topSequence += monManJetTag
