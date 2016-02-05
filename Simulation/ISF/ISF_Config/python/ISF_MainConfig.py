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
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaPython import PyAthena

from ISF_Config.ISF_jobProperties import ISF_Flags

############## Input: GenericGenerator ###############
def getInput_GenericGenerator(name="ISF_Input_GenericGenerator", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('AtRndmGenSvc', simFlags.RandomSvc.get_Value())
    kwargs.setdefault('orders', sorted([]))

    athenaCommonFlags.PoolEvgenInput.set_Off()
    import AthenaCommon.AtlasUnixGeneratorJob
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
    algorithm = ParticleGenerator(name+'PG', **kwargs)
    topSequence += algorithm

    # No longer using this, as it's now set by StackFiller.GenEventManipulators
    #from VertexPositionGenerator.VertexPositionGeneratorConf import VertexPositionGenerator
    #topSequence += VertexPositionGenerator(name+'VPG')
    return algorithm

############## Input: GenericFiles ###############
def getInput_GenericFiles(name="ISF_Input_GenericFiles", **kwargs):
    # Takes input file from athenaCommonFlags.PoolEvgenInput
    import AthenaPoolCnvSvc.ReadAthenaPool
    ServiceMgr.EventPersistencySvc.CnvServices += [ 'AthenaPoolCnvSvc' ]
    ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput.get_Value()
    return PyAthena.Alg(name)

############## Input: MultiParticleTest ###############
def getInput_MultiParticleTest(name="ISF_Input_MultiParticleTest", **kwargs):
    kwargs.setdefault('orders', sorted(['id[0]: sequence -13 13',
                                        'vertX[0]: constant 0.0',
                                        'vertY[0]: constant 0.0',
                                        'vertZ[0]: constant 0.0',
                                        't[0]: constant 0.0',
                                        'eta[0]: flat -2.0 2.0',
                                        'phi[0]: flat  0 6.28318',
                                        'e[0]: constant 50000',
                                        'id[1]: sequence 22',
                                        'vertX[1]: constant 0.0',
                                        'vertY[1]: constant 0.0',
                                        'vertZ[1]: constant 0.0',
                                        't[1]: constant 0.0',
                                        'eta[1]: flat -2.0 2.0',
                                        'phi[1]: flat  0 6.28318',
                                        'e[1]: constant 50000',
                                        'id[2]: sequence -13 13',
                                        'vertX[2]: constant 0.0',
                                        'vertY[2]: constant 0.0',
                                        'vertZ[2]: constant 0.0',
                                        't[2]: constant 0.0',
                                        'eta[2]: flat -2.0 2.0',
                                        'phi[2]: flat  0 6.28318',
                                        'e[2]: constant 50000',
                                        'id[3]: sequence -11 11',
                                        'vertX[3]: constant 0.0',
                                        'vertY[3]: constant 0.0',
                                        'vertZ[3]: constant 0.0',
                                        't[3]: constant 0.0',
                                        'eta[3]: flat -2.0 2.0',
                                        'phi[3]: flat  0 6.28318',
                                        'e[3]: constant 50000',
                                        'id[4]: sequence -211 211',
                                        'vertX[4]: constant 0.0',
                                        'vertY[4]: constant 0.0',
                                        'vertZ[4]: constant 0.0',
                                        't[4]: constant 0.0',
                                        'eta[4]: flat -2.0 2.0',
                                        'phi[4]: flat  0 6.28318',
                                        'e[4]: constant 50000',
                                        ]))
    return getInput_GenericGenerator(name, **kwargs)

############## Input: electrons ###############
def getInput_electrons(name="ISF_Input_electrons", **kwargs):
    kwargs.setdefault('orders', sorted(['pdgcode: sequence 11 -11',
                                        'vertX: constant 0.0',
                                        'vertY: constant 0.0',
                                        'vertZ: constant 0.0',
                                        't: constant 0.0',
                                        'eta: flat -3.0 3.0',
                                        'phi: flat  0 6.28318',
                                        'pt: constant 10000']))
    return getInput_GenericGenerator(name, **kwargs)

############## Input: muons ###############
def getInput_muons(name="ISF_Input_muons", **kwargs):
    kwargs.setdefault('orders', sorted(['pdgcode: sequence 13 -13',
                                        'vertX: constant 0.0',
                                        'vertY: constant 0.0',
                                        'vertZ: constant 0.0',
                                        't: constant 0.0',
                                        'eta: flat -4.0 4.0',
                                        'phi: flat  0 6.28318',
                                        'pt: constant 50000']))
    return getInput_GenericGenerator(name, **kwargs)

############## Input: pions ###############
def getInput_pions(name="ISF_Input_pions", **kwargs):
    kwargs.setdefault('orders', sorted(['pdgcode: sequence 211 -211',
                                        'vertX: constant 0.0',
                                        'vertY: constant 0.0',
                                        'vertZ: constant 0.0',
                                        't: constant 0.0',
                                        'eta: flat -4.0 4.0',
                                        'phi: flat  0 6.28318',
                                        'pt: constant 50000']))
    return getInput_GenericGenerator(name, **kwargs)

############## Input: photons ###############
def getInput_photons(name="ISF_Input_photons", **kwargs):
    kwargs.setdefault('orders', sorted(['pdgcode: fixed 22',
                                        'vertX: constant 0.0',
                                        'vertY: constant 0.0',
                                        'vertZ: constant 0.0',
                                        't: constant 0.0',
                                        'eta: flat -4.0 4.0',
                                        'phi: flat  0 6.28318',
                                        'pt: constant 50000']))
    return getInput_GenericGenerator(name, **kwargs)


############## Input: ttbar ###############
def getInput_ttbar(name="ISF_Input_ttbar", **kwargs):
    athenaCommonFlags.PoolEvgenInput = [ '/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root' ]
    return getInput_GenericFiles(name, **kwargs)

############## Input: minbias ###############
def getInput_minbias(name="ISF_Input_minbias", **kwargs):
    athenaCommonFlags.PoolEvgenInput = [ '/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/minbias_Inelastic-pythia8-7000.evgen.pool.root' ]
    # TODO:
    # BeamEffectTransform will not work properly in ISF, since they'll
    # be appliead to each single G4Event (ie each individual particle
    # that is handed over to G4)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.EventFilter.set_On()
    simFlags.EventFilter.get_Value()['BeamEffectTransformation'] = True
    return getInput_GenericFiles(name, **kwargs)

############## Input: geantinos ###############
def getInput_geantinos(name="ISF_Input_geantinos", **kwargs):
    kwargs.setdefault('orders', sorted(['pdgcode: fixed 999',
                                        'vertX: constant 0.0',
                                        'vertY: constant 0.0',
                                        'vertZ: constant 0.0',
                                        't: constant 0.0',
                                        'eta: flat -5.0 5.0',
                                        'phi: flat  0 6.28318',
                                        'pt: constant 10000']))
    return getInput_GenericGenerator(name, **kwargs)


#   ISF Inputs defined above
#
#
#
#
#
#
#
#
#
#
#
#   ISF Simulators defined below

############## Simulator: GenericSimulator ###############
def getKernel_GenericSimulator(name="ISF_Kernel_GenericSimulator", **kwargs):
    kwargs.setdefault("ParticleBroker"          , "ISF_ParticleBrokerSvc"              ) #ISF_Flags.ParticleBroker.get_Value() )
    kwargs.setdefault("TruthRecordService"      , ISF_Flags.TruthService.get_Value()   )
    kwargs.setdefault("SimHitService"           , "ISF_SimHitService"                  )
    kwargs.setdefault("MemoryMonitoringTool"    , "ISF_MemoryMonitor"                  )
    kwargs.setdefault("DoCPUMonitoring"         , ISF_Flags.DoTimeMonitoring()         )
    kwargs.setdefault("DoMemoryMonitoring"      , ISF_Flags.DoMemoryMonitoring()       )
    kwargs.setdefault("ValidationOutput"        , ISF_Flags.ValidationMode()           )


    from ISF_Algorithms.ISF_AlgorithmsConf import ISF__SimKernel
    SimKernel = ISF__SimKernel(name, **kwargs)

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += SimKernel
    return SimKernel

############## Simulator: MultiSimTest ###############
def getKernel_MultiSimTest(name="ISF_Kernel_MultiSimTest", **kwargs):
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_DefaultGeant4Selector' ]         )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_ElectronGeant4Selector',
                                                        'ISF_MuonGeant4Selector',
                                                        'ISF_DefaultFatrasSelector' ]         )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_MuonGeant4Selector',
                                                        'ISF_WithinEta5FastCaloSimSelector',
                                                        'ISF_DefaultGeant4Selector' ]         )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultGeant4Selector' ]         )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultParticleKillerSelector' ] )

    return getKernel_GenericSimulator(name, **kwargs)


