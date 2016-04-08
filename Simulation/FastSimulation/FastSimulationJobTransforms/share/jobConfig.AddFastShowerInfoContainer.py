#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#Stream1 = AthenaPoolOutputStream( "Stream1" ) WAAAHHHH! Do not do this, otherwise it won't work !!!!!
try:
    Stream1.ItemList += ['FastShowerInfoContainer#FastShowerInfoContainer']
    Stream1.OutputLevel = 2
except:
    print "Could not set Stream1.ItemList += FastShowerInfoContainer"
##########
try:
    StreamAOD.ItemList += ['FastShowerInfoContainer#FastShowerInfoContainer']
    StreamAOD.OutputLevel = 2
except:
    print "Could not set StreamAOD.ItemList += FastShowerInfoContainer"    
##########
try:
    StreamESD.ItemList += ['FastShowerInfoContainer#FastShowerInfoContainer']
    StreamESD.OutputLevel = 2
except:
    print "Could not set StreamAOD.ItemList += FastShowerInfoContainer"      
