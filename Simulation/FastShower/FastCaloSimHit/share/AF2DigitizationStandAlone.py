###############################################################
#
# Job options file to run:
#	 Digitization
#	 LVL1 Simulation
#	 ByteStream conversion
#
# Author: Sven Vahsen and John Chapman
#         adapted by Matthew Beckingham to include
#         FastCaloCellGetter to digitise FastCalo hits
#==============================================================
from AthenaCommon.Logging import logging
logDigitization_flags = logging.getLogger( 'Digitization' )

#--------------------------------------------------------------
# Get Digitization Flags (This sets Global and Det Flags)
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
#from DigitizationFlags import jobproperties
digitization = jobproperties.Digitization

#--------------------------------------------------------------
# Set Global flags for this run
#--------------------------------------------------------------
if 'GlobalFlags' in dir():
   logDigitization_flags.warning("GlobalFlags already imported:")
   #FIXME
else:
   from AthenaCommon.GlobalFlags import GlobalFlags
   GlobalFlags.DetGeo.set_atlas()
   GlobalFlags.DataSource.set_geant4()
   GlobalFlags.InputFormat.set_pool()
   
#--------------------------------------------------------------
# Set Detector flags for this run
#--------------------------------------------------------------
if 'DetFlags' in dir():
   logDigitization_flags.warning("DetFlags already defined:")
   logDigitization_flags.warning("This means DetFlags should have been fully configured already!")
   DetFlags.Print()   
else:
   # include DetFlags
   # by default everything is off
   from AthenaCommon.DetFlags import DetFlags
   DetFlags.ID_setOn()
   DetFlags.Calo_setOn()
   DetFlags.Muon_setOn()
   DetFlags.Truth_setOn()
   DetFlags.LVL1_setOn()

DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

#-----------------------------------------------------------
# Check Beam and Digitization jobproperties are synchronised
#-----------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
logDigitization_flags.info("Beam jobProperties values:")
jobproperties.Beam.print_JobProperties()
if jobproperties.Beam.numberOfCollisions.get_Value() != digitization.numberOfCollisions.get_Value() :
   logDigitization_flags.warning("jobproperties.Beam.numberOfCollisions != jobproperties.Digitization.numberOfCollisions")
   logDigitization_flags.warning("Overriding jobproperties.Beam.numberOfCollisions value")
   jobproperties.Beam.numberOfCollisions = digitization.numberOfCollisions.get_Value()
   jobproperties.Beam.override = True ## just incase - default is True

if jobproperties.Beam.bunchSpacing.get_Value() != digitization.bunchSpacing.get_Value() :
   logDigitization_flags.warning("jobproperties.Beam.bunchSpacing != jobproperties.Digitization.bunchSpacing")
   logDigitization_flags.warning("Overriding jobproperties.Beam.bunchSpacing value")
   jobproperties.Beam.bunchSpacing = digitization.bunchSpacing.get_Value()
   jobproperties.Beam.override = True ## just incase - default is True
   
#-------------------------------------------
# Print Job Configuration
#-------------------------------------------
DetFlags.Print()
GlobalFlags.Print()
logDigitization_flags.info("Digitization jobProperties values:")
digitization.print_JobProperties()

#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
if not digitization.overrideMetadata.get_Value() :
   include("Digitization/DigitizationReadMetaData.py")

#--------------------------------------------------------------
# Configure the job using jobproperties
#--------------------------------------------------------------
include("Digitization/ConfigDigitization.py")

#--------------------------------------------------------------
# jobproperties should not be changed after this point
#--------------------------------------------------------------
digitization.lock_JobProperties()

#--------------------------------------------------------------
# Write Digitization MetaData 
#--------------------------------------------------------------
include("Digitization/DigitizationWriteMetaData.py")

# Pool input (Change this to use a different file)
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'EventSelector'):
   import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = jobproperties.AthenaCommonFlags.PoolHitsInput()


