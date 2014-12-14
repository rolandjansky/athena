# an example of how to run the MuonEfficiencyCorrections tool in athena

# Set up the file reading:
FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5493.pool.root"
#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.devval_rel_5.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

algseq = CfgMgr.AthSequencer("AthAlgSeq")

# Add the MCP tool

#THIS IS THE EXAMPLE:
ToolSvc += CfgMgr.CP__MuonEfficiencyScaleFactors("MCPEffTool",WorkingPoint="CBandST",DataPeriod="2012")
algseq += CfgMgr.CP__MuonEfficiencyCorrectionsProvider(Input="Muons",Tool=ToolSvc.MCPEffTool,OutputLevel=VERBOSE)
#END OF EXAMPLE

theApp.EvtMax = 5

