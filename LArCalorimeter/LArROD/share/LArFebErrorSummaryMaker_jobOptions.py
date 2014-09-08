from LArROD.LArRODConf import LArFebErrorSummaryMaker
febSummaryMaker =LArFebErrorSummaryMaker()
febSummaryMaker.MaskFebScacStatus = [0x38080000]
febSummaryMaker.MaskFebEvtId      = [0x38080000]
# needed only if it is not in DB.
#febSummaryMaker.MaskFebZeroSample = [0x39618000,0x39930000,0x3b1b0000,0x38db0000,0x38f60000,0x39ae8000,0x3bb90000]

# bad channel tools
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc+=theLArBadChannelTool

febSummaryMaker.BadChannelTool = theLArBadChannelTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence+=febSummaryMaker 
