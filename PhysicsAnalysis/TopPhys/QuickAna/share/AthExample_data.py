import AthenaPoolCnvSvc.ReadAthenaPool

theApp.EvtMax = 5
testFile = os.getenv("ASG_TEST_FILE_DATA")
svcMgr.EventSelector.InputCollections = [testFile]

algSeq = CfgMgr.AthSequencer("AthAlgSeq")

quickAna = CfgMgr.ana__QuickAna()
quickAna.eventinfoDef = "default"
quickAna.electronDef = "default"
quickAna.muonDef = "default"
quickAna.tauDef = "default"
quickAna.photonDef = "default"
quickAna.jetDef = "default"
quickAna.metDef = "default"
quickAna.orDef = "default"

# We just need some files for testing. Do not use these in your analysis
#quickAna.muMcFiles = ["dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
#                      "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"]
## Using test file from SUSYTools
#quickAna.muDataFiles = ["dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"]

ToolSvc += quickAna

alg = CfgMgr.ana__AthExample()
alg.quickAna = quickAna
algSeq += alg

# Some log suppression
include("AthAnalysisBaseComps/SuppressLogging.py")
