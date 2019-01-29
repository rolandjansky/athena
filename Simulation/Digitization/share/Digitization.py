###############################################################
#
# Job options file to run:
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Sven Vahsen and John Chapman
#==============================================================
from AthenaCommon.Logging import logging
logDigitization_flags = logging.getLogger( 'Digitization' )

#--------------------------------------------------------------
# Get Digitization Flags (This sets Global and Det Flags)
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.BeamFlags import jobproperties

#--------------------------------------------------------------
# Set Global flags for this run
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
if jobproperties.Beam.beamType == "cosmics" :
    globalflags.DetGeo = 'commis'
#else:
#    globalflags.DetGeo = 'atlas'
# These should be set anyway, but just to make sure
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'

# This is for loading conditions in AthenaMT
from IOVDbSvc.CondDB import conddb


#--------------------------------------------------------------
# Set Detector flags for this run
#--------------------------------------------------------------
if 'DetFlags' in dir():
    logDigitization_flags.warning("DetFlags already defined! This means DetFlags should have been fully configured already..")
    DetFlags.Print()
else :
    # include DetFlags
    # by default everything is off
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.ID_setOn()
    DetFlags.Lucid_setOn()
    #DetFlags.ZDC_setOn()
    #DetFlags.ALFA_setOn()
    #DetFlags.AFP_setOn()
    #DetFlags.FwdRegion_setOn()
    DetFlags.Calo_setOn()
    if hasattr(DetFlags, 'HGTD_setOff'):
        DetFlags.HGTD_setOff()
    DetFlags.Muon_setOn()
    DetFlags.Truth_setOn()
    DetFlags.LVL1_setOn()

checkOverlay = getattr(DetFlags, 'overlay', None)
if checkOverlay is not None:
    DetFlags.overlay.all_setOff()
else:
    logDigitization_flags.warning( 'DetFlags.overlay is not supported in this release' )
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

#-------------------------------------------
# Print Job Configuration
#-------------------------------------------
DetFlags.Print()
logDigitization_flags.info("Global jobProperties values:")
globalflags.print_JobProperties()
logDigitization_flags.info("Digitization jobProperties values:")
digitizationFlags.print_JobProperties()

#--------------------------------------------------------------
# Ensure AthenaCommonFlags.FilesInput is set.
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=athenaCommonFlags.PoolHitsInput.get_Value()

#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
if 'ALL' in digitizationFlags.overrideMetadata.get_Value():
    logDigitization_flags.info("Skipping input file MetaData check.")
else :
    from Digitization.DigitizationReadMetaData import readHITSFileMetadata
    readHITSFileMetadata()

#--------------------------------------------------------------
# Configure the job using jobproperties
#--------------------------------------------------------------
include("Digitization/ConfigDigitization.py")
logDigitization_flags.info("Digitization Configured Successfully.")

#--------------------------------------------------------------
# jobproperties should not be changed after this point
# except in the special case of overriding the run number
#--------------------------------------------------------------
if digitizationFlags.simRunNumber.statusOn or not digitizationFlags.dataRunNumber.statusOn :
    digitizationFlags.lock_JobProperties()

#--------------------------------------------------------------
# Write Digitization MetaData
#--------------------------------------------------------------
from Digitization.DigitizationWriteMetaData import writeDigitizationMetadata
writeDigitizationMetadata()

#--------------------------------------------------------------
# Pool Output (Change this to use a different file)
#--------------------------------------------------------------
if DetFlags.writeRDOPool.any_on():
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    streamRDO = AthenaPoolOutputStream("StreamRDO", athenaCommonFlags.PoolRDOOutput.get_Value(), asAlg=True, noTag=False)
    streamRDO.ForceRead = True
    from Digitization.DigiOutput import getStreamRDO_ItemList
    streamRDO.ItemList = getStreamRDO_ItemList(logDigitization_flags)
    streamRDO.AcceptAlgs += [ digitizationFlags.digiSteeringConf.get_Value() ]
    streamRDO.OutputFile = athenaCommonFlags.PoolRDOOutput()
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_ALGORITHM = '2'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_LEVEL = '1'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'" ]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = 3

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed
theApp.EvtMax = athenaCommonFlags.EvtMax()
# Number of input events to be skipped
ServiceMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
