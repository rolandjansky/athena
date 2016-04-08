# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#==============================================================
#
# Job options file for GEANT4 Simulation, using 
# Geo2G4 for the InnerDetector and the MuonSystem
#
# - reads Generator objects from the AthenaPool file
# - converts them to transient objects
# - performs GEANT4 simulation
# - writes output (SimHits) via AthenaPool
#==============================================================

#--------------------------------------------------------------
# Load POOL support 
#--------------------------------------------------------------
include ( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Options for reading from POOL (use POOL implicit collection)
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = PoolEvgenInput

#--------------------------------------------------------------
# Options for writing to POOL  
#--------------------------------------------------------------
include ( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

theApp.OutStream     = [ "Stream1" ] 
theApp.OutStreamType = "AthenaOutputStream" 

Stream1 = Algorithm ( "Stream1" )
Stream1.EvtConversionSvc = "AthenaPoolCnvSvc" 

Stream1.OutputFile = PoolHitsOutput

#--------------------------------------------------------------
# Define output stream items  
#--------------------------------------------------------------
# Full list
Stream1.ItemList += [ "EventInfo#*",                  
                      "McEventCollection#*",
                      "TrackRecordCollection#*",
                      "SiHitCollection#*",
                      "TRTUncompressedHitCollection#*",
                      "TileHitVector#*",
                      "LArHitContainer#*",
                      "CaloCalibrationHitContainer#*",
                      "RPCSimHitCollection#*",
                      "TGCSimHitCollection#*",
                      "CSCSimHitCollection#*",
                      "MDTSimHitCollection#*" ]

# hit AthenaPool converters
include ( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include ( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )
include ( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
include ( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include ( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
include ( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

theApp.Dlls += [ "GeneratorObjectsAthenaPoolPoolCnv" ]

#--------------------------------------------------------------
# Setting up and running GEANT4
#--------------------------------------------------------------
theApp.setup( MONTECARLO )

include ( "NovaCnvSvc/NovaCnvSvc_jobOptions.py" )
include ( "NovaConverters/NovaConverters_jobOptions.py" )
include ( "PartPropSvc/PartPropSvc.py" )
include( "G4Svc/jobOptions.G4Svc.py" )

# GeoModel stuff
#--------------------------------------------------------------
theApp.DLLs   += [ "Geo2G4" ]
theApp.ExtSvc += [ "Geo2G4Svc" ]

# GeoModel stuff
#--------------------------------------------------------------
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")
GeoModelSvc.MuonDetectorTool.IncludeInertMaterials = 1

theApp.Dlls   += [ "G4SimAlg" ]
theApp.TopAlg += [ "G4SimAlg" ]

#--------------------------------------------------------------
# G4Svc options
#--------------------------------------------------------------
G4Svc = Service ( "G4Svc" )
G4Svc.PhysicsList       = "none"
G4Svc.SaveHits          =  FALSE
G4Svc.Visualize         =  FALSE
G4Svc.RunVerbosity      =      0
G4Svc.EventVerbosity    =      0
G4Svc.TrackingVerbosity =      0
G4Svc.Listener          =  FALSE
G4Svc.InteractiveG4     =  FALSE
G4Svc.FADSMacro         = "AtlasGeoModel.mac:EtaPhiFilter_settings.mac:PhysicsList.mac:TimingActions.mac:TrackRecord.mac:MCTruth.mac"

# G4SimAlg options
#--------------------------------------------------------------
G4SimAlg = Algorithm ( "G4SimAlg" )
G4SimAlg.Dll = "G4PhysicsLists:G4UserActions"

#--------------------------------------------------------------
# Event-related parameters
#--------------------------------------------------------------
# Number of output events to be processed
theApp.EvtMax = EvtMax

# Number of input events to be skipped
EventSelector.SkipEvents = SkipEvents


#--------------------------------------------------------------
# OUTPUT PRINTOUT LEVEL
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
# you can override this for individual modules if necessary
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING

Stream1.OutputLevel = 5
theApp.OutputLevel  = 5

AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.OutputLevel = 5

#==============================================================
