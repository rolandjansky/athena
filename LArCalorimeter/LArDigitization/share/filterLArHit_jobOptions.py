###############################################################
#
# Job options file to read hit file and produce output hit file
#  with filtered LArHits (only hits above some energy are kept)
#
#==============================================================

PoolOutputFileName = "filteredMB.pool.root"
PoolInputCollection = [
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00111.pool.root",
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00135.pool.root",
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00153.pool.root",
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00240.pool.root",
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00267.pool.root",
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00322.pool.root",
"mc08.105001.pythia_minbias.simul.HITS.e349_s462_tid024528/HITS.024528._00357.pool.root",
]
Geometry = "ATLAS-GEO-02-01-00"

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_geant4() 
DetFlags.detdescr.all_setOn()

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "PartPropSvc/PartPropSvc.py" )
#
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#
# Pool output
#
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "PoolOutputFileName
Stream1.ItemList=["EventInfo#*",      
                  "McEventCollection#TruthEvent"] # mc truth (hepmc)
Stream1.ItemList+=["LArHitContainer#*"]
Stream1.ForceRead = True

#
# Pool input
#
svcMgr.EventSelector.InputCollections = PoolInputCollection

#
# the Tile, LAr and Calo detector description package
#
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )

#
# LarHit filter algorithm
#
from LArDigitization.LArDigitizationConf import LArHitFilter
LArHitFilter = LArHitFilter("LArHitFilter")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += LArHitFilter


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
LArHitFilter.OutputLevel        =  INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = -1
theApp.EvtSel = "EventSelector"
