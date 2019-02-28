#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import *
from TrigEDMConfig.TriggerEDMRun3 import recordable

def CaloLUMIBCIDToolCfg( flags, name='CaloLumiBCIDToolDefault' ):
    acc = ComponentAccumulator()
    from CaloTools.CaloToolsConf import CaloLumiBCIDTool
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    if flags.Input.isMC == False:
      from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
      theLumiTool = LuminosityToolDefault()
      acc.addPublicTool( theLumiTool )

          
      if flags.Common.isOnline:
          acc.merge(addFolders(flags, ['/LAR/LArPileup/LArPileupShape<key>LArShape32</key>', 
                                       '/LAR/LArPileup/LArPileupAverage'], 'LAR_ONL'))
      else:
          acc.merge(addFolders(flags, ['/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>',
                                       '/LAR/ElecCalibOfl/LArPileupAverage'], 'LAR_OFL'))
      theTool = CaloLumiBCIDTool(name, 
                                 isMC=False, 
                                 LumiTool=theLumiTool, keyShape='LArShape32')

    else:
      from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
      theOFCTool = LArOFCToolDefault()
    
      acc.addPublicTool( theOFCTool )
      from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
      theBunchCrossingTool = BunchCrossingTool()
      
      acc.merge(addFolders(flags, ['/LAR/ElecCalibMC/Shape',
                                   '/LAR/ElecCalibMC/LArPileupAverage'], 'LAR_OFL'))
      theTool = CaloLumiBCIDTool(name,
                                 isMC=True,
                                 LArOFCTool = theOFCTool, BunchCrossingTool = theBunchCrossingTool)
    acc.addPublicTool( theTool )
    
    return acc
            

def TrigCaloDataAccessCfg(flags):

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc                      = ComponentAccumulator()
    acc.merge(CaloLUMIBCIDToolCfg(flags))
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess
    da                       = TrigDataAccess()
    # ??? so we do not need the tools (quick hack)
    da.ApplyOffsetCorrection = False 
    
    # ??? does not realy belong here
    acc.merge(addFolders(flags, ['/LAR/Identifier/OnOffIdMap', '/LAR/Identifier/CalibIdMap', 
                                 '/LAR/Identifier/LArTTCellMapAtlas'], 'LAR'))

    acc.merge(addFolders(flags, ['/CALO/Identifier/CaloTTOnOffIdMapAtlas', '/CALO/Identifier/CaloTTOnAttrIdMapAtlas',
                                       '/CALO/Identifier/CaloTTPpmRxIdMapAtlas'], 'CALO'))
    # ??? should be moved to tile domain
    from TileConditions.TileConditionsConfig import tileCondCfg
    acc.merge( tileCondCfg( flags ) )

    acc.addPublicTool( da )

    # configure trigger tools
    return acc

    



