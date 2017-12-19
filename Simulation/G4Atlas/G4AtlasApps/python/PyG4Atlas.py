# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Geant4 simulation Python facilities for the ATLAS experiment.

- General-purpose Python classes & functions for the Atlas G4 simulation.
- The attributes, methods and functions which start with "_" are private
  and intended only for internal use.

"""

__author__ = 'A. Dell`Acqua, M. Gallas, A. Buckley'

import os, os.path, string, sys
from time import time
from GaudiPython import PyAlgorithm
from AthenaCommon import Constants, Logging, SystemOfUnits
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Include import include
import cppyy

from G4AtlasApps import AtlasG4Eng

from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
    is_hive = True
else:
    is_hive = False


## In Hive, cannot yet use a python alg due to python GIL.
## So, we use a service for now instead.
from AthenaPython import PyAthena
if is_hive:
    PyG4Atlas_base = PyAthena.Svc
    PyG4Atlas_name = 'PyG4AtlasSvc'
else:
    PyG4Atlas_base = PyAthena.Alg
    PyG4Atlas_name = 'PyG4AtlasAlg'
## Python algorithm to set up the Python side of the G4 sim
## For now, the type of this component depends on the environment.
class _PyG4AtlasComp(PyG4Atlas_base):
    """
    PyAthena class which runs the ATLAS simulation.
    """

    def __init__(self, name=PyG4Atlas_name, sim_module=None):
        """
        name                  --> must be 'PyG4AtlasAlg' (non-hive) or 'PyG4AtlasSvc' (hive)
        sim_module            --> name of a Python module which describes the simulation

        NB. If sim_module is an empty string or None, the sim skeleton contained
        in G4AtlasEng.G4Eng.Dict() will be run.
        """
        super(_PyG4AtlasComp, self).__init__(name=name)
        self.doFirstEventG4SeedsCheck = True #FIXME - remove for G4Hive?
        self.sim_module = sim_module

        ## If the random number service hasn't been set up already, do it now.
        from G4AtlasApps.SimFlags import simFlags
        simFlags.RandomSeedList.useDefaultSeeds()

        ## Set up the appropriate sim skeleton in the G4 engine
        import AtlasG4Eng
        if "ATLAS" in simFlags.SimLayout():
            from SimAtlasKernel import AtlasSimSkeleton
            AtlasG4Eng.G4Eng.log.info('Configuring ATLAS simulations')
            dummy = AtlasSimSkeleton()
        elif "tb_Tile2000_2003" in simFlags.SimLayout():
            from SimCtbKernel import Tile2000_2003
            AtlasG4Eng.G4Eng.log.info('Configuring old standalone Tile test beams')
            dummy = Tile2000_2003()
        elif "tb_LArH6" in simFlags.SimLayout():
            AtlasG4Eng.G4Eng.log.info('Configuring LAr H6 test beam')
            from SimCtbKernel import LArH6_TB
            dummy = LArH6_TB()
        elif "ctbh8" in simFlags.SimLayout():
            from SimCtbKernel import CtbSim
            AtlasG4Eng.G4Eng.log.info('Configuring CTB H8 (2004) test beam')
            dummy = CtbSim()
        elif "simu_skeleton" not in AtlasG4Eng.G4Eng.Dict:
            AtlasG4Eng.G4Eng.log.error('No sim skeleton registered by time of %s construction: STOP!!' % self.name())
            raise ValueError('Unknown sim setup: STOP')

        ## Import sim module if requested
        # TODO: is this ever used?
        if self.sim_module:
            AtlasG4Eng.G4Eng.log.info("The kernel simulation Python module which describes the simulation is: %s" % self.sim_module)
            try:
                __import__(self.sim_module, globals(), locals())
            except:
                AtlasG4Eng.G4Eng.log.fatal("The kernel simulation Python module '%s' was not found!" % self.sim_module)
                raise RuntimeError('PyG4Atlas: %s: initialize()' % self.name())

        ## Call the sim skeleton pre-init method
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_PreInit()

        ## Start profiler
        AtlasG4Eng.G4Eng._app_profiler('PyG4AtlasAlg before initialize')


    def initialize(self):
        import AtlasG4Eng
        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        AtlasG4Eng.G4Eng.log.info('%s starting at (UTC): %s' % (self.name(), timestr))
        AtlasG4Eng.G4Eng._app_profiler('%s begin of initialize' % self.name())
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_All()
        AtlasG4Eng.G4Eng._init_Simulation()

        from G4AtlasApps.SimFlags import simFlags
        AtlasG4Eng.G4Eng._app_profiler('%s end of initialize' % self.name())

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.EvtMax.statusOn and theApp.EvtMax == -1:
            theApp.EvtMax = athenaCommonFlags.EvtMax()
        return True

    def finalize(self):
        import AtlasG4Eng
        AtlasG4Eng.G4Eng._app_profiler('%s at finalize  ' % self.name())
        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        AtlasG4Eng.G4Eng.log.info('%s ending at (UTC): %s'  % (self.name(), timestr))
        return True

if is_hive:
    PyG4AtlasSvc = _PyG4AtlasComp
else:
    PyG4AtlasAlg = _PyG4AtlasComp
