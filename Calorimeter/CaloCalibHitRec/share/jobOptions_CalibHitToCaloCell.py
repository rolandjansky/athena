###############################################################
#
# Job options file to put Calibration Hits into ntuple
#
# author: Sanya Solodkov	email:  sanya.solodkov@cern.ch
#
#==============================================================
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

include( "PartPropSvc/PartPropSvc.py" )

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOn()
DetFlags.em_setOn()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
#
DetFlags.simulate.Truth_setOn()

# select one of two setups:
DetDescrVersion = "Rome-Final"
#DetDescrVersion = "CTB"
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")

if ( DetDescrVersion=="CTB"):
    # LAr DetDescr for H8 testbeam
    include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
    # select one of 3 TileCal setups
    #GeoModelSvc.TileVersionOverride="TileTB-3B-00"
    #GeoModelSvc.TileVersionOverride="TileTB-2B2EB-00"
    GeoModelSvc.TileVersionOverride="TileTB-3B3EB-00"
    # Intialize TileCal conditions
    include( "TileConditions/TileTBConditions_jobOptions.py" )
else:
    GeoModelSvc.TileVersionOverride="TileCal-01"
    # Initialize TileCal conditions
    include( "TileConditions/TileConditions_jobOptions.py" )


# Pool Converters
#
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

# extra dictionary for calibration hits
include( "CaloSimEvent/CaloSimEventDict_joboptions.py" )

# Pool input
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "athena.root" ]


writePool = True

# Calibration hits from input POOL file to the output
#
if writePool:
    #--------------------------------------------------------------
    #---   Write output POOL file
    #--------------------------------------------------------------
    theApp.OutStream     =["Stream1"]
    theApp.OutStreamType ="AthenaOutputStream"
    Stream1 = Algorithm( "Stream1" )
    Stream1.EvtConversionSvc     ="AthenaPoolCnvSvc"
    #--------------------------------------------------------------
    Stream1.OutputFile = "SimplePoolFile2.root"
    Stream1.ItemList=["EventInfo#*",                       # EventInfo
                      "LArRawChannelContainer#*" ,         # LAr Raw Channels
                      "TileRawChannelContainer#*" ,        # Tile Raw Channels
                      "CaloCalibrationHitContainer#*",     # calib hits
                      "TileHitVector#*",                   # Tile hits
                      "TileHitContainer#*",                # Tile hits
                      "LArHitContainer#*",                 # LAr hits
                      "McEventCollection#*",               # mc truth (hepmc)
		      "CaloCellContainer#*"		   # CaloCells
		]
    Stream1.ForceRead=TRUE  #force read of output data

    theApp.Dlls += [ "CaloTools" ]

# calibration hits --> CaloCells
theApp.Dlls += [ "CaloCalibHitRec" ]
theApp.TopAlg += ["CalibHitToCaloCell"]

CalibHitToCaloCell = Algorithm( "CalibHitToCaloCell" )
CalibHitToCaloCell.StoreUnknownCells = False

# auditors
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 5

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = 5

NovaCnvSvc = Service( "NovaCnvSvc" )
NovaCnvSvc.OutputLevel = 5


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10000 
#EventSelector.SkipEvents = 12
theApp.EvtSel = "EventSelector"

