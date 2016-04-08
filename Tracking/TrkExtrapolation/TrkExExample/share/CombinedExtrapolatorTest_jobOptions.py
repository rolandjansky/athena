###############################################################
#
# Job options file fro the ExtrapolatorTest Algorithm
#
#==============================================================
#theApp.setup( MONTECARLO )
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# Global Flags to be set ######################################
varInit=dir()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence() 

# some very general flags, EvtMax, SkipEvents, PoolESDInput etc...
include ("AthenaCommon/AthenaCommonFlags.py")
# prevent this file to be included a second time
include.block("AthenaCommon/AthenaCommonFlags.py")

if 'GlobalFlags' in varInit:
   print "GlobalFlags already imported:"
else:
   from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo._beenSet = True
#GlobalFlags.DetGeo._flag_ctbh8 = True

GlobalFlags.DataSource._beenSet     = True
GlobalFlags.DataSource._flag_geant4 = True

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

###############################################################
# Layout
DetDescrVersion = "ATLAS-CSC-00-00-00"

IOVDbSvc = Service("IOVDbSvc")
if "AlignmentGlobalTag" in dir():
   IOVDbSvc.GlobalTag = AlignmentGlobalTag
else:  
   IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-00-00"


include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

################################################################

# Number of events to be processed
theApp.EvtMax = 50

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             =  3
MessageSvc.defaultLimit            = 1000000
MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"

################################################################
# load the AtlasExtrapolator
include ( 'TrkExExample/CombinedExtrapolator.py' )

from TrkExAlgs.TrkExAlgsConf import Trk__CombinedExtrapolatorTest
CombinedExtrapolatorTest = Trk__CombinedExtrapolatorTest('CombinedExtrapolatorTest')

CombinedExtrapolatorTest.Extrapolator = CombinedExtrapolator

topSequence += CombinedExtrapolatorTest

