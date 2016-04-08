#======================================================================
# 
#======================================================================

#--------------------------------------------------------------
# Some links
#--------------------------------------------------------------

#os.putenv("POOL_CATALOG","xmlcatalog_file:/afs/cern.ch/user/t/tcorneli/public/PoolFileCatalog.xml")
os.putenv("atlasdata","/afs/cern.ch/atlas/offline/data")

#--------------------------------------------------------------
# Internal flags for Inner Detector
#--------------------------------------------------------------

# --- setup flags with default values
include( "InDetTBRecExample/InDetTBFlags_jobOptions.py" )

# --- The selection of RDOs only works for real data for the moment
if InDetTBFlags.doSim:
    InDetTBFlags.SelectRDO = False;
    
# if doTrackSegments = True put all the trackers to false since I will
# decide which tracker to use in each case
if InDetTBFlags.doTrackSegments:
    InDetTBFlags.TRT_LUT=False
    InDetTBFlags.xKalman=False
    InDetTBFlags.iPatRec=False
    InDetTBFlags.CTBTracking=False
    InDetTBFlags.IDSCAN=False
    if not InDetTBFlags.CTBTracking:
        InDetTBFlags.Refit = False

# Set magnet, Current and OracleTag flags
if not InDetTBFlags.doSim:
    include("InDetTBRecExample/InDetTBSetFlags_jobOptions.py")

# ------------------------------------
# import GlobalFlags from RecExCommon
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags


GlobalFlags.DetGeo.set_ctbh8()

if InDetTBFlags.doSim:
    # Reading G4 simulated data from Pool
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()
else:
    # Reading ByteStream real data
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream() 

# ----------------------------------
# import DetFlags from AthenaCommon
# ----------------------------------
from AthenaCommon.DetFlags import DetFlags


DetFlags.detdescr.ID_setOn()
if InDetTBFlags.doSim:
    DetFlags.readRDOPool.ID_setOn()
else:
    DetFlags.readRDOBS.ID_setOn()
DetFlags.makeRIO.ID_setOn()        

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
if InDetTBFlags.doSim:
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    # converters
    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    #include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
    # Read Pool data
    EventSelector = Service( "EventSelector" )
    if InDetTBFlags.Magnet:
        #EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/k/koffas/ctb_MyOutputFile-Dig_pi850A04.root"]
        EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/i/idctb/simulation/digits/digits_run50002365_particle-211.root_new" ]
    else:
        # EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/k/koffas/ctb_MyOutputFile-Dig_pi0A04.root"]
        EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/i/idctb/simulation/digits/digits_run50002355_particle-211.root_new"] # pions new gas mix
if not InDetTBFlags.doSim:
    include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

    # Specify input file
    ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

    # Directory 
    #ByteStreamInputSvc.InputDirectory = ["/castor/cern.ch/user/c/costamj/testbeam"] ;
    #ByteStreamInputSvc.InputDirectory = ["/afs/cern.ch/delphi/scratch/c/costamj"] ;
    ByteStreamInputSvc.InputDirectory = [ "/castor/cern.ch/atlas/testbeam/combined/2004" ]
    # Prefix 
    ByteStreamInputSvc.FilePrefix = [InDetTBFlags.FilePrefix]
    # Run number 
    ByteStreamInputSvc.RunNumber = [InDetTBFlags.RunNumber] 
    
    # Reader type "Castor" or "Simple" 
    ByteStreamEventStorageInputSvc = Service( "ByteStreamEventStorageInputSvc" )
    ByteStreamEventStorageInputSvc.ReaderType = "Castor";

# ------------------------------------------------------------
# Specify IOVDbSvc and access conditions database
#-------------------------------------------------------------
include( "InDetTBRecExample/InDetTBConDB_jobOptions.py" )
    
#--------------------------------------------------------------
# GeoModel Detector description
#--------------------------------------------------------------
DetDescrVersion = "CTB"
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")
#include ("InDetTBRecExample/InDetTB2004DetDescr_jobOptions.py")
include.block( "AtlasGeoModel/SetGeometryVersion.py" )
include.block( "AtlasGeoModel/GeoModelInit.py" )

#NovaCnvSvc.Host = "atlasdbdev.cern.ch"

#---------------------------------------------------------------
# BS converters
# --------------------------------------------------------------
if not InDetTBFlags.doSim:
    include( "InDetTBRecExample/InDetTBBSCnv_jobOptions.py" )

#--------------------------------------------------------------
# Read Simulated Hits
#--------------------------------------------------------------
if InDetTBFlags.testHits:
    include( "InDetTBRecExample/InDetTBHits_jobOptions.py" )
#--------------------------------------------------------------
# Digitization
#--------------------------------------------------------------
if InDetTBFlags.doSim and InDetTBFlags.Digitization:
    include( "InDetTBRecExample/InDetTBDigitization_jobOptions.py" )
#--------------------------------------------------------------
# Read RDOs
#--------------------------------------------------------------
if InDetTBFlags.testRDO:
    include( "InDetTBRecExample/InDetTB_RDO_jobOptions.py" )    
#--------------------------------------------------------------
# reconstruction
#--------------------------------------------------------------
#include( "InDetTBRecExample/InDetTBRec_jobOptions.py" )
    
#--------------------------------------------------------------

include( "WriteCalibCTB.py" )
#include( "WriteAlignCTB.py" )
#--------------------------------------------------------------
# write out a summary of the time spent
#--------------------------------------------------------------
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
# -------------------------------------------------------------
#Output level
# -------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
# you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             = 3
# MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"
#StoreGateSvc.Dump=true;
# Number of events to be processed (default is 10)
theApp.EvtMax = 2
MessageSvc.defaultLimit=1000000;
#EventSelector.SkipEvents=10
theApp.AuditAlgorithms=True

