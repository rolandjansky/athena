# jobOptions for creating MC LArRawChannel, then write LAr ByteStream 
# works for G3
include( "LArSim/LArSim_MC_jobOptions.py" )

include( "LArROD/LArROD_MC_jobOptions.py" )

digitmaker1 = Algorithm( "digitmaker1" )
digitmaker1.NoiseOnOff = FALSE ; # switch noise off
digitmaker1.CrossTalk  = FALSE ; # switch crosstalk off
theApp.TopAlg += [ "ReadLArRaw" ]
ReadLArRaw = Algorithm( "ReadLArRaw" )
ReadLArRaw.LArRawChannelContainerName = "LArRawChannels" 
ReadLArRaw.HistogramID = 128 
ReadLArRaw.NtupleLocID = "/FILE1/LARG" 
# specify the LAr Converter and LArROD type and key for output. 
theApp.Dlls   += [ "LArCalibUtils" ]
theApp.Dlls   += [ "LArByteStream" ]
StreamBS = Algorithm( "StreamBS" )
StreamBS.ItemList +=["2721#*"]