# Pool Output (Change this to use a different file)
if DetFlags.writeRDOPool.any_on():
    Stream1.OutputFile = jobproperties.AthenaCommonFlags.PoolRDOOutput()


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = 2

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed
theApp.EvtMax = jobproperties.AthenaCommonFlags.EvtMax()
# Number of input events to be skipped
ServiceMgr.EventSelector.SkipEvents = jobproperties.AthenaCommonFlags.SkipEvents()

#----------------------------------------------------------------------
# Issue ERRORs if old-style flags have been set somewhere in jobOptions
#----------------------------------------------------------------------

OldDigitizationFlags = ["doCaloNoise","doInDetNoise","doMuonNoise","doMinimumBias","doCavern","doBeamGas",
                        "doBeamHalo","numberOfCollisions","numberOfCavern","numberOfBeamHalo","numberOfBeamGas",
                        "initialBunchCrossing","finalBunchCrossing","minBiasInputCols","cavernInputCols",
                        "beamGasInputCols","beamHaloInputCols","bunchSpacing","digitizationRndmSvc"]
for o in [ o for o in OldDigitizationFlags if o in dir() ]:
   logDigitization_flags.error('Old-style DigitizationFlag "%s" defined, but will be ignored in this job! Please update your jobOptions to use "jobproperties.Digitization.%s" instead!', o,o)
            
OldAthenaCommonFlags = ["EvtMax","SkipEvents","PoolEvgenInput","PoolHitsInput","PoolRDOInput","PoolESDInput",
                        "PoolAODInput","PoolEvgenOutput","PoolHitsOutput","PoolRDOOutput","PoolESDOutput","PoolAODOutput"]
for o in [ o for o in OldAthenaCommonFlags if o in dir() ]:
   logDigitization_flags.error('Old-style AthenaCommonFlag "%s" defined, but will be ignored in this job! Please update your jobOptions to use "jobproperties.AthenaCommonFlags.%s" instead!', o,o)
   
OldGlobalFlags=["DetGeo","DataSource","InputFormat","Luminosity","DetDescrVersion"]
for o in [ o for o in OldGlobalFlags if o in dir() ]:
   logDigitization_flags.error('Old-style Global "%s" defined, but will be ignored in this job! Please update your jobOptions to use the corresponding jobProperty!',o)

#--------------------------------------------------------------
#  Run the fast sim 
#--------------------------------------------------------------

print " Now do the FastSim ....."

#--------------------------------------------------------------
# FastHitConv
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

# FastCaloSimPart

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

print "Digitization jobProperties values (after FastCaloSim setup):"
#DetFlags.Print()
#GlobalFlags.Print()
#digitization.print_JobProperties()

from AthenaCommon.AppMgr import ServiceMgr

# Add top algorithms to be run
from FastCaloSimHit.FastCaloSimHitConf import FastHitConv
theFastHitConv=FastHitConv("MyFastHitConv")
#job += FastHitConv( "MyFastHitConv" )   # 1 alg, named "FastHitConv"
topSequence.insert(0,theFastHitConv);

job.MyFastHitConv.OutputLevel = ALL
job.OutputLevel = DEBUG

print topSequence

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSim = True
jobproperties.CaloCellFlags.doFastCaloSimNoise = False
jobproperties.CaloCellFlags.doLArCellEmMisCalib = False

from FastCaloSimHit.FastCaloCellGetter import CaloCellGetter
theCaloCellGetter=CaloCellGetter()
#jobproperties.CaloCellFlags.doFastCaloSim = False
#theCaloCellGetter._CaloCellMakerHandle.OutputLevel = 2

print "Set the TileHitVectors and LArHitContainers"

topSequence.TileHitVecToCnt.TileHitVectors+=["TileHitVec_Fast"]

topSequence.digitmaker1.EmBarrelHitContainerName = "LArHitEMB_FastComb" 
topSequence.digitmaker1.EmEndCapHitContainerName = "LArHitEMEC_FastComb"
topSequence.digitmaker1.HecHitContainerName = "LArHitHEC_FastComb"
topSequence.digitmaker1.ForWardHitContainerName = "LArHitFCAL_FastComb" # It really is spelt with a "W" in the middle!


# Print out job sequence
print " The jobsequence is now:"
print job

print " ... and the topsequence:"
print topSequence
