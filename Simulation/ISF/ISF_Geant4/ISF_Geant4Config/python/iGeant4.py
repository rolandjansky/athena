# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Gaudi.Configuration import *
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties

# confgetter
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

G4Eng = None

class iGeant4:
  def __init__(self, **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    global UseNewG4Configuration
    UseNewG4Configuration =  kwargs.pop('UseNewConfiguration', False)

    ####################################
    ##
    ## the Athena part:
    ##
    ####################################

    ##FIXME: This should be configured using a CfgGetter method
    ##defined in the SubDetectorEnvelopes package.
    ##G4EnvelopeDefSvc is only used in G4DetectorEnvelopes/src/DefSvcEnvelope.cxx
    from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
    G4_DDDBEnvelopeDefSvc = DetDescrDBEnvelopeSvc( "G4EnvelopeDefSvc" )
    G4_DDDBEnvelopeDefSvc.DBBeamPipeNode = 'BPipeEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBInDetNode    = 'InDetEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBCaloNode     = 'CaloEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBMSNode       = 'MuonEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBCavernNode   = 'CavernEnvelopeG4'
    svcMgr += G4_DDDBEnvelopeDefSvc

    # configure Geant4SimSvc
    from ISF_Geant4Services.ISF_Geant4ServicesConf import iGeant4__Geant4SimSvc
    Geant4SimService = iGeant4__Geant4SimSvc('ISFG4SimSvc', **kwargs)

    # DEBUG: dump Geant4SimService
    print Geant4SimService

    svcMgr += Geant4SimService
    self.__simSvc__ = Geant4SimService

    ##FIXME: ISF_ParticleFinalStateFilter is configured in
    ##ISF_HepMC_ToolsConfig.py and is only used by ISF_StackFiller
    ##which is also configured in ISF_HepMC_ToolsConfig.py.
    finalStateFilter = getPublicTool("ISF_ParticleFinalStateFilter")
    finalStateFilter.CheckGenSimStable = False
    finalStateFilter.CheckGenInteracting = False

    ####################################
    ##
    ## the G4 Python part:
    ##
    ####################################

    ##
    ## choose between new ISF G4 Python Configuration...
    ##   (work in progress)
    if UseNewG4Configuration:
      ## Set up the appropriate sim skeleton in the G4 engine
      #from ISF_Geant4Config.ISF_AtlasSimSkeleton import ISF_AtlasSimSkeleton
      #dummy = ISF_AtlasSimSkeleton( self.G4Eng )
      ## Call the sim skeleton pre-init method
      #self.G4Eng.Dict['simu_skeleton']._do_PreInit()
      # the _do_PreInit steps written out:
      # ELLI begin: set a freakin' high log level
      #self.G4Eng.log.setLevel(10*(VERBOSE - 1));
      # ELLI end
      from ISF_Geant4Config.G4AtlasEngine import G4AtlasEngine
      self.G4Eng = G4AtlasEngine()
      global G4Eng
      G4Eng = self.G4Eng

      # The Geant4 Python config tool (to be replaced by C++ soon!)
      from ISF_Geant4Tools.ISF_G4ConfigTool import ISF_G4ConfigTool
      G4ConfigTool             = ISF_G4ConfigTool('ISF_G4ConfigTool')
      G4ConfigTool.G4Eng       = self.G4Eng
      ToolSvc += G4ConfigTool
    ##
    ## ...or standard G4 Configuration from G4AtlasApps
    ##
    else :
      # let the G4 configuration know that is an ISF run!
      simFlags.ISFRun.set_Value_and_Lock( True)
      # call the standard G4 config
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
      topSequence += PyG4AtlasAlg()


  def getSimSvc(self):
    return self.__simSvc__
