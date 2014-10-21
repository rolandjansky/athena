# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved


cache = {}

def usingParticleKiller():
    global cache
    return bool(cache)

def getIParticleKiller():
    global cache
    if 'iParticleKiller' in cache:
        i = cache['iParticleKiller']
    else:
        from iParticleKiller import iParticleKiller
        i = iParticleKiller()
        cache['iParticleKiller'] = i
    return i

def getParticleKillerSvc(name="ISF_ParticleKillerSvc", **kwargs):
    return getIParticleKiller().getSimSvc()