############## Simulator: GenericG4Only ###############
def getKernel_GenericG4Only(name="ISF_Kernel_GenericG4Only", **kwargs):
    kwargs.setdefault("ParticleBroker"              , "ISF_ParticleBrokerSvcNoOrdering"  )
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_FullGeant4Selector' ] )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_FullGeant4Selector' ] )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_FullGeant4Selector' ] )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_FullGeant4Selector' ] )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultParticleKillerSelector' ] )
    return getKernel_GenericSimulator(name, **kwargs)

############## Simulator: FullG4 ###############
def getKernel_FullG4(name="ISF_Kernel_FullG4", **kwargs):
    return getKernel_GenericG4Only(name, **kwargs)

############## Simulator: PassBackG4 ###############
def getKernel_PassBackG4(name="ISF_Kernel_PassBackG4", **kwargs):
    kwargs.setdefault("ParticleBroker"              , "ISF_ParticleBrokerSvcNoOrdering"  )
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_PassBackGeant4Selector' ] )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_PassBackGeant4Selector' ] )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_PassBackGeant4Selector' ] )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_PassBackGeant4Selector' ] )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultParticleKillerSelector' ] )
    return getKernel_GenericSimulator(name, **kwargs)

############## Simulator: CosmicsG4 ###############
def getKernel_CosmicsG4(name="ISF_Kernel_CosmicsG4", **kwargs):
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_FullGeant4Selector' ] )
    kwargs.setdefault("EventFilterTools"            , [ 'ISF_CosmicEventFilter']   )
    return getKernel_FullG4(name, **kwargs)

