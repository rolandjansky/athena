###############################################################
#
# Job options file for the ExtrapolatorTest Algorithm
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
DetDescrVersion = "ATLAS-GEO-16-00-00"

if DetDescrVersion is not None :
       from AthenaCommon.GlobalFlags import globalflags
       # --- default: from AthenaCommon.GlobalFlags import GlobalFlags
       globalflags.DataSource.set_Value_and_Lock('geant4')
       globalflags.InputFormat.set_Value_and_Lock('pool')
       globalflags.DetGeo.set_Value_and_Lock('atlas')       
       # --- default is zero luminosity
       globalflags.Luminosity.set_Value_and_Lock('zero')
       # --- set geometry version
       globalflags.DetDescrVersion = DetDescrVersion
       
       # do the same for the GlobalFlags 
       
       # all det description
       include('RecExCond/AllDet_detDescr.py')

       # import the # the conditions setup
       from IOVDbSvc.CondDB import conddb
       conddb.setGlobalTag("OFLCOND-DR-BS7T-ANom-11")


import MagFieldServices.SetupField

################################################################
# Number of events to be processed
theApp.EvtMax = 10

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             = 3
MessageSvc.defaultLimit            = 100000
MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"

################################################################
# load the AtlasExtrapolator

# the Detector flags to be imported
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers = False # True # to test complex geometry

# validate also the collection of material layers
MaterialCollectionValidation = False
        
        
# create the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
MyExtrapolator = AtlasExtrapolator(name='TestExtrapolator')
MyExtrapolator.NavigationStatisticsOutput       = True
MyExtrapolator.OutputLevel                      = INFO
MyExtrapolator.StopWithNavigationBreak          = True
MyExtrapolator.DoCaloDynamic                    = True
MyExtrapolator.MaterialEffectsOnTrackValidation = True
MyExtrapolator.RequireMaterialDestinationHit    = MaterialCollectionValidation
MyExtrapolator.setEnergyLoss(False)

from TrkExTools.TrkExToolsConf import Trk__Navigator
MyNavigator = Trk__Navigator(name='TestNavigator')
MyNavigator.OutputLevel = INFO
ToolSvc       += MyNavigator

MyExtrapolator.Navigator = MyNavigator
ToolSvc       += MyExtrapolator

# create the algorithm
from TrkExAlgs.TrkExAlgsConf import Trk__ExtrapolationValidation 

ExtrapolationValidation = Trk__ExtrapolationValidation('ExtrapolationValidation')
ExtrapolationValidation.Extrapolator     = MyExtrapolator
ExtrapolationValidation.StartPerigeeMinP = 10000
ExtrapolationValidation.StartPerigeeMaxP = 150000
ExtrapolationValidation.ValidateMaterialCollection = MaterialCollectionValidation

# get the topSequence      
if 'topSequence' not in dir() :
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence() 
    
topSequence += ExtrapolationValidation

# to the AlgSequence

# Ntuple
from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc.Output = [ "val DATAFILE='ExtrapolationValidation.root' TYPE='ROOT' OPT='RECREATE'" ]

svcMgr += THistSvc() 

