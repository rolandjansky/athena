#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#


def L1DecoderCfg(flags):
    flags.dump()
    from AthenaCommon.Configurable import Configurable,ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    from L1Decoder.L1DecoderConf import L1Decoder, CTPUnpackingTool, EMRoIsUnpackingTool, MURoIsUnpackingTool, METRoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring, RoIsUnpackingMonitoring

    acc = ComponentAccumulator()
    decoderAlg = L1Decoder()
    decoderAlg.ctpUnpacker = CTPUnpackingTool( ForceEnableAllChains = flags.get("Trigger.L1Decoder.forceEnableAllChains"),
                                               MonTool = CTPUnpackingMonitoring(512, 200) )


    decoderAlg.roiUnpackers += [ EMRoIsUnpackingTool( Decisions = "EMRoIDecisions",
                                                      OutputTrigRoIs = "EMRoIs",
                                                      MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )) ]

    decoderAlg.roiUnpackers += [METRoIsUnpackingTool( Decisions = "METRoIDecisions",
                                                      OutputTrigRoI = "METRoI") ]

    from L1Decoder.L1MuonConfig import RPCCablingConfig, TGCCablingConfig
    acc.merge(TGCCablingConfig(flags ))
    acc.merge(RPCCablingConfig(flags ))
    decoderAlg.roiUnpackers += [ MURoIsUnpackingTool( Decisions = "MURoIDecisions",
                                                      OutputTrigRoIs = "MURoIs",
                                                      MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 ) ) ]        
    
    from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvcCfg
    acc.merge( TrigConfigSvcCfg( flags ) )

    return acc,decoderAlg

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.set("Trigger.L1Decoder.forceEnableAllChains",True)
    ConfigFlags.set('global.InputFiles',["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",])
    ConfigFlags.lock()
    acc, alg = L1DecoderCfg( ConfigFlags )
    acc.addEventAlgo(alg)
    f=open("L1DecoderConf.pkl","w")
    acc.store(f)
    f.close()
