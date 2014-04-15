#################################################################
#
## @file CheckImplicit_jo.py
## @brief
## This is the third script in the AthenaPoolMultiTest gmake check
##<br>
## Standard: Uses the 4 implicit collections produced by 
##           EventSplit_jo.py as input. Uses ReadData to make 
##           sure they're readable with correct data.
##<br>
## Extra: can accept command line designation of infile, as long 
##        as infile is in one of the file catalogs.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: CheckImplicit_jo.py,v 1.11 2009-04-01 21:18:32 cranshaw Exp $
#=================================================================

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")

from AthenaCommon.AppMgr import theApp

theApp.EvtMax = 200000

#import sys
#test=sys.argv[1]
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )
#--------------------------------------------------------------
#---   Read portion  -----
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool


from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# If you need to change or add input file catalogs
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.ReadCatalog  = ["XMLFileCatalog_file:EventSplit.xml"]
svcMgr.PoolSvc.ReadCatalog += ["XMLFileCatalog_file:Task1.xml"]
svcMgr.PoolSvc.ReadCatalog += ["XMLFileCatalog_file:SplittableData.xml"]
 
# The input file names
try:
  svcMgr.EventSelector.InputCollections  = infile; 
except:
  svcMgr.EventSelector.InputCollections  = [ "AthenaPoolMultiTest_Split1.root" ]; 
  svcMgr.EventSelector.InputCollections += [ "AthenaPoolMultiTest_Split2.root" ]; 
#  EventSelector.InputCollections += [ "AthenaPoolMultiTest_Split3.root" ]; 
  svcMgr.EventSelector.InputCollections += [ "AthenaPoolMultiTest_Missed.root" ]; 

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

#include( "AthenaPoolTestData/AthenaPoolTestDataDict_joboptions.py" )
#include( "AthenaPoolExampleData/AthenaPoolExampleDataDict_joboptions.py" )

#--------------------------------------------------------------
# Configure the filters
#--------------------------------------------------------------
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData

topSequence += AthPoolEx__ReadData("ReadData")

#include( "AthenaPoolExampleConverter/AthenaPoolExampleConverterDict_joboptions.py" ) # Load dict for Streamer
from AthenaPoolServices.AthenaRootStreamerSvcConf import AthenaRootStreamerSvc
StreamerSvc = AthenaRootStreamerSvc()
StreamerSvc.Streamers  += [ "ExampleHitStreamer_p0" ]
svcMgr += StreamerSvc

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.infoLimit = 5000
svcMgr.MessageSvc.debugLimit = 5000
svcMgr.MessageSvc.OutputLevel = INFO
#==============================================================
#
# End of job options file
#
###############################################################
