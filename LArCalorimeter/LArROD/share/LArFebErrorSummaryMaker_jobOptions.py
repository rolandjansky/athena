from LArROD.LArRODConf import LArFebErrorSummaryMaker
febSummaryMaker =LArFebErrorSummaryMaker()
from RecExConfig.RecFlags import rec
if (not rec.projectName()[4:6].isdigit()) or int(rec.projectName()[4:6]) > 20:
   febSummaryMaker.MaskFebScacStatus = [0x38680000, 0x38720000]
   febSummaryMaker.MaskFebEvtId      = [0x38680000]
else:
   febSummaryMaker.MaskFebScacStatus = [0x38080000]
   febSummaryMaker.MaskFebEvtId      = [0x38080000]
febSummaryMaker.WriteKey="StoreGateSvc+LArFebErrorSummary"

# needed only if it is not in DB.
#febSummaryMaker.MaskFebZeroSample = [0x39618000,0x39930000,0x3b1b0000,0x38db0000,0x38f60000,0x39ae8000,0x3bb90000]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence+=febSummaryMaker 
