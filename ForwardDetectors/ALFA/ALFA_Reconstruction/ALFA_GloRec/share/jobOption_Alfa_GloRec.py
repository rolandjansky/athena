#==============================================================
# Job options file for Geant4 Simulations
# ALFA_GloRec 
#==============================================================
#import jobOptionValues
#jobOptionValues=jobOptionValues

strInputRootCollection = 'rootColl_locRecCorr_04_PhysicsOn.root'
strOutputRootFile      = 'rootColl_gloRec_04_PhysicsOn.root'

print strInputRootCollection
print strOutputRootFile

#--------------------------------------------------------------
# Load POOL support
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
ServiceMgr.EventSelector.InputCollections = [strInputRootCollection]

#--------------------------------------------------------------
# Event related parameters
#theApp.EvtMax = jobOptionValues.iEventCountLocRecCorr

#--------------------------------------------------------------
# Application: AthenaPoolTest options
theApp.Dlls   += [ "ALFA_GloRec" ]
theApp.TopAlg += [ "ALFA_GloRec" ]

#--------------------------------------------------------------
# ALFA_GloRec properties
from ALFA_GloRec.ALFA_GloRecConf import ALFA_GloRec

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
ALFA_GloRec.DataType                   = 0
ALFA_GloRec.TrackPathPatterns          = 'B7L1U_A7L1U_X_X,B7L1L_A7L1L_X_X,X_X_B7R1U_A7R1U,X_X_B7R1L_A7R1L'
ALFA_GloRec.AnalysisFile               = 'GloRecAnalysis.root'
ALFA_GloRec.OutputGloRecCollectionName = 'ALFA_GloRecEvCollection'
ALFA_GloRec.InputLocRecCollectionName  = 'ALFA_LocRecCorrEvCollection'
ALFA_GloRec.TruthEventCollectionName   = 'TruthEvent'   

#--------------------------------------------------------------
# Output options
# LAr Hit 
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "ALFA_EventAthenaPool/ALFA_EventAthenaPool_joboptions.py" )

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
include( "ALFA_EventAthenaPool/ALFA_EventAthenaPoolItemList_joboptions.py" )

print fullItemList

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = strOutputRootFile

# List of DO's to write out - we don't use fullItemList, just list a
# few we want
#Stream1.ItemList   += fullItemList
Stream1.ItemList+= ["EventInfo#*", "McEventCollection#TruthEvent"] # mc truth (hepmc)
Stream1.ItemList+= ["ALFA_HitCollection#*"]
Stream1.ItemList+= ["ALFA_ODHitCollection#*"]
Stream1.ItemList+= ["ALFA_DigitCollection#*"]
Stream1.ItemList+= ["ALFA_ODDigitCollection#*"]
Stream1.ItemList+= ["ALFA_LocRecEvCollection#*"]
Stream1.ItemList+= ["ALFA_LocRecODEvCollection#*"]
Stream1.ItemList+= ["ALFA_LocRecCorrEvCollection#*"]
Stream1.ItemList+= ["ALFA_LocRecCorrODEvCollection#*"]
Stream1.ItemList+= ["ALFA_GloRecEvCollection#*"]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 6
MessageSvc.defaultLimit = 9999999
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 4
AthenaSealSvc.OutputLevel = 4

#--- End *.py file  -------------------------------------------
