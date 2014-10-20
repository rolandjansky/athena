# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Gaudi.Configuration import *
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties

# confgetter
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

G4Eng = None

class iGeant4:
  def __init__(self, ParticleService, TruthService, SimHitService, GeoIDSvc, UseNewConfiguration, PrintTimingInfo, FullGeant4 ):
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    global UseNewG4Configuration
    UseNewG4Configuration = UseNewConfiguration

    ####################################
    ##
    ## the Athena part:
    ##
    ####################################

    from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
    G4_DDDBEnvelopeDefSvc = DetDescrDBEnvelopeSvc( "G4EnvelopeDefSvc" )
    G4_DDDBEnvelopeDefSvc.DBBeamPipeNode = 'BPipeEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBInDetNode    = 'InDetEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBCaloNode     = 'CaloEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBMSNode       = 'MuonEnvelopeG4'
    G4_DDDBEnvelopeDefSvc.DBCavernNode   = 'CavernEnvelopeG4'
    svcMgr += G4_DDDBEnvelopeDefSvc

    # explicit initialisation of iGeant4::PhysicsValidationUserAction
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__PhysicsValidationUserAction
    G4PhysicsValidationUserAction                = iGeant4__PhysicsValidationUserAction('ISFG4PhysicsValidationUserAction')
    G4PhysicsValidationUserAction.ParticleBroker    = ParticleService
    ToolSvc += G4PhysicsValidationUserAction

    # explicit initialisation of iGeant4::TrackProcessorUserAction
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__TrackProcessorUserAction,iGeant4__SDActivateUserAction
    G4TrackProcessorUserAction                   = iGeant4__TrackProcessorUserAction('ISFG4TrackProcessorUserAction')
    G4TrackProcessorUserAction.ParticleBroker    = ParticleService
    G4TrackProcessorUserAction.GeoIDSvc          = GeoIDSvc

    G4TrackProcessorUserAction.Geant4OnlyMode    = FullGeant4

    if jobproperties.Beam.beamType() == 'cosmics' or \
       (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
      G4TrackProcessorUserAction.TruthVolumeLevel = 2

    ToolSvc += G4TrackProcessorUserAction


    G4SDActivateUserAction = iGeant4__SDActivateUserAction('ISFSDActivateUserAction')
    ToolSvc += G4SDActivateUserAction

    # explicit initialisation of iGeant4::MCTruthUserAction 
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__MCTruthUserAction 
    ISFMCTruthUserAction = iGeant4__MCTruthUserAction('ISFMCTruthUserAction') 
    ISFMCTruthUserAction.TruthRecordSvc = TruthService 
    ToolSvc += ISFMCTruthUserAction 

    # explicit initialisation of iGeant4::G4TransportTool
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__G4TransportTool
    Geant4TransportTool = iGeant4__G4TransportTool(
                               'ISFG4TransportTool',
                               RandomGenerator = 'athena',
                               RandomNumberService = simFlags.RandomSvc(),
                               PhysicsValidationUserAction = G4PhysicsValidationUserAction,
                               TrackProcessorUserAction = G4TrackProcessorUserAction,
                               MCTruthUserAction = ISFMCTruthUserAction,
                               SDActivateUserAction = G4SDActivateUserAction )
    ToolSvc += Geant4TransportTool

    # configure Geant4SimSvc
    from ISF_Geant4Services.ISF_Geant4ServicesConf import iGeant4__Geant4SimSvc
    Geant4SimService = iGeant4__Geant4SimSvc('ISFG4SimSvc')
    Geant4SimService.Identifier      = "Geant4"
    Geant4SimService.PrintTimingInfo = PrintTimingInfo
    if UseNewConfiguration:
      Geant4SimService.G4ConfigTool    = G4ConfigTool

    # DEBUG: dump Geant4SimService
    print Geant4SimService

    Geant4SimService.SimulationTool       = Geant4TransportTool
    svcMgr += Geant4SimService
    self.__simSvc__ = Geant4SimService

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
    if UseNewConfiguration:
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

