#==============================================================
# Job options file for Geant4 Simulations
# AFP_LocReco 
#==============================================================
#import jobOptionValues
#jobOptionValues=jobOptionValues

#strInputRootCollection = 'AFP_digi_step_100ev_203900_test18_r3.pool.root'
strInputRootCollection = 'AFP_digi_new.pool.root'
strOutputRootFile      = 'AFP_locreco.root'

print strInputRootCollection
print strOutputRootFile

#--------------------------------------------------------------
# Load POOL support
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
ServiceMgr.EventSelector.InputCollections = [strInputRootCollection]

#--------------------------------------------------------------
# Event related parameters
#theApp.EvtMax = jobOptionValues.iEventCountLocRecCorr

theApp.EvtMax = 63


#--------------------------------------------------------------
# Application: AthenaPoolTest options
theApp.Dlls   += [ "AFP_TDLocReco" ]
theApp.TopAlg += [ "AFP_TDLocReco" ]

# AFP_LocReco properties
from AFP_LocReco.AFP_LocRecoConf import AFP_TDLocReco
# select between a real data or a simulation mode - Simulation = 0, RealData = 1
AFP_TDLocReco.DataType   = 0
# select algo for timing reco
AFP_TDLocReco.ListAlgoTD = ["TDZVertex"]
AFP_TDLocReco.AmpThresh = 10.


#--------------------------------------------------------------
# Application: AthenaPoolTest options
theApp.Dlls   += [ "AFP_SIDLocReco" ]
theApp.TopAlg += [ "AFP_SIDLocReco" ]

# AFP_LocReco properties
from AFP_LocReco.AFP_LocRecoConf import AFP_SIDLocReco
# select between a real data or a simulation mode - Simulation = 0, RealData = 1
AFP_SIDLocReco.DataType   = 0
# select algo for timing reco
AFP_SIDLocReco.ListAlgoSID = ["SIDBasicKalman"]
AFP_SIDLocReco.AmpThresh = 5000.

#--------------------------------------------------------------
# Output options
# LAr Hit 
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
#include( "AFP_EventAthenaPool/AFP_EventAthenaPool_joboptions.py" )

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
#include( "AFP_EventAthenaPool/AFP_EventAthenaPoolItemList_joboptions.py" )


print fullItemList

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = strOutputRootFile

# List of DO's to write out - we don't use fullItemList, just list a
# few we want
#Stream1.ItemList   += fullItemList
Stream1.ItemList+= ["EventInfo#*", "McEventCollection#TruthEvent"] # mc truth (hepmc)
#Stream1.ItemList+= ["AFP_TDDigiCollection#*"]
Stream1.ItemList+= ["AFP_TDLocRecoEvCollection#*"]
#Stream1.ItemList+= ["AFP_SiDigiCollection#*"]
Stream1.ItemList+= ["AFP_SIDLocRecoEvCollection#*"]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.defaultLimit = 9999999
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
AthenaSealSvc.OutputLevel = 3

#--- End *.py file  -------------------------------------------
