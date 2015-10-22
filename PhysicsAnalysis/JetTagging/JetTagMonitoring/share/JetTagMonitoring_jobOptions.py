
monManJetTag = AthenaMonManager(name="JetTagMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                DataType            = DQMonFlags.monManDataType())
topSequence += monManJetTag

from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
trackSelectorTool = Analysis__TrackSelector (
    name           = "Analysis::TrackSelector"
    )
ToolSvc += trackSelectorTool
#trackSelectorTool.OutputLevel = VERBOSE
trackSelectorTool.nHitBLayer = 0

from JetTagMonitoring.JetTagMonitoringConf import JetTagMonitoring
jetTagMonTool = JetTagMonitoring (
    name           = "jetTagMonTool",
    )
ToolSvc += jetTagMonTool
monManJetTag.AthenaMonTools += [ jetTagMonTool ]

# remove events with non-collision BCIDs from your monitoring tools 
# see https://indico.cern.ch/getFile.py/access?contribId=1&resId=1&materialId=slides&confId=135968
jetTagMonTool.FilterTools += [ monFilledBunchFilterTool ]

# remove events with intolerable LAr defects
from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
monbadlb = GetLArBadLBFilterTool()
jetTagMonTool.FilterTools += [ monbadlb ] 

jetTagMonTool.OutputLevel = INFO
jetTagMonTool.JetContainer = "AntiKt4EMTopoJets"
jetTagMonTool.DQcuts = True
jetTagMonTool.PriVtxTrkMinCut = 4
jetTagMonTool.D0_min_cuts = 0
jetTagMonTool.D0_max_cuts = 1
jetTagMonTool.JetPt_cuts = 15000
jetTagMonTool.JetEta_cuts = 2.5
jetTagMonTool.nTrk_cuts = 1

##### REL20 recommendations
##### Email Tim Scanlon  23.04.2015

jetTagMonTool.SV1IP3D_weight_cut         = 0.0
jetTagMonTool.MV2c20_50_cut=0.751629
jetTagMonTool.MV2c20_70_cut=-0.10694
jetTagMonTool.MV2c20_80_cut=-0.658252

if not rec.doInDet:
    jetTagMonTool.UseTrackSelector = False
else:
    jetTagMonTool.UseTrackSelector = True

# Disable trigger for now since it produces an error in q-test q221
#if DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
#    print "jetTagMonTool will use TrigDecisionTool instance: %s" % DQMonFlags.nameTrigDecTool()
#    jetTagMonTool.TrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
#    jetTagMonTool.TriggerChain = "L1_J.*, L1_[3-5]J.* ,EF_e24vh_medium1, EF_mu18_IDTrkNoCut_tight, L1_RD0_FILLED"   #"L1_J10"   even older #"EF_mu4_L1J10_matched"
#    jetTagMonTool.UseTrigDecisionTool = True
#else:
#    print "WARNING!!! jetTagMonTool will NOT use TrigDecisionTool."
#    jetTagMonTool.UseTrigDecisionTool = False
jetTagMonTool.UseTrigDecisionTool = False

