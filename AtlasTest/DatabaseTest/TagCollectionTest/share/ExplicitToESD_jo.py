## @file ExplicitToESD_jo.py
## @brief Reads in an explicit ROOT collection and writes out an ESD file.
## @author J. Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: ExplicitToESD_jo.py,v 1.3 2005-01-08 01:08:43 kkarr Exp $

###############################################################
#
# Job options file
#
# This Job option:
# ----------------
# 1. Reads the data from the SimplePoolFile1.root file
#    that has been written with the AthneaPoolExample_WriteJobOptions.py
#
# 2. Writes another SimplePoolFile3.root file using ReWriteData algorithm
#
#==============================================================
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 200000

GlobalFlags.InputFormat.set_pool()
                                                                                
DetFlags.all_setOff()
DetFlags.ID_setOn()
#DetFlags.fastSetOn()
DetFlags.Print()

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )

# If you need to change or add input file catalogs
PoolSvc = Service("PoolSvc")
PoolSvc.ReadCatalog = [ "XMLFileCatalog_file:SplittableData.xml" ]
# Or if you need to change the name of the output file catalog
PoolSvc.WriteCatalog = "XMLFileCatalog_file:EventSplit.xml"
 
EventSelector = Service( "EventSelector" )
EventSelector.CollectionType = "ExplicitROOT"
EventSelector.InputCollections =  ["SplittableCollection"]; # The input file name
EventSelector.Query = "nEGamma>3"

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
theApp.Dlls  += [ "AthenaPoolExampleAlgorithms" ]
theApp.Dlls  += [ "AthenaPoolMultiTest" ]

include( "AthenaPoolExampleData/AthenaPoolExampleDataDict_joboptions.py" )

include( "RecExCommon/RecoOutputPoolCnv_jobOptions.py" )
include( "AthenaPoolMultiTest/AllAutoGenConverters_joboptions.py")
# include stream transform
include( "AthenaPoolMultiTest/ExampleStreamConfig.py" )

#--------------------------------------------------------------
# Configure the filters
#--------------------------------------------------------------
theApp.TopAlg += [ "ReadData" ]  # print out some data to chk its there

#--------------------------------------------------------------
#---  Set up the streams for the filters
#--------------------------------------------------------------

# bit 1
theApp.TopAlg     += [ "AthenaOutputStream/Stream1" ]
Stream1            = Algorithm( "Stream1" )
Stream1.ForceRead  = "true"
Stream1.OutputFile = "ESDExplicit.root"; # The output file name
include( "RecExCommon/RecoOutputItemList_jobOptions.py")
ExampleStreamConfig(Stream1)
Stream1.ItemList  += [ "FauxTriggerMap#*" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = DEBUG
#==============================================================
#
# End of job options file
#
###############################################################
