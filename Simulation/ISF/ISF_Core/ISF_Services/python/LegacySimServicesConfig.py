# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Configurations for (potentially) thread-unsafe ISF Sim Services
"""

from AthenaCommon import CfgMgr

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *

def getParticleKillerSvc(name="ISF_ParticleKillerSvc", **kwargs):
    kwargs.setdefault('Identifier',           "ParticleKiller")
    kwargs.setdefault('SimulatorTool',        "ISF_ParticleKillerTool")
    return CfgMgr.ISF__LegacySimSvc(name, **kwargs )

