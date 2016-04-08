# block the include for all others
include.block('TrkExExample/CombinedExtrapolator.py')
######################################################
# CombinedExtrapolator module
#
# it using the combined Tracking Geometry
#
######################################################

print 'Setting up CombinedExtrapolator'
#
#################################################################################
# Material for the Geometry
#
# if only ID is switched on different COOLDB Folders
#    and different Tag is used
# 
#################################################################################
# import the DetFlags for the setting
from AthenaCommon.DetFlags import DetFlags

CoolDataBaseFolder = '/Indet/TrackingGeo/LayerMaterial'
AtlasMaterialTag   = 'InDetLayerMat_v5_'

# set the CaloTrkGeo if not set --- Calorimeter in Beta Test
if not 'CaloTrkGeo' in dir():
   CaloTrkGeo=DetFlags.Calo_on()

# set the MuonTrkGeo if not set --- Muon System in Prototype state
if not 'MuonTrkGeo' in dir():
   MuonTrkGeo = DetFlags.Muon_on()

# the steering for the CaloEnergyLoss tool
if not 'doCaloTrkEloss' in dir():
  doCaloTrkEloss = False
  
if CaloTrkGeo:
   CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterial'
   AtlasMaterialTag   = 'AtlasLayerMat_v5_'

# load the right folders (preparation for calo inclusion)
from IOVDbSvc.CondDB import conddb
if DetFlags.ID_on() and not CaloTrkGeo:
   conddb.addFolder('INDET',CoolDataBaseFolder +'<tag>TagInfo/'+AtlasMaterialTag+'/GeoAtlas</tag>')
if CaloTrkGeo :
   conddb.addFolder('GLOBAL',CoolDataBaseFolder +'<tag>TagInfo/'+AtlasMaterialTag+'/GeoAtlas</tag>')

#################################################################################
# The Geometry Builder
#################################################################################

# the name to register the Geometry
AtlasTrackingGeometryName = 'AtlasTrackingGeometry'

# the geometry builder alg tool
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
AtlasGeometryBuilder = Trk__GeometryBuilder(name = 'AtlasGeometryBuilder')
# register it to tool svc
ToolSvc += AtlasGeometryBuilder
# switch on the Detectors
AtlasGeometryBuilder.InDetTrackingGeometry  = DetFlags.ID_on()
AtlasGeometryBuilder.CaloTrackingGeometry   = DetFlags.Calo_on()
AtlasGeometryBuilder.MuonTrackingGeometry   = DetFlags.Muon_on()

# (ID) 
if DetFlags.ID_on() :
  # get hand on the ID Tracking Geometry Builder
  from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__InDetTrackingGeometryBuilder
  InDetTrackingGeometryBuilder = InDet__InDetTrackingGeometryBuilder(name ='InDetTrackingGeometryBuilder')
  InDetTrackingGeometryBuilder.EncloseVolumesInEnevlope = not DetFlags.Calo_on()
  # make a public tool out of it
  ToolSvc += InDetTrackingGeometryBuilder
  # and give it to the Geometry Builder
  AtlasGeometryBuilder.InDetTrackingGeometryBuilder = InDetTrackingGeometryBuilder
  # 
# (Calo)
if DetFlags.Calo_on() and CaloTrkGeo:
   AtlasGeometryBuilder.CaloTrackingGeometry   = True
   from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilder import ConfiguredCaloTrackingGeometryBuilder
   CaloTrackingGeometryBuilder = ConfiguredCaloTrackingGeometryBuilder(name = 'CaloTrackingGeometryBuilder')
   ToolSvc += CaloTrackingGeometryBuilder
   # and give it to the Geometry Builder
   AtlasGeometryBuilder.CaloTrackingGeometryBuilder = CaloTrackingGeometryBuilder

# (Muon) --- not in production yet
if DetFlags.Muon_on() and MuonTrkGeo:
   AtlasGeometryBuilder.MuonTrackingGeometry    = True
   from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
   AtlasGeometryBuilder.MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder
      
# then get the 
from TrkDetDescrSvc.TrkDetDescrSvcConf import TrackingGeometrySvc
AtlasTrackingGeometrySvc = TrackingGeometrySvc(name = 'AtlasTrackingGeometrySvc')
# register the Builder 
AtlasTrackingGeometrySvc.GeometryBuilder = AtlasGeometryBuilder
# and register the name under which the geometry is registered
AtlasTrackingGeometrySvc.TrackingGeometryName = AtlasTrackingGeometryName
# some flags - only works when ID is on
AtlasTrackingGeometrySvc.AssignMaterialFromCOOL   = DetFlags.ID_on()
AtlasTrackingGeometrySvc.BuildGeometryFromTagInfo = True
AtlasTrackingGeometrySvc.GlobalScaleFactor        = 1.0
# switch to the rigth CoolDB folder 
AtlasTrackingGeometrySvc.InputLayerMaterialSetName = CoolDataBaseFolder

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += AtlasTrackingGeometrySvc

print '* [ Configuration : start ] *** AtlasTrackingGeometry ********************************'
print '* [ TrackingGeometrySvc   ]'
print AtlasTrackingGeometrySvc
print '* [ GeometryBuilder       ]'
print AtlasGeometryBuilder
print '* [ Configuration : end   ] *** AtlasTrackingGeometry ********************************'

# the navigator Configurable way
from TrkExTools.TrkExToolsConf import Trk__Navigator
CombinedNavigator = Trk__Navigator( name = 'CombinedNavigator',
                                    TrackingGeometrySvc = AtlasTrackingGeometrySvc )
ToolSvc += CombinedNavigator
print      CombinedNavigator


# PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------

AtlasPropagators  = []

from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as RkPropagator
AtlasRungeKuttaPropagator = RkPropagator(name = 'AtlasRungeKuttaPropagator')
ToolSvc += AtlasRungeKuttaPropagator

AtlasPropagators += [AtlasRungeKuttaPropagator]

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as STEP_Propagator
AtlasSTEP_Propagator = STEP_Propagator(name = 'AtlasSTEP_Propagator')
ToolSvc += AtlasSTEP_Propagator

AtlasPropagators += [AtlasSTEP_Propagator]

# UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       

AtlasUpdators    = []

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
ToolSvc += AtlasMaterialEffectsUpdator

AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]

AtlasMaterialEffectsUpdatorLandau = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdatorLandau')
AtlasMaterialEffectsUpdatorLandau.LandauMode = True
ToolSvc += AtlasMaterialEffectsUpdatorLandau

AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]
                     

# CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

AtlasSubPropagators = []
AtlasSubUpdators = []

# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name() ]
AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name() ]

# default for Calo is (Rk,MatLandau)
AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name() ]
AtlasSubUpdators    += [ AtlasMaterialEffectsUpdatorLandau.name() ]

# default for MS is (STEP,Mat)
AtlasSubPropagators += [ AtlasSTEP_Propagator.name() ]
AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name() ]
# ----------------------------------------------------------------------------------------------------------
          
# the Extrapolator
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
CombinedExtrapolator = Trk__Extrapolator( name = 'CombinedExtrapolator', 
                                          Navigator = CombinedNavigator,
                                          MaterialEffectsUpdators = AtlasUpdators,
                                          Propagators = AtlasPropagators,
                                          SubPropagators = AtlasSubPropagators,
                                          SubMEUpdators = AtlasSubUpdators 
                                          )
ToolSvc += CombinedExtrapolator
print CombinedExtrapolator

print 'Finished setting up CombinedExtrapolator'

