## @file  CaloDigitization.py
#  @brief include file to configure calorimeter digitization
from AthenaCommon.Resilience import protectedInclude

from AthenaCommon.DetFlags import DetFlags
from Digitization.DigitizationFlags import digitizationFlags

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
from AthenaCommon.AppMgr import ServiceMgr

##################################################################
# LArCalorimeter
##################################################################
if DetFlags.LAr_on():
    if DetFlags.digitize.LAr_on():
        job += CfgGetter.getAlgorithm("LArRawChannelBuilder", tryDefaultConfigurable=True)
        from LArROD.LArDigits import DefaultLArDigitThinner
        LArDigitThinner = DefaultLArDigitThinner('LArDigitThinner') # automatically added to topSequence
        if digitizationFlags.doDigiTruth():
          from LArROD.LArRawChannelGetter_DigiHSTruth import LArRawChannelGetter_DigiHSTruth
          LArRawChannelGetter_DigiHSTruth()
##################################################################
# TileCalorimeter
##################################################################
if DetFlags.Tile_on():
    if DetFlags.digitize.Tile_on():
        protectedInclude( "TileSimAlgs/TileDigiCommon_jobOptions.py" )
        protectedInclude ( "TileL2Algs/TileL2Algs_jobOptions.py" )
##################################################################
