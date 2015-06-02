# read LArRawChannel from G4, write to BS

theApp.Dlls   += [ "LArROD", "LArRawUtils" ]
theApp.TopAlg += [ "ReadLArRaw" ]
ReadLArRaw = Algorithm( "ReadLArRaw" )
ReadLArRaw.LArRawChannelContainerName = "LArRawChannels" 
#ReadLArRaw.HistogramID = 128 
#ReadLArRaw.NtupleLocID = "/FILE1/LARG" 
# specify the LAr Converter and LArROD type and key for output. 
theApp.Dlls   += [ "LArCalibUtils" ]
theApp.Dlls   += [ "LArByteStream" ]
StreamBS = Algorithm( "StreamBS" )
StreamBS.ItemList +=["2721#*"]
StreamBS.ForceRead=True
