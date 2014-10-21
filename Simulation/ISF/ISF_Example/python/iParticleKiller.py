# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Gaudi.Configuration import *

class iParticleKiller:
  def __init__(self):
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    from ISF_Services.ISF_ServicesConf import ISF__ParticleKillerSimSvc
    ParticleKiller = ISF__ParticleKillerSimSvc('ParticleKillerSvc')
    ParticleKiller.Identifier           = "ParticleKiller"
    svcMgr += ParticleKiller

    self.__simSvc__ = ParticleKiller


  def getSimSvc(self):
    return self.__simSvc__

