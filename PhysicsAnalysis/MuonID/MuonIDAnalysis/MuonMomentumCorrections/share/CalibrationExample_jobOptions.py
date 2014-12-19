# Set up the file reading:
FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

algseq = CfgMgr.AthSequencer("AthAlgSeq")

# Add the MCP tool

#THIS IS THE EXAMPLE:
ToolSvc += CfgMgr.CP__MuonCalibrationAndSmearingTool("MCPTool")
algseq += CfgMgr.CP__CalibratedMuonsProvider(Input="Muons",Output="CalibratedMuons",Tool=ToolSvc.MCPTool,OutputLevel=VERBOSE)
#END OF EXAMPLE

theApp.EvtMax = 5

