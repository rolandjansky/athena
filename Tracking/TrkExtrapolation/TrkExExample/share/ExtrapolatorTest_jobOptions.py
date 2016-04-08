###############################################################
#
# Job options file fro the ExtrapolatorTest Algorithm
#
#==============================================================


#--------------------------------------------------------------
# Set to monte carlo
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob    
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AppMgr import theApp    
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags import GlobalFlags

# set the detectors on/off and steer the fast simulation flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

###############################################################
# Layout
DetDescrVersion = "ATLAS-GEO-18-01-01"

if DetDescrVersion is not None :
       from AthenaCommon.GlobalFlags import globalflags
       # --- defaulfrom AthenaCommon.GlobalFlags import GlobalFlags
       globalflags.DataSource.set_Value('geant4')
       globalflags.InputFormat.set_Value('pool')
       globalflags.DetGeo.set_Value('atlas')       
       # --- dfault is zero luminosity
       globalflags.Luminosity.set_Value('zero')
       # --- set geometry version
       globalflags.DetDescrVersion = DetDescrVersion
       
       # do the same for the GlobalFlags 
       
       # all det description
       include('RecExCond/AllDet_detDescr.py')

       # import the # the conditions setup
       from IOVDbSvc.CondDB import conddb
       conddb.setGlobalTag("OFLCOND-SDR-BS7T-05-24")

################################################################

# Number of events to be processed
theApp.EvtMax = 1000

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             =  3
MessageSvc.defaultLimit            = 1000000
MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"


#MagFieldAthenaSvc = Service( "MagFieldAthenaSvc" )
#MagFieldAthenaSvc.UseIMagFieldSvc = False
################################################################
# load the AtlasExtrapolator and the TestAlgorithm

from TrkExAlgs.TrkExAlgsConf import Trk__ExtrapolatorTest
MyExtrapolatorTest = Trk__ExtrapolatorTest('MyExtrapolatorTest')

MyExtrapolatorTest.StartPerigeeSigmaD0 = 0.
MyExtrapolatorTest.StartPerigeeSigmaZ0 = 0.
MyExtrapolatorTest.StartPerigeeMinPhi  = 0.
MyExtrapolatorTest.StartPerigeeMaxPhi  = 3.14
MyExtrapolatorTest.StartPerigeeMinEta  = -1.5
MyExtrapolatorTest.StartPerigeeMaxEta  = 1.5
MyExtrapolatorTest.StartPerigeeMinP    = 10000.
MyExtrapolatorTest.StartPerigeeMaxP    = 10000.
MyExtrapolatorTest.StartDirection      = -1
MyExtrapolatorTest.EventsPerExecute    = 100000
MyExtrapolatorTest.OutputLevel         = INFO

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
MyExtrapolator = AtlasExtrapolator()
ToolSvc += MyExtrapolator

MyExtrapolatorTest.Extrapolator = MyExtrapolator
MyExtrapolatorTest.ReferenceSurfaceRadius = [ 700. ]
MyExtrapolatorTest.ReferenceSurfaceHalfZ  = [ 2000. ]

# get the topSequence from __main__ namespace      
if 'topSequence' not in dir() :
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence() 

topSequence += MyExtrapolatorTest

