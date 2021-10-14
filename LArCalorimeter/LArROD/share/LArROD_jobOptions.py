#################################################################
#
# This job option fragment is to create LArRaw Channels
#  from LAr Digits
# 
#################################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from LArROD.LArRODConf import LArRawChannelBuilder
LArRawChannelBuilder = LArRawChannelBuilder( "LArRawChannelBuilder" )
topSequence += LArRawChannelBuilder

LArRawChannelBuilder.DataLocation = "LArDigitContainer_MC" 
LArRawChannelBuilder.LArRawChannelContainerName = "LArRawChannels" 

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()
from LArRecUtils.LArOFCCondAlgDefault import LArOFCCondAlgDefault
LArOFCCondAlgDefault()

from Digitization.DigitizationFlags import digitizationFlags
if digitizationFlags.doDigiTruth():
  print "Running LAr Raw Channel DigiTruth"
  LArRawChannelBuilder_DigiHSTruth = LArRawChannelBuilder( "LArRawChannelBuilder_DigiHSTruth" )
  topSequence += LArRawChannelBuilder_DigiHSTruth
  LArRawChannelBuilder_DigiHSTruth.DataLocation = "LArDigitContainer_DigiHSTruth"
  LArRawChannelBuilder_DigiHSTruth.LArRawChannelContainerName = "LArRawChannels_DigiHSTruth"
