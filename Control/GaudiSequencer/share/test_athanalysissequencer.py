import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/project/PAT/xAODs/r5534/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5534/AOD.01482225._000107.pool.root.1" ]

# ====================================================================
# Define your output root file using MultipleStreamManager
# ====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
rootStreamName = "MyFirstHistoStream"
rootFileName   = "myHistosAth.root"
MyFirstHistoXAODStream = MSMgr.NewRootStream( rootStreamName, rootFileName )
rootDirName = "/Hists"

# Now, import the new histogram manager and histogram tool
from HistogramUtils.HistogramManager import HistogramManager as HistMgr
from HistogramUtils.HistogramTool import HistogramTool as HistTool

histMgr = HistMgr( "MyHistMgr",
                   RootStreamName = rootStreamName,
                   RootDirName    = rootDirName,
                   )


# Import the 1-d and 2-d histograms from ROOT
from ROOT import TH1F, TH2F
# Adding a few histograms to the histogram manager
# Note the different methods of doing this (they are all equivalent)
histMgr += ( TH1F("ept", "p_{t}^{e}", 50, 0.0, 100.0), "ElectronCollection.pt / GeV ")
histMgr.add( TH1F("mupt", "p_{t}^{#mu}", 50, 0.0, 100.0), "Muons.pt / GeV ")
histMgr += HistTool( TH2F("muptvselpt", "p_{t}^{#mu} vs. p_{t}^{e}", 50, 0.0, 100.0, 50, 0.0, 100.0),
                     "Muons.pt / GeV", "ElectronCollection.pt / GeV" , OnlyAtCutStages = ["Cut2"] )

# ====================================================================
# Create a subsequence:
# Remember that a subsequece stops its execution for a given event
# after an algorithm that declares that that event doesn't pass
# a certain selection.
# ====================================================================
algseq = CfgMgr.AthSequencer("AthAlgSeq")
subSeq = CfgMgr.AthAnalysisSequencer("MySubSeq", HistToolList = histMgr.ToolList() , OutputLevel=DEBUG)
#subSeq = CfgMgr.AthRetrySequencer("MySubSeq")
#subSeq = CfgMgr.AthSequencer("MySubSeq")
algseq += subSeq

# Add an algorithm to your sub-sequence that fills all the histograms that
# are managed by the histogram manager
# subSeq += CfgMgr.HistAlg("HistAlgCut0", HistToolList = histMgr.ToolList() )

# Make a cut
subSeq += CfgMgr.CutAlg("Cut1", Cut = "count(Muons.pt > 10*GeV) >= 2" )

# Add more histograms after the Cut1
# subSeq += CfgMgr.HistAlg("HistAlgCut1", HistToolList = histMgr.ToolList() )

# Make another cut
subSeq += CfgMgr.CutAlg("Cut2", Cut = "count(ElectronCollection.pt > 10*GeV) >= 2" )

# Add more histograms after the Cut2
# subSeq += CfgMgr.HistAlg("HistAlgCut2", HistToolList = histMgr.ToolList() )
