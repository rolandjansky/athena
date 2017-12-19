# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Job options file for Geant4 Simulations

import os, shutil
from PyJobTransformsCore.envutil import *
import random, time

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = WARNING

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
#DetFlags.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'g4atlas.hits.pool.root'
athenaCommonFlags.EvtMax = 1

#include("InDetSLHC_Example/preInclude.OverrideBFieldTag.py")

# SLHC Flags -----------------------------------------------------
#from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
#SLHC_Flags.SLHC_Version = ''

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On()
simFlags.SimLayout='ATLAS-P2-ITK-04-01-00_VALIDATION'

# Select the geometry version. 
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-P2-ITK-04-01-00'

## Set global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-SDR-BS14T-IBL-04"

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# --- setup version
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only

SLHC_Flags.SLHC_Version = ""
print  SLHC_Flags

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_Alpine_VF import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#--------------------------------------------------------------
# Particle gun
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

import ParticleGun as PG
print dir(PG)

pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 999     # geantino
pg.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=[-4.,4.], phi=[0,6.28318])
topSeq += pg


myRandomSeed1 = int(random.uniform(0,time.time()))
myRandomSeed2 = int(random.uniform(0,time.time()))
simFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

from RngComps.RngCompsConf import AtRndmGenSvc
myAtRndmGenSvc = AtRndmGenSvc()
myAtRndmGenSvc.Seeds = ["SINGLE "+str(myRandomSeed1)+" "+str(myRandomSeed2) ]
myAtRndmGenSvc.OutputLevel      = VERBOSE
myAtRndmGenSvc.EventReseeding   = False
ServiceMgr += myAtRndmGenSvc

## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

## Use the G4 UI commands via a callback function at level 1
def setup_g4geo():
    from G4AtlasApps import AtlasG4Eng

    AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
    AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_start 0")
    AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_depth 2")
    AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursive_test")

    ## Turn off processes other than transport
    # TODO! Do this using the UI commands
    print "WARNING: the fast physics list is dead! We need to tell this JO to run particle transport only"
#simFlags.InitFunctions.add_function(1, setup_g4geo)

def test_preInit():
    print "CALLBACK AT PREINIT"
def test_postInit():
    print "CALLBACK AT POSTINIT"
def use_geometry_check():
    print "CALLBACK use_geometry_check"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetGeometryCheck(250)
simFlags.InitFunctions.add_function("preInitG4", use_geometry_check)
simFlags.InitFunctions.add_function("preInit", test_preInit)
simFlags.InitFunctions.add_function("postInit", test_postInit)

# ## Exit before instantiation to level 2
# def force_exit():
#     from AthenaCommon.AppMgr import theApp
#     theApp.exit(0)
# simFlags.InitFunctions.add_function(1, force_exit)

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add app to alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
