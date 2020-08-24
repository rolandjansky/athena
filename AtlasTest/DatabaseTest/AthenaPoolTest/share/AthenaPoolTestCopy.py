###############################################################
#
# Job options file
#
## @file AthenaPoolTestStep2Write.py
##
## @brief For Athena POOL test: write out AthenaPoolTest data objects
## in a second file to test navigation
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from CLIDComps.CLIDCompsConf import ClassIDSvc
svcMgr += ClassIDSvc()
ClassIDSvc.CLIDDBFiles = ['CLIDSvc/clid.db']

from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_commis()

# Reading G4 simulated data from Pool
#GlobalFlags.DataSource.set_geant4()
#GlobalFlags.InputFormat.set_pool()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
import AthenaPoolCnvSvc.WriteAthenaPool
DetDescrVersion = "ATLAS-CommNF-00-00-00"
include ("AtlasGeoModel/SetGeometryVersion.py")
GeoModelSvc = Service( "GeoModelSvc" )
include ("AtlasGeoModel/GeoModelInit.py")
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.01.01.Initial.Light"
#--------------------------------------------------------------
# Define the input parameters
#--------------------------------------------------------------

svcMgr.EventSelector.InputCollections        = [ "rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020897/ESD/M4.0020897.debug.L1TT-b00000011.ESD.v13002507.part0013._lumi0000._0001.1" ] 

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = -1
#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )


include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )	
include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
include ("CaloAthenaPool/CaloAthenaPool_joboptions.py")
theApp.Dlls   += [ "MuonEventAthenaPoolPoolCnv" ]
# Run OutputStream as an algorithm
topSequence += AthenaPoolOutputStream( "Stream1" )

# Must make sure that no OutStream's have been declared
theApp.OutStream    = []; 
# Stream's output file
Stream1.WritingTool = "AthenaOutputStreamTool"
Stream1.OutputFile  = "SimplePoolFile2.root"
Stream1.OutputLevel = DEBUG

# Copy everything from the input and must force reading of all input
# objects
Stream1.TakeItemsFromInput     = True
# Turn off provenance - means that new copy does NOT point back to its
# input file, but it DOES retain the possibility to navigate to the
# files upstream of the input. Thus the output "replaces" the input in
# terms of back-navigation.
Stream1.ExtendProvenanceRecord = False

print Stream1.ItemList

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc = Service( "MessageSvc" )
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
#AthenaEventLoopMgr.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