############## Simulator: FullG4_LongLived ###############
def getKernel_FullG4_LongLived(name="ISF_Kernel_FullG4_LongLived", **kwargs):
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_DefaultLongLivedGeant4Selector' ] )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_DefaultLongLivedGeant4Selector' ] )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_DefaultLongLivedGeant4Selector' ] )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultLongLivedGeant4Selector' ] )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultParticleKillerSelector' ]  )
    kwargs.setdefault("ParticleBroker"              ,   'ISF_LongLivedParticleBrokerSvc'       )
    return getKernel_GenericG4Only(name, **kwargs)

############## Simulator: MC12G4_longLived ###############
def getKernel_MC12G4_longLived(name="ISF_Kernel_MC12G4_longLived", **kwargs):
    return getKernel_FullG4_LongLived(name, **kwargs)

############## Simulator: MC12G4 ###############
def getKernel_MC12G4(name="ISF_Kernel_MC12G4", **kwargs):
    return getKernel_GenericG4Only(name, **kwargs)

############## Simulator: MC12G4_IDOnly ###############
def getKernel_MC12G4_IDOnly(name="ISF_Kernel_MC12G4_IDOnly", **kwargs):
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_DefaultParticleKillerSelector' ] )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultParticleKillerSelector' ] )
    return getKernel_GenericG4Only(name, **kwargs)

############## Simulator: MC12G4_IDCalo ###############
def getKernel_MC12G4_IDCalo(name="ISF_Kernel_MC12G4_IDCalo", **kwargs):
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultParticleKillerSelector' ] )
    return getKernel_GenericG4Only(name, **kwargs)

