# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Job options file for Geant4 ATLAS detector simulations

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags

## Simulation flags
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()

## Layout tags: see SimFlags.SimLayout for allowed values
## Use the default layout:
SimFlags.SimLayout.set_On()
## Set a specific layout tag:
SimFlags.SimLayout = 'ATLAS-GEO-10-00-00'
#SimFlags.SimLayout = 'ATLAS-GEO-16-00-00'
## Set a specific non-officially-supported layout tag using the _VALIDATION suffix:
#SimFlags.SimLayout = 'ATLAS-GEO-16-00-00_VALIDATION'

#MC conditions / alignments
globalflags.ConditionsTag='OFLCOND-SIM-BS900-00'

#Data conditions / alignments
#globalflags.DataSource.set_Value_and_Lock('data') #force to use the data database
#globalflags.ConditionsTag='COMCOND-ES1PS-001-00'
#SimFlags.RunNumber.set_Value_and_Lock(152845)
#SimFlags.RunNumber.set_Value_and_Lock(142383)

#Custom alignments
#from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/Indet/Align','InDetAlign_CSC_00')
#conddb.addOverride('/Indet/Align','InDetAlign_Collision_2009_08')
#conddb.addOverride('/Indet/Align','InDetAlign_Collision_7T_2010_07')
#conddb.addOverride('/TRT/Align','TRTAlign_Collision_2009_04')

## Set the EtaPhi, VertexSpread and VertexRange checks on
SimFlags.EventFilter.set_On()

## Set the LAr parameterization
#SimFlags.LArParameterization = 2

## No magnetic field
#SimFlags.MagneticField.set_Off()

## Register callback functions at various init levels
def test_preInit():
    print "CALLBACK AT PREINIT"
def test_postInit():
    print "CALLBACK AT POSTINIT"
def use_geometry_check():
    print "CALLBACK use_geometry_check"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetGeometryCheck(10)
SimFlags.InitFunctions.add_function("preInitG4", use_geometry_check)
SimFlags.InitFunctions.add_function("preInit", test_preInit)
SimFlags.InitFunctions.add_function("postInit", test_postInit)
## Change the field stepper and use verbose G4 tracking
#from G4AtlasApps import callbacks
#SimFlags.InitFunctions.add_function("postInit", callbacks.use_nystromrk4_stepper)
#SimFlags.InitFunctions.add_function("postInit", callbacks.use_verbose_tracking)

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"
athenaCommonFlags.EvtMax = 1

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add G4 sim framework alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