def l2CaloAlgCfg( flags, roisKey="EMCaloRoIs" ):

    #from AthenaCommon.CFElements import parOR, seqOR, seqAND

    from AthenaCommon.Constants import DEBUG
    acc = ComponentAccumulator()

    # load Calo geometry
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg    
    acc.merge(LArGMCfg( flags ))
    acc.merge(TileGMCfg( flags ))
    acc.getService('GeoModelSvc').DetectorTools['TileDetectorTool'].GeometryConfig = 'RECO'

    # ??? likely should be elsewhere
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge( addFolders(flags, ['/LAR/Identifier/FebRodMap'], 'LAR' ))

    acc.merge(TrigCaloDataAccessCfg(flags ))


    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import RingerFexConfig
    ringer = RingerFexConfig('RingsMaker')
    ringer.RingsKey='CaloRings'

    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import EgammaEmEnFexNoTimerConfig, EgammaHadEnFexNoTimerConfig, EgammaSamp1FexNoTimerConfig, EgammaSamp2FexNoTimerConfig, RingerFexConfig

    samp2 = EgammaSamp2FexNoTimerConfig(name='FaAlgoSamp2FexConfig')
    acc.addPublicTool( samp2 )

    samp1 = EgammaSamp1FexNoTimerConfig('FaAlgoSamp1FexConfig')
    acc.addPublicTool( samp1 )

    sampe = EgammaEmEnFexNoTimerConfig('FaAlgoEmEnFexConfig')
    acc.addPublicTool( sampe )

    samph = EgammaHadEnFexNoTimerConfig('FaAlgoHadEnFexConfig')
    acc.addPublicTool( samph )

    ring = RingerFexConfig('RingsMaker')
    ring.OutputLevel=DEBUG
    ring.RingsKey='CaloRings'
    acc.addPublicTool( ring )

    __fex_tools = [ samp2, samp1, sampe, samph, ring ]
    
    from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaFastAlgo
    fastCalo                         = T2CaloEgammaFastAlgo( 'FastEMCaloAlgo' )
    fastCalo.OutputLevel             = DEBUG
    fastCalo.ClustersName            = recordable('L2CaloEMClusters')
    fastCalo.RoIs                    = roisKey
    fastCalo.EtaWidth                = 0.2
    fastCalo.PhiWidth                = 0.2
    # will be replace by the service, which should drive its confing via flags
    # i.e. sth along this line acc.addConfig( TrigCaloDataAccessSvc, flags )
    # for the moment this is done by hand here    
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess    
    accessTool                       = TrigDataAccess()
    accessTool.ApplyOffsetCorrection = False

    acc.addPublicTool(accessTool)
    
    fastCalo.IAlgToolList = __fex_tools
    from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaHitsCalibrationBarrelConfig, EgammaHitsCalibrationEndcapConfig, EgammaGapCalibrationConfig
    from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaTransitionRegionsConfig

    from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaSshapeCalibrationBarrelConfig, EgammaSshapeCalibrationEndcapConfig
    from TrigT2CaloEgamma.TrigT2CaloEgammaMonitoring import TrigT2CaloEgammaValMonitoring, TrigT2CaloEgammaCosmicMonitoring, TrigT2CaloEgammaOnMonitoring, TrigT2CaloEgammaSwValMonitoring, TrigT2CaloEgammaSwOnMonitoring, TrigT2CaloEgammaSwCosMonitoring, TrigT2CaloEgammaTimeMonitoring, TrigT2CaloEgammaAllTimeMonitoring, TrigT2CaloEgammaRingerTimeMonitoring

    __sshape      = EgammaSshapeCalibrationBarrelConfig()
    __hits        = EgammaHitsCalibrationBarrelConfig()
    __gap         = EgammaGapCalibrationConfig()
    __transition  = EgammaTransitionRegionsConfig()
    __barrelTools = [ __sshape, __hits, __gap, __transition ]

    fastCalo.CalibListBarrel = __barrelTools
    #[ acc.addAlgTool( t ) for t in __barrelTools ]                      
    
    __endcapTools = [ EgammaSshapeCalibrationEndcapConfig(),
                      EgammaHitsCalibrationEndcapConfig(),
                      EgammaGapCalibrationConfig() ]

    #[ acc.addAlgTool( t ) for t in __endcapTools ]
    fastCalo.CalibListEndcap= __endcapTools

    return acc, fastCalo

def l2CaloRecoCfg( flags ):
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco

    reco = InViewReco("FastCaloEMReco")
    algAcc, alg = l2CaloAlgCfg( flags, roisKey = reco.name+'RoIs' )
    reco.addRecoAlg( alg )
    reco.merge( algAcc )

    return reco

def l2CaloHypoCfg( flags, name="UNSPECIFIED", CaloClusters="UNSPECIFIED" ):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo                     = TrigL2CaloHypoAlgMT( name )
    l2CaloHypo.CaloClusters        = CaloClusters
    return l2CaloHypo


def generateElectronsCfg( flags ):
    acc = ComponentAccumulator()
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo                     = TrigL2CaloHypoAlgMT( 'L2ElectronCaloHypo' )
    l2CaloHypo.CaloClusters        = 'L2CaloEMClusters'

    l2CaloReco = RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags )
    acc.merge( l2CaloReco )
    
    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromDict )

    fastCaloStep = ChainStep("Electron_step1", [fastCaloSequence])

           
    # # fast ID 
    
    # # EF calo

    # # EF ID
    
    # # offline egamma

    chains = [ Chain(c.split()[0], c.split()[1], [fastCaloStep] )  for c in flags.Trigger.menu.electrons ]    

    return acc, chains # will be more steps here




if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.CFElements import parOR, seqOR, seqAND
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.Trigger.menu.electrons = ["HLT_e5_etcut L1_EM3", "HLT_e10_etcut L1_EM3"]
    ConfigFlags.lock()

    acc = ComponentAccumulator()
    for n in range(10):
        acc.addSequence( parOR("HLTStep_%d"%n ) )

    elAcc, elSeq = generateElectronsCfg( ConfigFlags )
    for n,s in enumerate( elSeq, 1 ):
        stepName = "HLTStep_%d"%n 
        acc.addSequence( s.sequence(), stepName )    
    acc.merge( elAcc )
       
    acc.printConfig()
    acc.store( open("test.pkl", "w") )
    print 'All ok'
