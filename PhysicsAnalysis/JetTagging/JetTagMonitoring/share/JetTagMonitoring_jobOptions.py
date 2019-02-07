
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

#Properties defined in the jobOptions
#START
if (rec.doHeavyIon() or rec.doHIP()): # choose the jet collection
   jetTagMonTool.JetContainer = "AntiKt4HIJets" #Enable Heavy Ion jet collection
   jetTagMonTool.SkipJetQuality = True #Skip Jet Quality selection
else: 
   jetTagMonTool.JetContainer = "AntiKt4EMTopoJets" #nominal jet collection
   jetTagMonTool.SkipJetQuality = False #Do not Skip Jet Quality selection

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

# Choose between MV2c10, MV2c10mu, MV2c10rnn, DL1, DL1mu, DL1rnn
jetTagMonTool.MV_algorithmName = "MV2c10"
jetTagMonTool.MV_nBins = 100
jetTagMonTool.MV_rangeStart = -1.
jetTagMonTool.MV_rangeStop = 1.
jetTagMonTool.MV_cFraction = 0.08

# r21 Benchmarks (https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarksRelease21) 3 March 2018
if (jetTagMonTool.MV_algorithmName == "MV2c10") :
     jetTagMonTool.MV_60_cut = 0.94
     jetTagMonTool.MV_70_cut = 0.83
     jetTagMonTool.MV_77_cut = 0.64
     jetTagMonTool.MV_85_cut = 0.11
if (jetTagMonTool.MV_algorithmName == "MV2c10mu") :
     jetTagMonTool.MV_60_cut = 0.95
     jetTagMonTool.MV_70_cut = 0.87
     jetTagMonTool.MV_77_cut = 0.71
     jetTagMonTool.MV_85_cut = 0.23
if (jetTagMonTool.MV_algorithmName == "MV2c10rnn") :
     jetTagMonTool.MV_60_cut = 0.96
     jetTagMonTool.MV_70_cut = 0.87
     jetTagMonTool.MV_77_cut = 0.71
     jetTagMonTool.MV_85_cut = 0.26
if (jetTagMonTool.MV_algorithmName == "MV2c10r") : #Please update, when WP are available
     jetTagMonTool.MV_60_cut = 0.95
     jetTagMonTool.MV_70_cut = 0.87
     jetTagMonTool.MV_77_cut = 0.71
     jetTagMonTool.MV_85_cut = 0.23
if (jetTagMonTool.MV_algorithmName == "MV2c10rmu") : #Please update, when WP are available
     jetTagMonTool.MV_60_cut = 0.96
     jetTagMonTool.MV_70_cut = 0.87
     jetTagMonTool.MV_77_cut = 0.71
     jetTagMonTool.MV_85_cut = 0.26
if (jetTagMonTool.MV_algorithmName == "DL1") :
     jetTagMonTool.MV_60_cut = 2.74
     jetTagMonTool.MV_70_cut = 2.02
     jetTagMonTool.MV_77_cut = 1.45
     jetTagMonTool.MV_85_cut = 0.46
     jetTagMonTool.MV_nBins = 150
     jetTagMonTool.MV_rangeStart = -5.
     jetTagMonTool.MV_rangeStop = 10.
     jetTagMonTool.MV_cFraction = 0.08
if (jetTagMonTool.MV_algorithmName == "DL1mu") :
     jetTagMonTool.MV_60_cut = 2.72
     jetTagMonTool.MV_70_cut = 1.83
     jetTagMonTool.MV_77_cut = 1.10
     jetTagMonTool.MV_85_cut = 0.12
     jetTagMonTool.MV_nBins = 150
     jetTagMonTool.MV_rangeStart = -5.
     jetTagMonTool.MV_rangeStop = 10.
     jetTagMonTool.MV_cFraction = 0.08
if (jetTagMonTool.MV_algorithmName == "DL1rnn") :
     jetTagMonTool.MV_60_cut = 4.31
     jetTagMonTool.MV_70_cut = 2.98
     jetTagMonTool.MV_77_cut = 2.23
     jetTagMonTool.MV_85_cut = 1.32
     jetTagMonTool.MV_nBins = 150
     jetTagMonTool.MV_rangeStart = -5.
     jetTagMonTool.MV_rangeStop = 10.
     jetTagMonTool.MV_cFraction = 0.03
if (jetTagMonTool.MV_algorithmName == "DL1r") : #Please update, when WP are available
     jetTagMonTool.MV_60_cut = 2.72
     jetTagMonTool.MV_70_cut = 1.83
     jetTagMonTool.MV_77_cut = 1.10
     jetTagMonTool.MV_85_cut = 0.12
     jetTagMonTool.MV_nBins = 150
     jetTagMonTool.MV_rangeStart = -5.
     jetTagMonTool.MV_rangeStop = 10.
     jetTagMonTool.MV_cFraction = 0.08
if (jetTagMonTool.MV_algorithmName == "DL1rmu") : #Please update, when WP are available
     jetTagMonTool.MV_60_cut = 4.31
     jetTagMonTool.MV_70_cut = 2.98
     jetTagMonTool.MV_77_cut = 2.23
     jetTagMonTool.MV_85_cut = 1.32
     jetTagMonTool.MV_nBins = 150
     jetTagMonTool.MV_rangeStart = -5.
     jetTagMonTool.MV_rangeStop = 10.
     jetTagMonTool.MV_cFraction = 0.03

jetTagMonTool.SV1IP3D_weight_cut = 0.0
#END

if not rec.doInDet:
     jetTagMonTool.UseTrackSelector = False
else:
     jetTagMonTool.UseTrackSelector = True

# Setting up the trigger decision tool for trigger-aware monitoring
jetTagMonTool.UseTrigDecisionTool = True
if DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
     print "jetTagMonTool will use TrigDecisionTool instance: %s" % DQMonFlags.nameTrigDecTool()
     jetTagMonTool.UseTrigDecisionTool = True
else:
     print "WARNING!!! jetTagMonTool will NOT use TrigDecisionTool."
     jetTagMonTool.UseTrigDecisionTool = False

if (rec.triggerStream()=='express'): # don't require trigger if running on express stream
     jetTagMonTool.UseTrigDecisionTool = False

if (rec.doHeavyIon() or rec.doHIP()): # Disable triggers for Ion-Ion and Ion-proton runs
     jetTagMonTool.UseTrigDecisionTool = False

jetTagMonTool.ElectronTrigger_201X = "HLT_e[2-9][0-9]_.*"; # electrons 20-99 GeV
jetTagMonTool.MuonTrigger_201X = "HLT_mu[2-9][0-9].*"; # muons 20-99 Ge
#jetTagMonTool.MuonTrigger_201X = "HLT_mu.*"; # muons *all* GeV
#jetTagMonTool.JetTrigger_201X = "HLT_j15"; # jets (disabled)
