###############################################################
#
# Job options file for the EnergyLossValidation Algorithm
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
DetFlags.Muon_setOff()

###############################################################
# Layout

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-GEO-18-01-03'

if myGeo is not None :
       from AthenaCommon.GlobalFlags import globalflags
       # --- default: from AthenaCommon.GlobalFlags import GlobalFlags
       globalflags.DataSource.set_Value_and_Lock('geant4')
       globalflags.InputFormat.set_Value_and_Lock('pool')
       globalflags.DetGeo.set_Value_and_Lock('atlas')       
       # --- default is zero luminosity
       globalflags.Luminosity.set_Value_and_Lock('zero')
       # --- set geometry version
       globalflags.DetDescrVersion = myGeo
       
       # all det description
       include('RecExCond/AllDet_detDescr.py')

       # import the # the conditions setup
       from IOVDbSvc.CondDB import conddb
       if myGeo.split('-')[2] > 16 :
           conddb.setGlobalTag("OFLCOND-SDR-BS7T-05-02")
       else :
           conddb.setGlobalTag("OFLCOND-SDR-BS7T-04-15")

################################################################
# Number of events to be processed

if 'myMomentum' not in dir() :
    myMomentum = 5000

if 'myRunNumber' not in dir() :
    myRunNumber = 0

if 'myMaxEvent' not in dir() :
    myMaxEvent = 10

if 'myPt' not in dir() :
    myPt = 'pt'
    
if 'myExtrapolator' not in dir() :
    myExtrapolator = 'Atlas'

theApp.EvtMax = myMaxEvent


MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             = INFO
MessageSvc.defaultLimit            = 10000
MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"

################################################################
# load the FatrasExtrapolator

# the Detector flags to be imported
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers   = False # True # to test complex geometry

# validate also the collection of material layers
MaterialCollectionValidation       = False

# needed for both sim/reco
from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as STEP_Propagator
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
#from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as Propagator
#from TrkExHelixPropagator.TrkExHelixPropagatorConf import Trk__HelixPropagator as Propagator
from TrkExTools.TrkExToolsConf import Trk__Extrapolator as Extrapolator
from TrkExTools.TrkExToolsConf import Trk__Navigator as Navigator

# import the energy loss updator tool and assign it
from FatrasTools.FatrasToolsConf import Fatras__McEnergyLossUpdator
FatrasEnergyLossUpdator = Fatras__McEnergyLossUpdator(name = 'FatrasEnergyLossUpdator')
 # adding the Bethe-Heitler energy loss updator
from FatrasTools.FatrasToolsConf import Fatras__McBetheHeitlerEnergyLossUpdator
FatrasBetheHeitlerEnergyLossUpdator = Fatras__McBetheHeitlerEnergyLossUpdator(name = 'FatrasBetheHeitlerEnergyLossUpdator')
ToolSvc += FatrasBetheHeitlerEnergyLossUpdator
 # add it as the energy loss updator
FatrasEnergyLossUpdator.ElectronEnergyLossUpdator = FatrasBetheHeitlerEnergyLossUpdator
ToolSvc += FatrasEnergyLossUpdator

# import the multiple scattering tool and assign it
from TrkExTools.TrkExToolsConf import Trk__MultipleScatteringUpdator
FatrasMultipleScatteringUpdator = Trk__MultipleScatteringUpdator(name='FatrasMultipleScatteringUpdator')
ToolSvc += FatrasMultipleScatteringUpdator  

# hadronic interaction creator 
from FatrasTools.FatrasToolsConf import Fatras__HadIntProcessorParametric
FatrasHadIntProcessor = Fatras__HadIntProcessorParametric(name='FatrasadIntProcessor')
#FatrasHadIntProcessor.MinimumHadronicOutEnergy       = FatrasTuningFlags.HadronicInteractionMinMomentumOut()
#FatrasHadIntProcessor.HadronicInteractionValidation  = FatrasValidationFlags.HadronicInteractions()  
ToolSvc += FatrasHadIntProcessor

