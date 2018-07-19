#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import *


def CaloLUMIBCIDTool( flags, name='CaloLumiBCIDToolDefault' ):
    acc = ComponentAccumulator()
    from CaloTools.CaloToolsConf import CaloLumiBCIDTool
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    if flags.get('global.isMC') == False:
      from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
      theLumiTool = LuminosityToolDefault()
      acc.addPublicTool( theLumiTool )

          
      if flags.get('global.isOnline'):
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
            
def TileCond( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TileConditions.TileConditionsConf import TileCondToolEmscale
    tool = TileCondToolEmscale()
    tool.OnlCacheUnit      = 'OnlineMegaElectronVolts'
    # from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerFlt_ as TileCondProxyFileFlt
    # from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerBch_ as TileCondProxyFileBch
    from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerFlt_ as TileCondProxyCoolFlt
    from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerBch_ as TileCondProxyCoolBch
    # from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerOfc_ as TileCondProxyCoolOfc

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    def __addFolder(f):        
        acc.merge(addFolders(flags, '%s <key>%s</key>' %(f,f),   'TILE_OFL' if '/OFL' in f else 'TILE_ONL'))

    tool.ProxyOflCes    = TileCondProxyCoolFlt('TileCondProxyCool_OflCes',    Source = '/TILE/OFL02/CALIB/CES') 
    tool.ProxyOflCisLin = TileCondProxyCoolFlt('TileCondProxyCool_OflCisLin', Source = '/TILE/OFL02/CALIB/CIS/LIN') 
    tool.ProxyOflCisNln = TileCondProxyCoolFlt('TileCondProxyCool_OflCisNln', Source = '/TILE/OFL02/CALIB/CIS/NLN') 
    tool.ProxyOflEms    = TileCondProxyCoolFlt('TileCondProxyCool_OflEms',    Source = '/TILE/OFL02/CALIB/EMS') 
    tool.ProxyOflLasFib = TileCondProxyCoolFlt('TileCondProxyCool_OflLasFib', Source = '/TILE/OFL02/CALIB/LAS/FIBER') 
    tool.ProxyOflLasLin = TileCondProxyCoolFlt('TileCondProxyCool_OflLasLin', Source = '/TILE/OFL02/CALIB/LAS/LIN') 
    tool.ProxyOflLasNln = TileCondProxyCoolFlt('TileCondProxyCool_OflLasNln', Source = '/TILE/OFL02/CALIB/LAS/NLN') 
    tool.ProxyOnlCes    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCes',    Source = '/TILE/ONL01/CALIB/CES') 
    tool.ProxyOnlCis    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCis',    Source = '/TILE/ONL01/CALIB/CIS/LIN') 
    tool.ProxyOnlEms    = TileCondProxyCoolFlt('TileCondProxyCool_OnlEms',    Source = '/TILE/ONL01/CALIB/EMS') 
    tool.ProxyOnlLas    = TileCondProxyCoolFlt('TileCondProxyCool_OnlLas',    Source = '/TILE/ONL01/CALIB/LAS/LIN')

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
    acc.addPublicTool( tool )    

    from TileConditions.TileConditionsConf import TileCondToolNoiseSample
    noiseSampleTool = TileCondToolNoiseSample()
    noiseSampleTool.ProxyNoiseSample       = TileCondProxyCoolFlt( 'TileCondProxyCool_NoiseSample', Source =  '/TILE/OFL02/NOISE/SAMPLE' )
    noiseSampleTool.ProxyOnlineNoiseSample = TileCondProxyCoolFlt( 'TileCondProxyCool_OnlineNoiseSample', Source =  '/TILE/ONL01/NOISE/SAMPLE' )    
    __addFolder( '/TILE/OFL02/NOISE/SAMPLE' )
    __addFolder( '/TILE/ONL01/NOISE/SAMPLE' )    
    acc.addPublicTool( noiseSampleTool )

    from TileConditions.TileConditionsConf import TileCondToolTiming
    timingTool = TileCondToolTiming()
    timingTool.ProxyAdcOffset = TileCondProxyCoolFlt('TileCondProxyCool_AdcOffset', Source = '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    __addFolder( '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    acc.addPublicTool( timingTool )


    from TileConditions.TileConditionsConf import TileBadChanTool
    badChanTool = TileBadChanTool()
    badChanTool.ProxyOflBch = TileCondProxyCoolBch('TileCondProxyCool_OflBch', Source = '/TILE/OFL02/STATUS/ADC' )
    badChanTool.ProxyOnlBch = TileCondProxyCoolBch('TileCondProxyCool_OnlBch', Source = '/TILE/ONL01/STATUS/ADC' )
    __addFolder( '/TILE/OFL02/STATUS/ADC' )
    __addFolder( '/TILE/ONL01/STATUS/ADC' )
    acc.addPublicTool( badChanTool )

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

def TrigCaloDataAccessConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc                      = ComponentAccumulator()
    acc.merge(CaloLUMIBCIDTool(flags))
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
    
    acc.merge(TileCond(flags ))

    acc.addPublicTool( da )

    # configure trigger tools
    return acc


def EgammaCaloMod( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.CFElements import parOR, seqOR, seqAND

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

    acc.merge(TrigCaloDataAccessConfig(flags ))


    # setup algorithms
    #acc.addSequence( seqAND('L2CaloEgamma'), parentName=parentSeq )
    mainSeq=seqAND('L2CaloEgamma')
    from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
    filterL1RoIsAlg             = RoRSeqFilter('filterL1RoIsAlg')
    filterL1RoIsAlg.Input       = ['EMRoIDecisions']
    filterL1RoIsAlg.Output      = ['FilteredEMRoIDecisions']
    from TrigUpgradeTest.TestUtils import MenuTest
    filterL1RoIsAlg.Chains      = [ m.split(':')[1].strip() for m in MenuTest.EMThresholdToChainMapping ]
    filterL1RoIsAlg.OutputLevel = DEBUG

    #acc.addEventAlgo( filterL1RoIsAlg, sequenceName='L2CaloEgamma' )
    mainSeq+=filterL1RoIsAlg
    inViewAlgsSeqName = 'fastCaloInViewAlgs'

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from AthenaCommon.Constants import DEBUG
    fastCaloViewsMaker                 = EventViewCreatorAlgorithm('fastCaloViewsMaker', OutputLevel=DEBUG)
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.InputMakerInputDecisions  = ['FilteredEMRoIDecisions'] # from EMRoIsUnpackingTool
    fastCaloViewsMaker.RoIsLink        = 'initialRoI' # -||-
    fastCaloViewsMaker.InViewRoIs      = 'EMCaloRoIs' # contract with the fastCalo
    fastCaloViewsMaker.Views           = 'EMCaloViews'
    fastCaloViewsMaker.InputMakerOutputDecisions = [ 'L2CaloLinks']

    fastCaloViewsMaker.ViewNodeName = inViewAlgsSeqName

    #acc.addEventAlgo( fastCaloViewsMaker, sequenceName='L2CaloEgamma' )
    mainSeq+=fastCaloViewsMaker
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

    #acc.addSequence( seqAND( inViewAlgsSeqName ),  parentName='L2CaloEgamma' )
    inViewSeq=seqAND( inViewAlgsSeqName )
    mainSeq+=inViewSeq

    from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaFastAlgo
    fastCalo                         = T2CaloEgammaFastAlgo( 'FastCaloAlgo' )
    fastCalo.OutputLevel             = DEBUG
    fastCalo.ClustersName            = 'L2CaloClusters'
    fastCalo.RoIs                    = 'EMCaloRoIs'
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

    #acc.addEventAlgo( fastCalo, sequenceName=inViewAlgsSeqName )
    inViewSeq+=fastCalo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    hypo                     = TrigL2CaloHypoAlgMT( 'L2CaloHypo' )
    hypo.HypoInputDecisions  = fastCaloViewsMaker.InputMakerOutputDecisions[0]
    hypo.CaloClusters        = 'L2CaloClusters'
    hypo.HypoOutputDecisions = 'EgammaCaloDecisions'

    # Here the menu needs to be used.
    # The other option is to do it later when all sequences are setup, 
    # but that would mean relying on HypoAlg names that will need to be searched for 
    # For now ... a hack
    chains = ["HLT_e3_etcut",
              "HLT_e5_etcut",
              "HLT_2e3_etcut",
              "HLT_e3e5_etcut",
              "HLT_e3e5_etcut",
              "HLT_g5_etcut",
              "HLT_e7_etcut"]

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
    for chain in chains:
        tool  = TrigL2CaloHypoToolFromName( chain )
        hypo.HypoTools +=  [ tool ]

    #acc.addEventAlgo( hypo, sequenceName = 'L2CaloEgamma' )
    mainSeq+=hypo

    return acc,mainSeq


if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.set( "global.InputFiles",
                     ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] )
    ConfigFlags.lock()
    acc,seq = EgammaCaloMod( ConfigFlags )
    acc.printConfig()
    print 'All ok'
