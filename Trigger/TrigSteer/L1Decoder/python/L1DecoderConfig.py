#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#


def L1DecoderCfg(flags):
    flags.dump()
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    from AthenaCommon.Configurable import Configurable,ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    from L1Decoder.L1DecoderConf import L1Decoder, CTPUnpackingTool, EMRoIsUnpackingTool, MURoIsUnpackingTool, METRoIsUnpackingTool, JRoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring, RoIsUnpackingMonitoring

    acc = ComponentAccumulator()
    decoderAlg = L1Decoder()
    decoderAlg.L1DecoderSummaryKey = recordable("L1DecoderSummary")
    decoderAlg.ctpUnpacker = CTPUnpackingTool( ForceEnableAllChains = flags.Trigger.L1Decoder.forceEnableAllChains,
                                               MonTool = CTPUnpackingMonitoring(512, 200) )


    decoderAlg.roiUnpackers += [ EMRoIsUnpackingTool( Decisions = recordable("L1EM"),
                                                      OutputTrigRoIs = recordable("EMRoIs"),
                                                      MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )) ]

    decoderAlg.roiUnpackers += [METRoIsUnpackingTool( Decisions = recordable("L1MET")) ]

    decoderAlg.roiUnpackers += [JRoIsUnpackingTool( Decisions = recordable("L1J"),
                                                      OutputTrigRoIs = recordable("JETRoI")) ]


    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg    
    acc.mergeAll( TGCCablingConfigCfg( flags ) )
    acc.mergeAll( RPCCablingConfigCfg( flags ) )
    decoderAlg.roiUnpackers += [ MURoIsUnpackingTool( Decisions = recordable("L1MU"),
                                                      OutputTrigRoIs = recordable("MURoIs"),
                                                      MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 ) ) ]        
    



    from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvcCfg
    acc.merge( TrigConfigSvcCfg( flags ) )

    return acc,decoderAlg

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Trigger.L1Decoder.forceEnableAllChains= True
    ConfigFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",]
    ConfigFlags.lock()
    acc, alg = L1DecoderCfg( ConfigFlags )
    acc.addEventAlgo(alg)

    f=open("L1DecoderConf.pkl","w")
    acc.store(f)
    f.close()