##############################################################################################################################     
# The extrapolator for the ID simulation    
# the Navigator
include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
FatrasNavigator    = Navigator(name = 'FatrasNavigatorID')
FatrasNavigator.TrackingGeometrySvc=AtlasTrackingGeometrySvc
ToolSvc += FatrasNavigator
# Updator for Fatras - MC or Fatras
from FatrasTools.FatrasToolsConf import Fatras__McMaterialEffectsUpdator
FatrasMaterialUpdator      = Fatras__McMaterialEffectsUpdator(name ='FatrasMaterialUpdatorID')
# hadronic interactions
FatrasMaterialUpdator.HadronicInteractionProcessor   = FatrasHadIntProcessor
ToolSvc += FatrasMaterialUpdator
# energy loss
FatrasMaterialUpdator.EnergyLoss = True
FatrasMaterialUpdator.EnergyLossUpdator = FatrasEnergyLossUpdator
# mutiple scattering
FatrasMaterialUpdator.MultipleScattering        = True
FatrasMaterialUpdator.MultipleScatteringUpdator = FatrasMultipleScatteringUpdator         
# the validation output 
# the sim propagator
FatrasPropagator   = Propagator('FatrasPropagatorID')  
ToolSvc += FatrasPropagator
FatrasSTEP_Propagator   = STEP_Propagator('FatrasSTEP_PropagatorID')  
ToolSvc += FatrasSTEP_Propagator
# Extrapolator for fatras 
FatrasExtrapolator = Extrapolator(name='FatrasExtrapolatorID')
# assign the tools
FatrasExtrapolator.Navigator                = FatrasNavigator
FatrasExtrapolator.MaterialEffectsUpdators  = [ FatrasMaterialUpdator ]
FatrasExtrapolator.Propagators              = [ FatrasPropagator ]
FatrasExtrapolator.STEP_Propagator = FatrasSTEP_Propagator
ToolSvc += FatrasExtrapolator


################################################################
# load the AtlasExtrapolator

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
MyExtrapolator = AtlasExtrapolator(name='TestExtrapolator')
MyExtrapolator.NavigationStatisticsOutput       = True
MyExtrapolator.OutputLevel                      = INFO
MyExtrapolator.StopWithNavigationBreak          = True
MyExtrapolator.DoCaloDynamic                    = False
MyExtrapolator.MaterialEffectsOnTrackValidation = True
MyExtrapolator.RequireMaterialDestinationHit    = False
MyExtrapolator.setEnergyLoss(True)


# fix
from TrkExTools.TrkExToolsConf import Trk__EnergyLossUpdator
MyEnergyLossUpdator = Trk__EnergyLossUpdator(name='TestEnergyLossUpdator')
#MyEnergyLossUpdator.UseFix = False
MyEnergyLossUpdator.OutputLevel = VERBOSE
print MyEnergyLossUpdator
ToolSvc += MyEnergyLossUpdator

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
MyMaterialEffectsUpdator = Trk__MaterialEffectsUpdator(name='TestMaterialEffectsUpdator')
#MyMaterialEffectsUpdator.MostProbableEnergyLoss = True
MyMaterialEffectsUpdator.EnergyLossUpdator = MyEnergyLossUpdator
print MyMaterialEffectsUpdator
ToolSvc += MyMaterialEffectsUpdator

#MyExtrapolator.EnergyLossUpdators      = [ MyEnergyLossUpdator ]
MyExtrapolator.MaterialEffectsUpdators = [ MyMaterialEffectsUpdator ]


# proposal by Niels
MyExtrapolator.Tolerance = 0.0011

#from TrkExTools.TrkExToolsConf import Trk__Navigator
MyNavigator = Navigator(name='TestNavigator')
MyNavigator.OutputLevel = INFO
ToolSvc       += MyNavigator

