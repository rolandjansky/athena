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

    # let the G4 configuration know that is an ISF run!
    simFlags.ISFRun.set_Value_and_Lock( True)
    # call the standard G4 config
    # ADS: these lines must be called somewhere else. Can't have the alg added to the sequence in an unpredictable order 
    #from AthenaCommon.AlgSequence import AlgSequence
    #topSequence = AlgSequence()
    #from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
    #topSequence += PyG4AtlasAlg()


  def getSimSvc(self):
    return self.__simSvc__
