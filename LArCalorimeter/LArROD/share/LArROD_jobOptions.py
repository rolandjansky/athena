#################################################################
#
# This job option fragment is to create LArRaw Channels
#  from LAr Digits
# 
#################################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

# Turn off printing for LArRoI_Map
from AthenaCommon.AppMgr import ToolSvc
from LArRawUtils.LArRawUtilsConf import LArRoI_Map
LArRoI_Map = LArRoI_Map("LArRoI_Map")
ToolSvc += LArRoI_Map
LArRoI_Map.Print=FALSE; 

from LArROD.LArRODConf import LArRawChannelBuilder
LArRawChannelBuilder = LArRawChannelBuilder( "LArRawChannelBuilder" )
topSequence += LArRawChannelBuilder

LArRawChannelBuilder.DataLocation = "LArDigitContainer_MC" 
LArRawChannelBuilder.LArRawChannelContainerName = "LArRawChannels" 
LArRawChannelBuilder.UseOFCTool=TRUE

from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theADC2MeVTool = LArADC2MeVToolDefault()
ToolSvc += theADC2MeVTool
LArRawChannelBuilder.ADC2MeVTool = theADC2MeVTool

from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
theOFCTool = LArOFCToolDefault()
ToolSvc += theOFCTool
LArRawChannelBuilder.OFCTool = theOFCTool
