""" Configuration for LumiBlockMuWriter to write mu from LumiDB into EventInfo
"""
#
#
__author__  = 'E.Torrrence'
#
# 
# Configure folder access

from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
lumiTool = LuminosityToolDefault()  # Also configures DB
# lumiTool.OutputLevel = DEBUG
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += lumiTool

# Configure MuWriter algorithm
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuWriter
muWriter = LumiBlockMuWriter("LumiBlockMuWriter")
muWriter.LuminosityTool = lumiTool;
#muWriter.PileupLuminosityTag = "OflLumi-UPD2-005"

# muWriter.OutputLevel = DEBUG

# Add this to the top-level sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += muWriter

