#--------------------------------------------------------------
# Geometry test jobOptions: SLHC 
#
# There are two geometry tests available. Only one can be run at a time:
#   doFastCheck True:  Runs the "GeometryCheck".
#                      This is a faster checker and finds most overlaps.
#   doFastCheck False: Runs the "recursive_test".
#                      This takes a lot longer but sometimes finds overlaps
#                      the other misses.
#
# It is recommended to run GeometryCheck while developing the geometry
# as it will find most of the overlaps and to run the recursive_test
# before using the geometry in production as an extra check.
#
#-----------------------------------------------------------------------------------------------------------------------

doFastCheck = True

execfile("ID_only.py")
execfile("geometry.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput="singleMuon_testGeometry_slhc_Hits.pool.root"
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.EvtMax=1

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 13',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -1.1 -2.7',
             'phi: flat  0 6.28318',
             'e: constant 10000']

import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=50000, eta=[-3,3])
topSeq += pg

from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = ["SINGLE 2040160768 443921183"]

if (doFastCheck) :
  def test_preInit():
    print "CALLBACK AT PREINIT"
  def test_postInit():
    print "CALLBACK AT POSTINIT"
  def use_geometry_check():
    print "CALLBACK use_geometry_check"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetGeometryCheck(1000)
  simFlags.InitFunctions.add_function("preInitG4", use_geometry_check)
  simFlags.InitFunctions.add_function("preInit", test_preInit)
  simFlags.InitFunctions.add_function("postInit", test_postInit)

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

MessageSvc = Service("MessageSvc")
MessageSvc.OutputLevel = INFO

#
# Geometry Check
#
from G4AtlasApps import AtlasG4Eng
if (not doFastCheck) :
    AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
    #AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_start 0")
    #AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_depth 2")
    AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursive_test")
