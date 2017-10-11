from ROOT import TH1F, TH2F
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
import AthenaCommon.SystemOfUnits as Units
from HistogramUtils.HistogramManager import HistogramManager as HistMgr
from HistogramUtils.HistogramTool import HistogramTool as HistTool

from RecExConfig.RecFlags import rec
rec.doDPD                 = False
rec.doHist                = False
rec.doWriteTAG            = False
rec.doPerfMon             = False
rec.doSemiDetailedPerfMon = False
rec.doDetailedPerfMon     = False
rec.doFileMetaData        = False

# Create the D3PD stream using MultipleStreamManager:
# the aruments are: stream name, file name, TTree name in file

myPath = "/afs/cern.ch/atlas/project/PAT/xAODs/r5534/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5534/AOD.01482225._000107.pool.root.1"
jp.AthenaCommonFlags.FilesInput = [myPath]

#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()

include ("RecExCommon/AnalysisCommon_topOptions.py")

streamName = "MyFirstHistoStream"
fileName   = "myHistosAth.pool.root"
rootStreamName = streamName
rootDirName = "/Hists"

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
MyFirstHistoXAODStream = MSMgr.NewRootStream( streamName, fileName )


if not hasattr( ServiceMgr, 'THistSvc' ): print 'Ahhhhhhhh'

ServiceMgr.THistSvc.OutputLevel = VERBOSE


# ====================================================================
# Create a subsequence:
# Only when the first algorithm returns isEventAccepted,
# the rest of this sub-sequence is executed
# ====================================================================
from AthenaCommon.AlgSequence import AthSequencer

subSeqMyAthAnalysis = AthSequencer("MyFirstAnalysisSubSeq")
topSequence += subSeqMyAthAnalysis

    
myHistMgr = HistMgr("HistMgrHtoWWtoemu",
					RootStreamName = rootStreamName,
					RootDirName    = rootDirName,
					OutputLevel    = VERBOSE
#					OnlyAtCutStages = ["CutAlg2", "CutAlg3"]
)

myHistMgr += ( TH1F("ept", "p_{t}^{e}", 50, 0.0, 100.0), "ElectronCollection.pt / GeV ")
myHistMgr.add( TH1F("mupt", "p_{t}^{#mu}", 50, 0.0, 100.0), "Muons.pt / GeV ")
myHistMgr.add( [ TH1F("ept2", "p_{t}^{e}", 50, 0.0, 100.0), "ElectronCollection.pt / GeV " ] )
myHistMgr += TH2F("muptvselpt", "p_{t}^{#mu} vs. p_{t}^{e}", 50, 0.0, 100.0, 50, 0.0, 100.0), "Muons.pt / GeV", "ElectronCollection.pt / GeV" 
myHistMgr += HistTool( TH2F("muptvselpt2", "p_{t}^{#mu} vs. p_{t}^{e}", 50, 0.0, 100.0, 50, 0.0, 100.0), "Muons.pt / GeV", "ElectronCollection.pt / GeV" )

print(myHistMgr)

#    
#    # example for custom hist tool
#    # from MyCode.MyCodeConf import MetHistTool
#    # myHistMgr +=  MetHistTool("MyMetHistTool")
#    
#    mySpecialHistToolInstance = HistTool( TH2F("muptvselpt2", "p_{t}^{#mu} vs. p_{t}^{e}", 50, 0.0, 1.0, 50, 0.0, 1.0), "muon.pt()", "electron.pt()",
#                                          RootStreamName  = rootStreamName,
#                                          RootDirName     = rootDirName,
#                                          OnlyAtCutStages = ["CutAlg2", "CutAlg3"]
#                                          )
#    
#    
#    mySpecialHistToolInstance2 = HistTool( TH2F("muptvselpt3", "p_{t}^{#mu} vs. p_{t}^{e}", 50, 0.0, 1.0, 50, 0.0, 1.0), "muon.pt()", "electron.pt()" )
#    mySpecialHistToolInstance2.RootStreamName  = rootStreamName
#    mySpecialHistToolInstance2.RootDirName     = rootDirName
#    mySpecialHistToolInstance2.OnlyAtCutStages = ["CutAlg2", "CutAlg3"]
#    
#    
#    # http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Control/GaudiSequencer/src/AthSequencer.h
#    #from AthenaCommon.AlgSequence import AthHistogramSequencer

    
#    #htoWWtoemuSeq += Alg1("Alg1")
#    
#    htoWWtoemuSeq += CutAlg("CutAlg2", "count(ElectronContainer.pt() > 20 GeV) >=1")
#    
#    htoWWtoemuSeq += CutAlg("CutAlg3", "count(muons.pt() > 20 GeV) >= 1")
#    



myHistToolInstance = HistTool( TH1F("my_mupt", "p_{t}^{#mu}", 50, 0.0, 100.0), "  Muons.pt * 0.001 ",
							   RootStreamName=rootStreamName,
							   RootDirName=rootDirName,
							   OutputLevel = VERBOSE
						   )

# subSeqMyAthAnalysis += CfgMgr.CutAlg( "MuonPtCut",
# 									  OutputLevel = VERBOSE,
#                                       Cut = "count(Muons.pt > 10*GeV) >= 1" )

subSeqMyAthAnalysis += CfgMgr.HistAlg("myHistAlg",
									  OutputLevel = VERBOSE,
									  RootStreamName        = streamName,
									  RootDirName           = "/MyHists",  
									  HistToolList = [ myHistToolInstance ] )

myHistMgr.printToolList()


htoWWtoemuSeq = CfgMgr.AthAnalysisSequencer( "HtoWWtoemuSeq", HistToolList = myHistMgr.ToolList() )
topSequence += htoWWtoemuSeq

#subSeqMyAthAnalysis += CfgMgr.HistAlg("myHistMgrAlg",
#									  HistToolList = myHistMgr.ToolList() )
