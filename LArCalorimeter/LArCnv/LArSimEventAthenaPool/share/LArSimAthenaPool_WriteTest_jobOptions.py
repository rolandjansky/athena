###############################################################
#
# Job options file
#
#==============================================================
theApp.setup( MONTECARLO )

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
##include "$LARSIMEVENTROOT/dict/LArSimEvent_joboptions.txt"
##include "$IDENTIFIERROOT/dict/Identifier_joboptions.txt"
##include "$EVENTINFOROOT/dict/EventInfo_joboptions.txt"
# LAr Hit 
# #include "$TDRDETDESCRATHENAROOTROOT/share/TDRDetDescrAthenaRoot_jobOptions.txt"
# #include "$LARDETMGRDETDESCRCNVROOT/share/LArDetMgrDetDescrCnv_joboptions.txt"
# #include "$LARSIMROOT/share/LArSim_Fake_jobOptions.txt"
theApp.Dlls   += [ "LArSim" ]
theApp.TopAlg   += [ "LArFakeHitProducer" ]
theApp.Dlls   += [ "LArSimEventAthenaPoolPoolCnv" ]
Stream1 = Algorithm( "Stream1" )
Stream1.ItemList+=["LArHitContainer#*"]
Stream1.ItemList+=["EventInfo#*"]
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 2
Stream1.OutputFile="LArSimTest.root" 
#==============================================================
#
# End of job options file
#
###############################################################
