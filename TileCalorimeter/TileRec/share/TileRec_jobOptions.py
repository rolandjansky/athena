#----------------------------------
# Read TileCal data from ByteStream
#----------------------------------
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    ServiceMgr += ByteStreamAddressProviderSvc()
 
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += ["TileBeamElemContainer/TileBeamElemCnt",
                                                      "TileDigitsContainer/TileDigitsCnt",
                                                      "TileDigitsContainer/MuRcvDigitsCnt",
                                                      "TileL2Container/TileL2Cnt",
                                                      "TileLaserObject/TileLaserObj",
						      "TileMuonReceiverContainer/TileMuRcvCnt"]

from TileRecUtils.TileRawChannelGetter import *
theTileRawChannelGetter=TileRawChannelGetter()
