"""
Job options file for Geant4 Simulations

Example for the simplest simulation entity
"""

__author__ = "M. Gallas"


## Common flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax = 3
ServiceMgr.MessageSvc.OutputLevel = WARNING

from AthenaCommon.JobProperties import JobProperty
from G4AtlasApps import simFlags, PyG4Atlas, AtlasG4Eng, SimSkeleton

## SimLayout
simFlags.SimLayout = 'mytest_tb_VALIDATION'

## Particle generator steering
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 11',
             'vertX: constant -2750.0',
             'vertY: flat -10.0 15.0',
             'vertZ: flat -15.0 15.0',
             't: constant -2750.0',
             'energy: constant 10000',
             'momX: fixed 1',
             'momY: fixed 0',
             'momY: fixed 0']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
job += ParticleGenerator()
job.ParticleGenerator.orders = sorted(spgorders)
job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()


## Describe the simulation entity using the skeleton
class MySimSkeleton(SimSkeleton.SimSkeleton):
    """
    My own simulation entity skeleton based on the base class SimSkeleton from
    G4AtlasApps.PyG4Atlas
    """
    def __init__(self):
        if not AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton'):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton'] = self
            AtlasG4Eng.G4Eng.Name = "TestStandalone"
        else:
            AtlasG4Eng.G4Eng.log.warning('SimSkeleton: simulation already has a skeleton. '
                                         'You can find it in the G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def do_GeoSD(self):
        """ Describes the geometry and sensitive detectors.
        """
        ## World volume
        worldvolume = PyG4Atlas.DetFacility("ResizeableBox","WorldVolume")
        worldvolume.df.SetDx(5000.) #  half dimension in mm
        worldvolume.df.SetDy(5000.)
        worldvolume.df.SetDz(5000.)
        AtlasG4Eng.G4Eng.add_DetFacility(worldvolume,worldvolume)
        ## Add 2 scintillators: S1 and S2
        s1 = PyG4Atlas.DetFacility("ResizeableBox","S1")
        s2 = PyG4Atlas.DetFacility("ResizeableBox","S2")
        ## + dimensions
        s1.df.SetDx(10.)
        s1.df.SetDy(500.)
        s1.df.SetDz(500.)
        s2.df.SetDx(10.)
        s2.df.SetDy(200.)
        s2.df.SetDz(200.)
        ## + material
        from G4AtlasApps import atlas_materials
        s1.df.SetMaterial("Scintillator")
        s2.df.SetMaterial("Scintillator")
        ## + position
        s1.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-20.0,0.0,0.0))
        s2.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(+100.0,10.0,0.0))
        s2.df.RotateX(0.7854) # will be also rotated
        ## Adding scintillators to the G4Eng
        AtlasG4Eng.G4Eng.add_DetFacility(s1,worldvolume)
        AtlasG4Eng.G4Eng.add_DetFacility(s2,worldvolume)
        ## Adding a generic scintillator SD
        from G4AtlasApps.atlas_utilities import ScintillatorSD
        GenScintSD = ScintillatorSD(['S1','S2'])
        ## A block of iron
        iron = PyG4Atlas.DetFacility("ResizeableBox", "IRON")
        iron.df.SetDx(100.)
        iron.df.SetDy(100.)
        iron.df.SetDz(100.)
        iron.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(+1000.0,0.0,0.0))
        iron.df.SetMaterial("Iron")
        AtlasG4Eng.G4Eng.add_DetFacility(iron, worldvolume)

    @classmethod
    def do_Physics(self):
        """
        Physics list from the simFlags.PhysicsList flag with default cut of 1mm.
        """
        atlasphysics = PyG4Atlas.PhysicsList(simFlags.PhysicsList.get_Value(), 1)
        AtlasG4Eng.G4Eng.add_PhysicsList(atlasphysics)

    @classmethod
    def do_mytest(self):
        """A non standard method that will be executed automatically."""
        simFlags.print_JobProperties('tree&value')
        # Set the default verbosity of the PyG4Atlas machinery
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng.log.setLevel(INFO)


## Create an instance of the simulation entity skeleton
myTest = MySimSkeleton()


## Register callback functions in the sim init process:

## Set the G4 verbosity level
#simFlags.G4Commands += ['/tracking/verbose 1']

## Change the world volume y range
def change_worldvolume_y():
    myWorld = AtlasG4Eng.G4Eng.Dict_DetFacility.get('WorldVolume')
    myWorld.df.SetDy(2000)
    print 'Setting Y coordinate of the WorldVolume to 2000'
simFlags.InitFunctions.add_function(1, change_worldvolume_y)


include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Insert the PyG4Atlas algorithm at the head of the alg sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
