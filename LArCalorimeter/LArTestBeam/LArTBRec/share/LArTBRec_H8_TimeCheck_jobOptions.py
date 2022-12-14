#*******************************************************************
#
# JobOption to analyse LAr testbeam data :  LArDigit to  LArRawChannel
#
#
#*******************************************************************

#if not doSim:

theApp.Dlls += [ "LArByteStream" ]
# read LArDigit
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
ToolSvc = Service( "ToolSvc" )

# read TDC
#abc ByteStreamAddressProviderSvc.TypeNames += ["TBTDC/LArTBTDC"] 
#abc ToolSvc.TBByteStreamCnvTool.SubDetID = 129

theApp.Dlls += ["LArRawUtils", "LArROD", "LArTools" , "LArEventTest","LArCalibUtils" ]

#abc LArDigit->LArRawChannel part 1 : Digit pre-processor 

#abc theApp.TopAlg += [
#abc     "LArDigitPreProcessor<LArDigitContainer>/LArDigitPreProcessor"]
#abc LArDigitPreprocessor = Algorithm( "LArDigitPreprocessor" )
#abc LArDigitPreprocessor.NumberOfSamples = 5
#abc LArDigitPreprocessor.FirstSample = 1
#abc LArDigitPreprocessor.InputContainers += [ "FREE" ]
#abc LArDigitPreprocessor.OutputContainer = "LArDigit"

#abc  LArDigit->LArRawChannel part 2 : raw channel builder
#----- Check LAr LVL1 and BCID consistency
theApp.TopAlg += [ "CheckLArFebHeader" ]
include("LArConditionsCommon/LArConditionsCommon_H8_jobOptions.py") 
#abc  IOVDbSvc.GlobalTag = "TB04-4"

theApp.TopAlg += [ "LArTimeChecker" ]
LArTimeChecker = Algorithm( "LArTimeChecker" )
LArTimeChecker.DataLocation = "FREE"

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

theApp.TopAlg += [ "LArRawChannelBuilder" ]
LArRawChannelBuilder = Algorithm("LArRawChannelBuilder");
LArRawChannelBuilder.DataLocation = "FREE" 
LArRawChannelBuilder.UseTDC=True


ToolSvc.LArRodDecoder.FirstSample=2

# write out a list of all Storegate collection with their keys and
# lock/unlock state. Very useful for debugging purpose
#
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE
#DetectorStore = Service( "DetectorStore" )
#DetectorStore.Dump = TRUE
#ConditionStore = Service( "ConditionStore" )
#ConditionStore.Dump = TRUE