############## Simulator: ATLFASTII ###############
def getKernel_ATLFASTII(name="ISF_Kernel_ATLFASTII", **kwargs):
    kwargs.setdefault("ParticleBroker"             , 'ISF_AFIIParticleBrokerSvc'                    )

    kwargs.setdefault("BeamPipeSimulationSelectors", [ 'ISF_DefaultAFIIGeant4Selector' ]            )
    kwargs.setdefault("IDSimulationSelectors"      , [ 'ISF_DefaultAFIIGeant4Selector' ]            )
    kwargs.setdefault("CaloSimulationSelectors"    , [ 'ISF_MuonAFIIGeant4Selector',
                                                       'ISF_EtaGreater5ParticleKillerSimSelector',
                                                       'ISF_DefaultLegacyAFIIFastCaloSimSelector' ] )
    kwargs.setdefault("MSSimulationSelectors"      , [ 'ISF_DefaultAFIIGeant4Selector' ]            )
    kwargs.setdefault("CavernSimulationSelectors"  , [ 'ISF_DefaultParticleKillerSelector' ]        )
    # set the simFlags accordingly (TODO: is this even needed?)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "AtlfastII" # TODO: can we rename this to "ATLFASTII" ?
    return getKernel_GenericSimulator(name, **kwargs)

############## Simulator: ATLFASTIIF ###############
def getKernel_ATLFASTIIF(name="ISF_Kernel_ATLFASTIIF", **kwargs):
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_DefaultParticleKillerSelector' ]       )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_DefaultFatrasSelector' ]               )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_MuonFatrasSelector',
                                                        'ISF_EtaGreater5ParticleKillerSimSelector',
                                                        'ISF_DefaultFastCaloSimSelector'  ]         )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultFatrasSelector' ]               )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultParticleKillerSelector' ]       )
    # set the simFlags accordingly (TODO: is this even needed?)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF"
    return getKernel_GenericSimulator(name, **kwargs)

############## Simulator: Fatras_newExtrapolation ###############
def getKernel_Fatras_newExtrapolation(name="ISF_Kernel_Fatras_newExtrapolation", **kwargs):
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    # set the simFlags accordingly (TODO: is this even needed?)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "Fatras_newExtrapolation"
    return getKernel_GenericSimulator(name, **kwargs)

