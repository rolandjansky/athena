#
#    Check geometry for oberlaps
#

execfile("ID_only.py")
execfile("geometry.py")

# See https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RecipesG4AtlasApps for options/explanation
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.RandomSvc = 'AtDSFMTGenSvc'
simFlags.SimLayout='ATLAS-P2-ITK-01-00-00_VALIDATION'
#simFlags.SimLayout='ATLAS-SLHC-02-00-00'
simFlags.SimLayout.set_On()
simFlags.EventFilter.set_On()
simFlags.MagneticField.set_Off()
simFlags.RunNumber = 222510 

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolHitsOutput="singleMuon_testGeometry_slhc_Hits.pool.root"
athenaCommonFlags.PoolEvgenInput.set_Off()

import AthenaCommon.AtlasUnixGeneratorJob

import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=50000, eta=[-3,3])

# G4 sensitive volume ID is simulation ID (i.e. select appropriate SCT_G4_SD method)
from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("SLHC_SctSensorSD").GmxSensor=True 

gdmlFile = './Strip.gdml'
import os
if os.access(gdmlFile, os.F_OK):
    os.remove(gdmlFile)

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('VolumeDebugger',['EndOfRun'])
# Configure it:
getPublicTool('VolumeDebugger', tryDefaultConfigurable=True).OutputPath=gdmlFile
getPublicTool('VolumeDebugger').TargetVolume='SCT::SCT'
#getPublicTool('VolumeDebugger').TargetVolume=''
# Resolution is an integer (so don't try 1.e7)
getPublicTool('VolumeDebugger').Resolution=10000 # Default = 1000
getPublicTool('VolumeDebugger').Tolerance=0.
getPublicTool('VolumeDebugger').Verbose=True
getPublicTool('VolumeDebugger').RunGeoTest=True
getPublicTool('VolumeDebugger').MaxCopiesToCheck=10.
getPublicTool('VolumeDebugger').DumpGDML=False


from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
topSeq += pg

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

theApp.EvtMax=1
 
