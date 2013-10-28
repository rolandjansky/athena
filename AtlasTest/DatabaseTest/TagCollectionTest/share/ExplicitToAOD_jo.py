## @file ExplicitToAOD_jo.py
## @brief Reads in an explicit ROOT collection and writes out an AOD file.
## @author J. Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: ExplicitToAOD_jo.py,v 1.4 2008-04-09 15:09:46 gemmeren Exp $

from AthenaCommon.GlobalFlags import GlobalFlags
from ParticleEventAthenaPool.AODFlags import AODFlags

GlobalFlags.InputFormat.set_pool()

AODFlags.allSetOff()
AODFlags.fastSetOn()
#AODFlags.Photon = True
AODFlags.Print()

#--------------------------------------------------------------
# Load POOL Read support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
#--------------------------------------------------------------
#---   Load Converters for tests  -----
#--------------------------------------------------------------
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )
include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )
                                                                                
PoolSvc = Service("PoolSvc")
#PoolSvc.Output = "EventTagWriter.pool"
PoolSvc.ReadCatalog = [ "xmlcatalog_file:SplittableData.xml" ]
                                                                                
EventSelector = Service( "EventSelector" )
EventSelector.CollectionType = "ExplicitROOT"
#EventSelector.InputCollections = [ "AODCollection" ]; # The input file name
EventSelector.InputCollections = [ "SplittableCollection" ]; # The input file name
EventSelector.Query = "nEGamma >3"
#
include( "ParticleEventAthenaPool/AODtoPool_jobOptions.py" )
# include stream transform
include( "AthenaPoolMultiTest/ExampleStreamConfig.py" )
 
theApp.EvtMax = 200000

# Stream1 is defined in the AODtoPool job options
Stream1.OutputLevel = INFO
Stream1.ForceRead = "true"
Stream1.OutputFile = "AODExplicit.root"
ExampleStreamConfig(Stream1)