############## Simulator: Fatras_newExtrapolation_IDOnly ###############
def getKernel_Fatras_newExtrapolation_IDOnly(name="ISF_Kernel_Fatras_newExtrapolation_IDOnly", **kwargs):
    kwargs.setdefault("BeamPipeSimulationSelectors" , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    kwargs.setdefault("IDSimulationSelectors"       , [ 'ISF_DefaultFatrasNewExtrapolationSelector' ] )
    kwargs.setdefault("CaloSimulationSelectors"     , [ 'ISF_DefaultParticleKillerSelector' ] )
    kwargs.setdefault("MSSimulationSelectors"       , [ 'ISF_DefaultParticleKillerSelector' ] )
    kwargs.setdefault("CavernSimulationSelectors"   , [ 'ISF_DefaultParticleKillerSelector' ] )
    # set the simFlags accordingly (TODO: is this even needed?)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "Fatras_newExtrapolation_IDOnly"
    return getKernel_GenericSimulator(name, **kwargs)

############## Simulator: ATLFASTIIF fast pileup ###############
def getKernel_ATLFASTIIF_PileUp(name="ISF_Kernel_ATLFASTIIF_PileUp", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_FatrasRandomSelector'),
                                              #getPublicTool('ISF_FatrasTRTPileupSelector'),
                                              getPublicTool('ISF_FatrasPileupSelector'),
                                              getPublicTool('ISF_DefaultFatrasSelector')  ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_MuonFatrasSelector'),
                                              getPublicTool('ISF_EtaGreater5ParticleKillerSimSelector'),
                                              getPublicTool('ISF_FastCaloSimPileupSelector'),
                                              getPublicTool('ISF_DefaultFastCaloSimSelector')  ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    # set the simFlags accordingly
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_PileUp"
    return SimKernel

############## Simulator: ATLFASTIIF_IDOnly ###############
def getKernel_ATLFASTIIF_IDOnly(name="ISF_Kernel_ATLFASTIIF_IDonly", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_DefaultFatrasSelector')         ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    # set the simFlags accordingly (TODO: is this even needed?)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_IDOnly"
    return SimKernel

############## Simulator: ATLFASTIIF_IDCalo ###############
def getKernel_ATLFASTIIF_IDCalo(name="ISF_Kernel_ATLFASTIIF_IDCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_DefaultFatrasSelector')         ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    # set the simFlags accordingly (TODO: is this even needed?)
    from G4AtlasApps.SimFlags import simFlags
    simFlags.SimulationFlavour = "ATLFASTIIF_IDCalo"
    return SimKernel

############## Simulator: FastOnly ###############
def getKernel_FastOnly(name="ISF_Kernel_FastOnly", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_DefaultFastCaloSimSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    return SimKernel

############## Simulator: G4GammaCones ###############
def getKernel_G4GammaCones(name="ISF_Kernel_G4GammaCones", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4GammaCones_FastCalo ###############
def getKernel_G4GammaCones_FastCalo(name="ISF_Kernel_G4GammaCones_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_MuonGeant4Selector'),
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_PhotonConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: FatrasGammaCones_FastCalo ###############
def getKernel_FatrasGammaCones_FastCalo(name="ISF_Kernel_FatrasGammaCones_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_PhotonConeFatrasSelector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_WithinEta5FastCaloSimSelector')   ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4BHadronsOnly_FastCalo ###############
def getKernel_G4BHadronsOnly_FastCalo(name="ISF_Kernel_G4BHadronsOnly_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_BHadronProductsGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = SimKernel.BeamPipeSimulationSelectors
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_MuonGeant4Selector'),
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_BHadronProductsGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4BHadronsOnly_FastCalo ###############
def getKernel_G4BHadrons_FatrasID_FastCalo(name="ISF_Kernel_G4BHadrons_FatrasID_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_BHadronProductsGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = SimKernel.BeamPipeSimulationSelectors
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_MuonGeant4Selector'),
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_MuonGeant4Selector'),
                                                getPublicTool('ISF_BHadronProductsGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel


############## Simulator: G4TauProductsOnly ###############
def getKernel_G4TauProductsOnly(name="ISF_Kernel_G4TauProductsOnly", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_TauProductsGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = SimKernel.BeamPipeSimulationSelectors
    SimKernel.CaloSimulationSelectors     = SimKernel.BeamPipeSimulationSelectors
    SimKernel.MSSimulationSelectors       = SimKernel.BeamPipeSimulationSelectors
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4HiggsLeptonsConeOnly ###############
def getKernel_G4HiggsLeptonsConeOnly(name="ISF_Kernel_G4HiggsLeptonsConeOnly", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicToolClone('ISF_HiggsLeptonsConeGeant4SelectorBeamPipe',
                                                                   'ISF_HiggsLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicToolClone('ISF_HiggsLeptonsConeGeant4SelectorID',
                                                                   'ISF_HiggsLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicToolClone('ISF_HiggsLeptonsConeGeant4SelectorCalo',
                                                                   'ISF_HiggsLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicToolClone('ISF_HiggsLeptonsConeGeant4SelectorMS',
                                                                   'ISF_HiggsLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4HiggsLeptons ###############
def getKernel_G4HiggsLeptons(name="ISF_Kernel_G4HiggsLeptons", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_DefaultGeant4Selector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_DefaultGeant4Selector') ]

    #SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_HiggsLeptonsConeGeant4CaloSelector'),
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_HiggsLeptonsConeGeant4Selector'),
                                              getPublicTool('ISF_DefaultFastCaloSimSelector') ]

    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_DefaultGeant4Selector') ]

    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4WLeptonsCone_FatrasID_FastCalo ###############
def getKernel_G4WLeptonsCone_FatrasID_FastCalo (name="ISF_Kernel_G4WLeptonsCone_Fatras_ID_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_WLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_WLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_WLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_WLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4JPsiLeptonsCone_FatrasID_FastCalo ###############
def getKernel_G4JPsiLeptonsCone_FatrasID_FastCalo (name="ISF_Kernel_G4JPsiLeptonsCone_Fatras_ID_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_JPsiLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_JPsiLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_JPsiLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_JPsiLeptonsConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4ZLeptonsCone_FatrasID_FastCalo ###############
def getKernel_G4ZLeptonsCone_FatrasID_FastCalo (name="ISF_Kernel_G4ZLeptonsCone_FatrasID_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_ZLeptonsPositionConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_ZLeptonsDirectionConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_ZLeptonsPositionConeGeant4Selector') ,
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_SubDetStickyGeant4SimSelector'),
                                                getPublicTool('ISF_ZLeptonsPositionConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel

############## Simulator: G4ZLeptonsConeSticky_FatrasID_FastCalo ###############
def getKernel_G4ZLeptonsConeSticky_FatrasID_FastCalo (name="ISF_Kernel_G4ZLeptonsConeSticky_FatrasID_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    SimKernel.BeamPipeSimulationSelectors = [ getPublicTool('ISF_GlobalStickyGeant4SimSelector'),
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.IDSimulationSelectors       = [ getPublicTool('ISF_GlobalStickyGeant4SimSelector'),
                                                getPublicTool('ISF_ZLeptonsDirectionConeGeant4Selector') ,
                                                getPublicTool('ISF_DefaultFatrasSelector') ]
    SimKernel.CaloSimulationSelectors     = [ getPublicTool('ISF_GlobalStickyGeant4SimSelector'),
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.MSSimulationSelectors       = [ getPublicTool('ISF_GlobalStickyGeant4SimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector') ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector')   ]
    return SimKernel


############## Simulator: G4ZDecayLeptons_FatrasID_FastCalo ###############
def getKernel_G4ZDecayProducts_FatrasID_FastCalo (name="ISF_Kernel_G4ZDecayProducts_FatrasID_FastCalo", **kwargs):
    SimKernel = getKernel_GenericSimulator(name, **kwargs)

    # This selector chain assigns the Z decay products for G4 simulation.
    # The GlobalStickG4 selector ensures that a particle which was inside
    # G4 at one point will always be simulated within G4.
    GlobalStickyG4ZDecayProductsSelector = [ getPublicTool('ISF_GlobalStickyGeant4SimSelector'),
                                             getPublicTool('ISF_ZProductsGeant4Selector')        ]

    SimKernel.BeamPipeSimulationSelectors =   GlobalStickyG4ZDecayProductsSelector + [
                                                getPublicTool('ISF_DefaultParticleKillerSelector')
                                              ]
    SimKernel.IDSimulationSelectors       =   GlobalStickyG4ZDecayProductsSelector + [
                                                getPublicTool('ISF_DefaultFatrasSelector')
                                              ]
    SimKernel.CaloSimulationSelectors     =   GlobalStickyG4ZDecayProductsSelector + [
                                                getPublicTool('ISF_WithinEta5FastCaloSimSelector'),
                                                getPublicTool('ISF_DefaultParticleKillerSelector')
                                              ]
    SimKernel.MSSimulationSelectors       =   GlobalStickyG4ZDecayProductsSelector + [
                                                getPublicTool('ISF_DefaultFatrasSelector')
                                              ]
    SimKernel.CavernSimulationSelectors   = [ getPublicTool('ISF_DefaultParticleKillerSelector') ]
    return SimKernel

# This does not work yet: if the ISF_Flags are set via a preExec it mixes
#       up the initialization order and simulators do not get configured
#       properly!
############### Simulator: FromJobProperties ###############
#def getKernel_FromJobProperties (name="ISF_Kernel_FromJobProperties", **kwargs):
#    SimKernel = getKernel_GenericSimulator(name, **kwargs)
#
#    from ISF_Config.ISF_jobProperties import ISF_Flags
#    SimKernel.BeamPipeSimulationSelectors = ISF_Flags.RoutingChainBeamPipe()
#    SimKernel.IDSimulationSelectors       = ISF_Flags.RoutingChainID()
#    SimKernel.CaloSimulationSelectors     = ISF_Flags.RoutingChainCalo()
#    SimKernel.MSSimulationSelectors       = ISF_Flags.RoutingChainMS()
#    SimKernel.CavernSimulationSelectors   = ISF_Flags.RoutingChainCavern()
#
#    return SimKernel
