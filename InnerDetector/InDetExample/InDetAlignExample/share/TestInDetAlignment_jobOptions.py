###############################################################
#
# Job options file to test silicon alignments
# It loops through all detector elements and prints out the
# shifts from their nominal position.
#
# usage:
#   athena InDetAlignExample/TestInDetAlignment_jobOptions.py
# or 
#   athena -c 'testCase = 1' InDetAlignExample/TestInDetAlignment_jobOptions.py
#
#==============================================================

# There are several cases
# Case 1. Simulation with generation on the fly
# Case 2. Simulation reading in generation pool file
# Case 3. Reconstruction
# Case 4. Test call backs without input pool file.

# In Case 1 and 2, alignments will be read in during initialization
# In Case 3 and 4, they are read in after initialization but before
#  the first event.

if not 'testCase'  in dir() :
    testCase = 4;

# To override the run number set this variable.
# Only has an effect in cases 1 and 4 and conditions read in during initialization phase in case 2.
# runNumber = 12345

#
#==============================================================
# Load Detector Description for Pixel and SCT.
#==============================================================
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags

# Select which detectors
DetFlags.detdescr.ID_setOn()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.detdescr.TRT_setOn()

globalflags.DataSource='geant4'
#globalflags.DataSource='data'

#DetDescrVersion = "DC2"
#DetDescrVersion = "Rome-Initial"
DetDescrVersion = "ATLAS-CSC-01-02-00"
#DetDescrVersion = "CTB"
globalflags.DetDescrVersion = DetDescrVersion

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion


# To use the same as was used in the simulation comment this out.
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-CSC-00-01-00")
#conddb.setGlobalTag("OFLCOND-CSC-00-00-00")

# Reading of alignment from COOL by defualt

# This is default anyway
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc.AutomaticGeometryVersion = False
#GeoModelSvc.AlignCallbacks = True

# To ignore tag differences
# GeoModelSvc.IgnoreTagDifference = True

# To read in Pool file without IOV.
#include ("ReadPool.py")

## If you want to override the alignment sets add similar lines to below.
## Look at InDetAlignExample/InDetAlignableOverride.py for more examples.
#from IOVDbSvc.CondDB import conddb
## Override tag
#conddb.addOverride('/Indet/Align','InDetAlign_CSC_02')
## Use sqlite file mycool.db
#conddb.blockFolder("/Indet/Align")
#conddb.addFolderWithTag("LOCAL","/Indet/Align","mytag",force=True)

if testCase == 1 :
    # for simulation like jobs
    import AthenaCommon.AtlasUnixGeneratorJob
    GeoModelSvc.AlignCallbacks = False
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.EventSelector.OverrideRunNumber = True
    svcMgr.EventSelector.RunNumber         = 1
    svcMgr.EventSelector.FirstLB           = 1
    svcMgr.EventSelector.InitialTimeStamp  = 0

if testCase == 2 or testCase == 3 :
    # For cases that have an input file. Its not used so any file will do. 
    PoolEvgenInput = ["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]
    import AthenaPoolCnvSvc.ReadAthenaPool
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.EventSelector.InputCollections = PoolEvgenInput

if testCase == 2 :
    # For simulation that read in generator pool file
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.EventSelector.OverrideRunNumber = True
    svcMgr.EventSelector.RunNumber         = 1
    svcMgr.EventSelector.FirstLB           = 1
    svcMgr.EventSelector.InitialTimeStamp  = 0
    GeoModelSvc.AlignCallbacks = False

# For case 4 nothing extra to do unless run number set.
if testCase == 4 :
    import AthenaCommon.AtlasUnixGeneratorJob
  

# Override run number.
if ('runNumber' in dir()):
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.EventSelector.RunNumber         = runNumber
    #svcMgr.EventSelector.FirstEvent        = 0
    #svcMgr.EventSelector.FirstLB           = 0


from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


if DetFlags.detdescr.pixel_on():
    from InDetDetDescrExample.InDetDetDescrExampleConf import TestSiAlignment
    job += TestSiAlignment("TestPixelAlignment")
    job.TestPixelAlignment.ManagerName="Pixel"
    job.TestPixelAlignment.LongPrintOut=False
    print job.TestPixelAlignment

if DetFlags.detdescr.SCT_on():
    from InDetDetDescrExample.InDetDetDescrExampleConf import TestSiAlignment
    job += TestSiAlignment("TestSCT_Alignment")
    job.TestSCT_Alignment.ManagerName="SCT"
    job.TestSCT_Alignment.LongPrintOut=False
    print job.TestSCT_Alignment

if DetFlags.detdescr.TRT_on():
    from InDetDetDescrExample.InDetDetDescrExampleConf import TestTRT_Alignment
    job += TestTRT_Alignment()
    #job.TestTRT_Alignment.LongPrintOut  = False
    #job.TestTRT_Alignment.TestAllStraws = False
    #job.TestTRT_Alignment.TestAllElements = False
    #job.TestTRT_Alignment.HardwiredShifts = False
    #job.TestTRT_Alignment.Precision = 6
    print job.TestTRT_Alignment


#--------------------------------------------------------------
# Algorithms Options
#--------------------------------------------------------------
#
# TestSiAlignment properties
#
# ManagerName:      Manager name.
# LongPrintOut:     If true prints out changes in the axes as well as center
# ErrorRotation:    Error used when checking if delta is identity (rotation part). Default is 1e-15
# ErrorTranslation: Error used when checking if delta is identity (translation part). Default is 1e-12
#
# TestTRT_Alignment properties
#
# LongPrintOut:     If true prints out changes in the straw axes as well as center
# TestAllElements:  Printout for every element. If false only corner straws for each module will be printed.
#                   If true and TestAllStraws is false only first and last straw is printed.
# TestAllStraws:    Printout for every straw. Ignored if TestAllElements is false
# HardwiredShifts:  Use some hardcoded shifts rather than IOV.
# Precision:        Precision used to printout numbers.
# ErrorRotation:    Error used when checking if delta is identity (rotation part). Default is 1e-15
# ErrorTranslation: Error used when checking if delta is identity (translation part). Default is 1e-12

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# To set global output level use
# athena -l DEBUG

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.IOVSvc.OutputLevel=DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1


#==============================================================
#
# End of job options file
#
###############################################################
