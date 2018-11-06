#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import *


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
            
def TileCondCfg( flags ):

    acc = ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    def __addFolder(f):        
        acc.merge( addFolders( flags, '%s <key>%s</key>' %(f,f),   'TILE_OFL' if '/OFL' in f else 'TILE_ONL',  className="CondAttrListCollection") )

    # from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerFlt_ as TileCondProxyFileFlt
    # from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerBch_ as TileCondProxyFileBch
    from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerFlt_ as TileCondProxyCoolFlt
    from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerBch_ as TileCondProxyCoolBch
    # from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerOfc_ as TileCondProxyCoolOfc

    emScale = 'TileEMScale'
    from TileConditions.TileConditionsConf import TileEMScaleCondAlg
    emScaleCondAlg = TileEMScaleCondAlg(name = emScale + 'CondAlg', TileEMScale = emScale)
    emScaleCondAlg.OnlCacheUnit      = 'OnlineMegaElectronVolts'
    emScaleCondAlg.OflCesProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OflCes',    Source = '/TILE/OFL02/CALIB/CES') 
    emScaleCondAlg.OflCisLinProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflCisLin', Source = '/TILE/OFL02/CALIB/CIS/LIN') 
    emScaleCondAlg.OflCisNlnProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflCisNln', Source = '/TILE/OFL02/CALIB/CIS/NLN') 
    emScaleCondAlg.OflEmsProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OflEms',    Source = '/TILE/OFL02/CALIB/EMS') 
    emScaleCondAlg.OflLasFibProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasFib', Source = '/TILE/OFL02/CALIB/LAS/FIBER') 
    emScaleCondAlg.OflLasLinProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasLin', Source = '/TILE/OFL02/CALIB/LAS/LIN') 
    emScaleCondAlg.OflLasNlnProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasNln', Source = '/TILE/OFL02/CALIB/LAS/NLN') 
    emScaleCondAlg.OnlCesProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCes',    Source = '/TILE/ONL01/CALIB/CES') 
    emScaleCondAlg.OnlCisProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCis',    Source = '/TILE/ONL01/CALIB/CIS/LIN') 
    emScaleCondAlg.OnlEmsProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlEms',    Source = '/TILE/ONL01/CALIB/EMS') 
    emScaleCondAlg.OnlLasProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlLas',    Source = '/TILE/ONL01/CALIB/LAS/LIN')

    __addFolder( '/TILE/OFL02/CALIB/CES')       
    __addFolder( '/TILE/OFL02/CALIB/CIS/LIN')   
    __addFolder( '/TILE/OFL02/CALIB/CIS/NLN')   
    __addFolder( '/TILE/OFL02/CALIB/EMS')       
    __addFolder( '/TILE/OFL02/CALIB/LAS/FIBER') 
    __addFolder( '/TILE/OFL02/CALIB/LAS/LIN')   
    __addFolder( '/TILE/OFL02/CALIB/LAS/NLN')   
    __addFolder( '/TILE/ONL01/CALIB/CES')       
    __addFolder( '/TILE/ONL01/CALIB/CIS/LIN')   
    __addFolder( '/TILE/ONL01/CALIB/EMS')       
    __addFolder( '/TILE/ONL01/CALIB/LAS/LIN')   

    acc.addCondAlgo( emScaleCondAlg )

    from TileConditions.TileConditionsConf import TileCondToolEmscale
    emScaleTool = TileCondToolEmscale(name = 'TileCondToolEmscale', TileEMScale = emScale)
    acc.addPublicTool( emScaleTool )


    from TileConditions.TileConditionsConf import TileCalibCondAlg_TileCalibDrawerFlt_ as TileCalibFltCondAlg
    def __addCondAlg(calibData, proxy):
        calibCondAlg = calibData + 'CondAlg'
        condAlg = TileCalibFltCondAlg( name = calibCondAlg, ConditionsProxy = proxy, TileCalibData = calibData)
        acc.addCondAlgo( condAlg )


    sampleNoise = 'TileSampleNoise'
    sampleNoiseProxy = TileCondProxyCoolFlt( 'TileCondProxyCool_NoiseSample', Source = '/TILE/OFL02/NOISE/SAMPLE' )
    __addFolder( '/TILE/OFL02/NOISE/SAMPLE' )
    __addCondAlg(sampleNoise, sampleNoiseProxy)

    onlineSampleNoise = 'TileOnlineSampleNoise'
    onlineSampleNoiseProxy = TileCondProxyCoolFlt( 'TileCondProxyCool_OnlineNoiseSample', Source = '/TILE/ONL01/NOISE/SAMPLE' )
    __addFolder( '/TILE/ONL01/NOISE/SAMPLE' )    
    __addCondAlg(onlineSampleNoise, onlineSampleNoiseProxy)

    from TileConditions.TileConditionsConf import TileCondToolNoiseSample
    noiseSampleTool = TileCondToolNoiseSample(name = 'TileCondToolNoiseSample', 
                                              TileSampleNoise = sampleNoise, 
                                              TileOnlineSampleNoise = onlineSampleNoise)
    acc.addPublicTool( noiseSampleTool )


    timing = 'TileTiming'
    timingProxy = TileCondProxyCoolFlt('TileCondProxyCool_AdcOffset', Source = '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    __addFolder( '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    __addCondAlg(timing, timingProxy)

    from TileConditions.TileConditionsConf import TileCondToolTiming
    timingTool = TileCondToolTiming(name = 'TileCondToolTiming', TileTiming = timing)
    acc.addPublicTool( timingTool )


    badChannels = 'TileBadChannels'
    from TileConditions.TileConditionsConf import TileBadChannelsCondAlg
    badChannelsCondAlg = TileBadChannelsCondAlg( name = badChannels + 'CondAlg', TileBadChannels = badChannels)
    badChannelsCondAlg.OflBchProxy = TileCondProxyCoolBch('TileCondProxyCool_OflBch', Source = '/TILE/OFL02/STATUS/ADC' )
    badChannelsCondAlg.OnlBchProxy = TileCondProxyCoolBch('TileCondProxyCool_OnlBch', Source = '/TILE/ONL01/STATUS/ADC' )
    __addFolder( '/TILE/OFL02/STATUS/ADC' )
    __addFolder( '/TILE/ONL01/STATUS/ADC' )
    acc.addCondAlgo( badChannelsCondAlg )

    from TileConditions.TileConditionsConf import TileBadChanTool
    badChanTool = TileBadChanTool(name = 'TileBadChanTool', TileBadChannels = badChannels)
    acc.addPublicTool( badChanTool )

    from TileConditions.TileConditionsConf import TileBadChanLegacyTool
    from AthenaCommon.Constants import DEBUG
    badChanLegacyTool = TileBadChanLegacyTool( 'TileBadChanLegacyTool' )
    badChanLegacyTool.ProxyOflBch = TileCondProxyCoolBch('TileCondProxyCool_OflBchLegacy', Source = '/TILE/OFL02/STATUS/ADC' )
    badChanLegacyTool.ProxyOnlBch = TileCondProxyCoolBch('TileCondProxyCool_OnlBchLegacy', Source = '/TILE/ONL01/STATUS/ADC', OutputLevel=DEBUG )    
    acc.addPublicTool( badChanLegacyTool )



    acc.merge( addFolders(flags, ['/LAR/BadChannelsOfl/BadChannels <key>/LAR/BadChannels/BadChannels</key>', 
                                  '/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>'], 'LAR_OFL'))
    

    from TileConditions.TileConditionsConf import TileInfoLoader, TileCablingSvc
    infoLoaderSvc = TileInfoLoader()
    infoLoaderSvc.NoiseScaleIndex= 2
    acc.addService( infoLoaderSvc ) 
#    acc.addToAppProperty('CreateSvc', [infoLoaderSvc.getFullName()] )


    cabligSvc = TileCablingSvc()
    cabligSvc.CablingType = 4
    acc.addService( cabligSvc )

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
    
    acc.merge( TileCondCfg( flags ) )

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
    fastCalo.ClustersName            = 'L2CaloEMClusters'
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



def generateElectronsCfg( flags ):
    acc = ComponentAccumulator()
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo                     = TrigL2CaloHypoAlgMT( 'L2ElectronCaloHypo' )
    l2CaloHypo.CaloClusters        = 'L2CaloEMClusters'

    l2CaloReco = RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags )
    acc.merge( l2CaloReco )
    
    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromName )

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
