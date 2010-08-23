theApp.Dlls += [ "MuonByteStream" ]
StreamBS = Algorithm( "StreamBS" )

StreamBS.ItemList +=["4187#*"]
StreamBS.ItemList +=["4190#*"]
StreamBS.ItemList +=["4186#*"]
StreamBS.ItemList +=["4183#*"]
StreamBS.ForceRead=True

