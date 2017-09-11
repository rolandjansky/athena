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
__version__ = "$Revision: 793038 $"


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

    def __setattr__(self, name, n_value):
        """
        Ensure that _VALIDATION layout tags get through the first level of checking.
        """
        #print name, n_value
        if name == "StoredValue":
            if type(n_value) == int or type (n_value) == float:
                import math
                if abs(n_value) > 1.1:
                    raise ValueError,('THE ETA VALUE MUST BE IN [-1.1,1.1]!!! The selected value %s is not in the range.' %n_value)
        JobProperty.__setattr__(self, name, n_value)


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

    def __setattr__(self, name, n_value):
        """
        Ensure that _VALIDATION layout tags get through the first level of checking.
        """
        #print name, n_value
        if name == "StoredValue":
            if type(n_value) == int or type (n_value) == float:
                import math
                if abs(n_value) > 60. and not (abs(abs(n_value)-90.0) < 0.01) :
                    raise ValueError,('THETA MUST BE IN [-60,60] or +/-90 !!! The selected value %s is not in the range.' %n_value)
        JobProperty.__setattr__(self, name, n_value)


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

    def __setattr__(self, name, n_value):
        """
        Ensure that _VALIDATION layout tags get through the first level of checking.
        """
        #print name, n_value
        if name == "StoredValue":
            if type(n_value) == int or type (n_value) == float:
                import math
                if math.fabs(n_value) > 8.5:
                    raise ValueError,('THE PHI VALUE MUST BE IN [-8.5,8.5]!!! The selected value %s not in range.' %n_value)
        JobProperty.__setattr__(self, name, n_value)


class Y(JobProperty):
    """
    For the Tile standalone TB (2000-2003) Y is an additional table displacement
    in vertical direction used in Y-scans at theta=+/-90
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 0.00

    def __setattr__(self, name, n_value):
        """
        Ensure that _VALIDATION layout tags get through the first level of checking.
        """
        #print name, n_value
        if name == "StoredValue":
            if type(n_value) == int or type (n_value) == float:
                if abs(n_value) > 600:
                    raise ValueError,('THE Y VALUE MUST BE IN [-600,600] !!! The selected value %s is not in the range.' %n_value)
        JobProperty.__setattr__(self, name, n_value)


