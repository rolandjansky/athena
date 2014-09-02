# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
SimFlags specific to the old standalone Tile test beams

Apart from the AthenaCommon.DetFlags and AthenaCommon.GlobalFlags
the simulation jobs will need specific flags that are collected
here as "SimFlags".

The flags here described are for the old standalone Tile test beams
during the years 2000-2003.
"""

__author__= 'M. Gallas'
__version__ = "$Revision: 449236 $"


from AthenaCommon.JobProperties import JobProperty

## Remove and replace the core SimFlags version of CalibrationRun
import SimFlags
from G4AtlasApps.SimFlags import jobproperties
jobproperties.SimFlags.del_JobProperty(SimFlags.CalibrationRun)
del SimFlags


class CalibrationRun(JobProperty):
    """
    Sets the calibration runs for Tile detector.
    """
    statusOn = False
    allowedTypes = ['str']
    allowedValues = ['','Tile']
    StoredValue = ''


class Eta(JobProperty):
    """
    Eta value.

    In the test beam simulation Eta means that the Tile table was rotated in Y
    axis and shifted in X by:

    ThetaY=-(pi/2)+2*atan(math.exp(-Eta))
    DeltaX=(2298-6208)/cosh(Eta)+6208

    - The default status is False and Value=0.35
    - Values from -1.1 to 1.1
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 0.35


class Theta(JobProperty):
    """
    For the Tile standalone TB (2000-2003) the position & rotation of the
    modules can be set using "eta" or using the par: (Theta,Z). The two
    possibilities are exclusive.

    Negative theta corresponds to positive eta values
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 0.00


class Z(JobProperty):
    """
    For the Tile standalone TB (2000-2003) the position & rotation of the
    modules can be set using "eta" or using the par: (Theta,Z). You have to use
    Eta or (Theta,Z), but not both.

    The Z coordinate is calculated along the front face of the module at R =
    2290: it is the distance from the center of the modules to the desired
    impact point. A positive value -> impact point at positive eta side.
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 0.00


class Phi(JobProperty):
    """
    For the Tile standalone TB (2000-2003) Phi is an additional rotation around
    Z axis used in so-called phi-scans at fixed theta<90 and also phi can be set
    to +/-5.625 degrees and in such a case beam is entering in up/down module
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 0.00


class Y(JobProperty):
    """
    For the Tile standalone TB (2000-2003) Y is an additional table displacement
    in vertical direction used in Y-scans at theta=+/-90
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 0.00
