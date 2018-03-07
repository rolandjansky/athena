from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.CFElements import parOR, seqOR, seqAND

def EgammaCaloMod( flags ):
    from AthenaCommon.Constants import DEBUG
    acc = ComponentAccumulator()

    # load Calo geometry
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg    
    acc.addConfig( LArGMCfg,  flags )
    acc.addConfig( TileGMCfg, flags )

    # setup algorithms
    acc.addSequence( seqAND("L2CaloEgamma") )
    from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
    filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
    filterL1RoIsAlg.Input = ["EMRoIDecisions"]
    filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
    from TrigUpgradeTest.TestUtils import MenuTest
    filterL1RoIsAlg.Chains = [ m.split(":")[1].strip() for m in MenuTest.EMThresholdToChainMapping ]
    filterL1RoIsAlg.OutputLevel = DEBUG

    acc.addEventAlgo( filterL1RoIsAlg, "L2CaloEgamma" )
    inViewAlgsSeqName = "fastCaloInViewAlgs"

    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from AthenaCommon.Constants import DEBUG
    fastCaloViewsMaker = EventViewCreatorAlgorithm("fastCaloViewsMaker", OutputLevel=DEBUG)
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.Decisions = "FilteredEMRoIDecisions" # from EMRoIsUnpackingTool
    fastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
    fastCaloViewsMaker.InViewRoIs = "EMCaloRoIs" # contract with the fastCalo
    fastCaloViewsMaker.Views = "EMCaloViews"

    fastCaloViewsMaker.ViewNodeName = inViewAlgsSeqName

    acc.addEventAlgo( fastCaloViewsMaker, "L2CaloEgamma" )


    acc.addSequence( seqAND( inViewAlgsSeqName ),  "L2CaloEgamma" )

    
    from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaFastAlgo
    fastCalo = T2CaloEgammaFastAlgo( "FastCaloAlgo" )
    fastCalo.OutputLevel=DEBUG
    fastCalo.ClustersName="L2CaloClusters"
    fastCalo.RoIs="EMCaloRoIs"
    fastCalo.EtaWidth = 0.2
    fastCalo.PhiWidth = 0.2
    # will be replace by the service, which shoudl drive its confing via flags
    # i.e. sth along this line acc.addConfig( TrigCaloDataAccessSvc, flags )
    # for the moment this is done by hand here    
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess    
    accessTool = TrigDataAccess()
    accessTool.ApplyOffsetCorrection=False
    acc.addAlgTool( accessTool )

    # the tools should becone private at soem point
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import *
    ringer = RingerFexConfig("RingsMaker")
    ringer.RingsKey="CaloRings"
    tools = [ EgammaSamp2FexNoTimerConfig("FaAlgoSamp2FexConfig"),
              EgammaSamp1FexNoTimerConfig("FaAlgoSamp1FexConfig"),
              EgammaEmEnFexNoTimerConfig("FaAlgoEmEnFexConfig"),
              EgammaHadEnFexNoTimerConfig("FaAlgoHadEnFexConfig"),
              ringer ]
    [ acc.addPublicTool( t ) for t in tools ]
    fastCalo.IAlgToolList = tools #[t.getFullName() for t in tools ]
    
    fastCalo.CalibListBarrel=[ EgammaSshapeCalibrationBarrelConfig(),
                               EgammaHitsCalibrationBarrelConfig(),
                               EgammaGapCalibrationConfig(),
                               EgammaTransitionRegionsConfig() ]

    fastCalo.CalibListEndcap=[ EgammaSshapeCalibrationEndcapConfig(),
                               EgammaHitsCalibrationEndcapConfig(),
                               EgammaGapCalibrationConfig() ]

    acc.addEventAlgo( fastCalo, inViewAlgsSeqName )
    return acc


if __name__ == "__main__":
    from AthenaConfiguration.ConfigFlags import ConfigFlagContainer    
    flags = ConfigFlagContainer()
    acc = EgammaCaloMod( flags )
    acc.printConfig()
    print "All ok"
