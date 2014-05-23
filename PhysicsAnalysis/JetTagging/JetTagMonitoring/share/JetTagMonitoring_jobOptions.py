
monManJetTag = AthenaMonManager(name="JetTagMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                DataType            = DQMonFlags.monManDataType())
topSequence += monManJetTag

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
jetTagMonTool.JetContainer = "AntiKt4TopoEMJets"
jetTagMonTool.DQcuts = True
jetTagMonTool.PriVtxTrkMinCut = 4
jetTagMonTool.D0_min_cuts = 0
jetTagMonTool.D0_max_cuts = 1
jetTagMonTool.JetPt_cuts = 15000
jetTagMonTool.JetEta_cuts = 2.5
jetTagMonTool.nTrk_cuts = 1

##### EPS recommendations
##### https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Analysis16#Calibrations_to_be_used_for_EPS

jetTagMonTool.SV0_weight_cut             = 5.85
jetTagMonTool.SV1IP3D_weight_cut         = 4.5
jetTagMonTool.JetFitterCOMBNN_weight_cut = 2.0
#jetTagMonTool.MV1_weight_cut = 0.601713
jetTagMonTool.MV1_weight_cut = 0.601713
jetTagMonTool.IP3D_weight_cut            = 3.75

if not rec.doInDet:
    jetTagMonTool.UseTrackSelector = False
else:
    jetTagMonTool.UseTrackSelector = True

if DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
    print "jetTagMonTool will use TrigDecisionTool instance: %s" % DQMonFlags.nameTrigDecTool()
    jetTagMonTool.TrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
    jetTagMonTool.TriggerChain = "L1_J.*, L1_[3-5]J.* ,EF_e24vh_medium1, EF_mu18_IDTrkNoCut_tight, L1_RD0_FILLED"   #"L1_J10"   even older #"EF_mu4_L1J10_matched"
    jetTagMonTool.UseTrigDecisionTool = True
else:
    print "WARNING!!! jetTagMonTool will NOT use TrigDecisionTool."
    jetTagMonTool.UseTrigDecisionTool = False

