#### register the FastShowerInfoContainer to Stream1 and configure AF2Digitization
Stream1.ItemList += ['FastShowerInfoContainer#*']
Stream1.OutputLevel = 2

include("FastCaloSimHit/AF2Digitization.py")