MyExtrapolator.Navigator = MyNavigator
ToolSvc       += MyExtrapolator


################################################################
# choose the Extrapolator

# create the algorithm
from TrkExAlgs.TrkExAlgsConf import Trk__EnergyLossExtrapolationValidation 

EnergyLossValidation = Trk__EnergyLossExtrapolationValidation('EnergyLossExtrapolationValidation')

if myExtrapolator=='Atlas' :
    EnergyLossValidation.Extrapolator     = MyExtrapolator
else :
    EnergyLossValidation.Extrapolator     = FatrasExtrapolator
  
EnergyLossValidation.StartPerigeeMomentum = myMomentum
EnergyLossValidation.StartPerigeeUsePt    = (myPt=='pt') #False
EnergyLossValidation.StartPerigeeMinEta   =-2.7
EnergyLossValidation.StartPerigeeMaxEta   = 2.7


### Energy Loss validation (for Niels van Eldik and Peter Kluit)
EnergyLossValidation.ValidationCylinders  = 6
EnergyLossValidation.ValidationCylinderR  = [ 0,   36.5,  250,  550, 1085, 1170, 4250 ]
EnergyLossValidation.ValidationCylinderZ  = [ 0, 2748  , 2750, 2820, 3120, 3520, 6527 ]

### ID barrel study (for Beate Heinemann)
#EnergyLossValidation.ValidationCylinders  = 3
#EnergyLossValidation.ValidationCylinderR  = [ 0,  600, 1170, 4250 ]
#EnergyLossValidation.ValidationCylinderZ  = [ 0, 2700, 2748, 6527 ]

### MS study (for Niels van Eldik)
#EnergyLossValidation.ValidationCylinders  = 6
#EnergyLossValidation.ValidationCylinderR  = [ 0, 4250, 4440, 6200,  8500, 11500, 12000 ]
#EnergyLossValidation.ValidationCylinderZ  = [ 0, 6527, 7100, 8000, 10500, 16000, 22025 ]

#EnergyLossValidation.ValidationCylinderR  = [ 0, 4250, 6200,  8500, 12000, 12500, 13000 ]
#EnergyLossValidation.ValidationCylinderZ  = [ 0, 6527, 8000, 10500, 12500, 16000, 22000 ]

### only barrel (according to TrackingGeometry)
#EnergyLossValidation.ValidationCylinderR  = [ 0,   39.0,  163.7 ,  545.5, 1170   , 4250, 10000, 13000 ]
#EnergyLossValidation.ValidationCylinderZ  = [ 0, 2748  ,  455.5 ,  795.7,  795.8 , 3250, 15000, 18650 ]
### total container (according to TrackingGeometry)
#EnergyLossValidation.ValidationCylinderZ  = [ 0, 2748  , 2748.05, 2748.1, 2748.15, 6527, 22000, ]

### old values (according to Atlas Geometry)
#EnergyLossValidation.ValidationCylinderR  = [ 0, 34.5, 250,  550, 1085, 4250, 13000 ]
#EnergyLossValidation.ValidationCylinderZ  = [ 0, 2748, 680, 2820, 3120, 6500, 22500 ]

EnergyLossValidation.UseMaterialCollection= True
EnergyLossValidation.OutputLevel          = INFO

# get the topSequence      
if 'topSequence' not in dir() :
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    
topSequence += EnergyLossValidation

# to the AlgSequence

# Ntuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "val DATAFILE='"+myExtrapolator+"."+str(myGeo)+'.'+myPt+"."+str(myMomentum)+".MeV."+str(myMaxEvent)+"."+str(myRunNumber)+".root' TYPE='ROOT' OPT='RECREATE'" ]
#ServiceMgr.THistSvc.Output = [ "val DATAFILE='AtlasExtrapolation."+str(myAthena)+"."+str(myMomentum)+".MeV."+str(myMaxEvent)+"."+str(myRunNumber)+".root' TYPE='ROOT' OPT='RECREATE'" ]

