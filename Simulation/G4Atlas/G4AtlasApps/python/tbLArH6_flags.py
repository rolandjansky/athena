# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" Simulation specific flags.

 Apart from the AthenaCommon.DetFlags and AthenaCommon.GlobalFlags
 the simulation jobs will need specific flags that are collected
 here as "simFlags".

 The flags here described are for the the old standalone LAr
 test beams at the H6.

  Note: since revision version 1.10 the simFlags are using a new
 base class (JobProperty) that should add more functionality

"""

__author__= 'Joe Boudreau, Mohsen Kakhzad, Margret Fincke-Keeler, J.P. Archambault, M. Gallas'

from AthenaCommon.JobProperties import JobProperty

# We re-use already defined flags that maybe we will customize here
from G4AtlasApps.SimFlags import jobproperties
from ctb_flags import *  # noqa: F401 F403
# 25/03/2008  A.M.  added  by analogy with atlas_flags.py

jobproperties.SimFlags.CalibrationRun.set_Off()


class LArTB_H1CryoXPos(JobProperty):
    """ Sets the X position for the H1 Cryostat (in the H6 Beamline)

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

class LArTB_H1TableYPos(JobProperty):
    """ Sets the X position for the H1 Cryostat (in the H6 Beamline)

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

class LArTB_H1XSmear(JobProperty):
    """ Sets the X  smearing of the beam in the H6

    """
    statusOn=True
    TypesAllowed=['float']
    StoredValue=0.0

class LArTB_H1YSmear(JobProperty):
    """ Sets the Y smearing of the beam in the H6

    """
    statusOn=True
    TypesAllowed=['float']
    StoredValue=0.0

class LArTB_H6Run1(JobProperty):
    """ Sets the run period for the H6 2004 TB

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    TypesAllowed=['bool']
    StoredValue=False

class LArTB_H6Step(JobProperty):
    """ Sets the Stepping Action for the H6 2004 TB

        It is used by SimCtbKernel before the starting of the simulation.
    """
    statusOn=True
    TypesAllowed=['bool']
    StoredValue=False

class LArTB_H6Emec(JobProperty):
    """ Sets the run period for the H6 2004 TB

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    TypesAllowed=['bool']
    StoredValue=True

class LArTB_H6Hec(JobProperty):
    """ Sets the run period for the H6 2004 TB

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    TypesAllowed=['bool']
    StoredValue=True

class LArTB_H6Fcal(JobProperty):
    """ Sets the run period for the H6 2004 TB

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    TypesAllowed=['bool']
    StoredValue=True

class LArTB_H6Coldnose(JobProperty):
    """ Sets the run period for the H6 2004 TB

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    TypesAllowed=['bool']
    StoredValue=True

class LArTBEC_CryoEtaPos(JobProperty):
    """ Sets the eta position for the LArTBEC

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=12.0

class LArTBEC_CryoPhiPos(JobProperty):
    """ Sets the phi position for the LArTBEC

        It is used by GeoModel before the starting of the simulation.
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=16.0

