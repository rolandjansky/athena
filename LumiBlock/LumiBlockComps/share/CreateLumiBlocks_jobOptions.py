#--------------------------------------------------------------
# job options fragment to bootstrap metadata
#--------------------------------------------------------------

#include ( "EventInfo/EventInfoDict_joboptions.py" )
include ( "EventInfo/EventInfo_joboptions.py" )
include ( "LumiBlockAthenaPool/LumiBlockAthenaPool_joboptions.py")

#To run the luminosity block from event info 'bootstrap' algo
theApp.Dlls += [ "LumiBlockComps" ]
theApp.TopAlg += [ "CreateLumiBlockCollectionFromFile" ]
CreateLumiBlockCollectionFromFile = Algorithm( "CreateLumiBlockCollectionFromFile");


