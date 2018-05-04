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

    if flags.get("Trigger.L1Decoder.doCalo") == True:
        decoderAlg.roiUnpackers += [ EMRoIsUnpackingTool( Decisions = "EMRoIDecisions",
                                                          OutputTrigRoIs = "EMRoIs",
                                                          MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )) ]

        decoderAlg.roiUnpackers += [METRoIsUnpackingTool( Decisions = "METRoIDecisions",
                                                          OutputTrigRoI = "METRoI") ]

    if flags.get("Trigger.L1Decoder.doMuon") == True:
        from L1Decoder.L1MuonConfig import RPCCablingConfig, TGCCablingConfig
        acc.addConfig( TGCCablingConfig, flags )
        acc.addConfig( RPCCablingConfig, flags )
        decoderAlg.roiUnpackers += [ MURoIsUnpackingTool( Decisions = "MURoIDecisions",
                                                          OutputTrigRoIs = "MURoIs",
                                                          MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 ) ) ]        
    
    from AthenaCommon.Constants import DEBUG
    decoderAlg.OutputLevel=DEBUG
    for u in decoderAlg.roiUnpackers:
        u.OutputLevel=DEBUG

    acc.addEventAlgo(decoderAlg)
    

    from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvcCfg
    acc.addConfig( TrigConfigSvcCfg, flags )


    return acc

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.set("Trigger.L1Decoder.doCalo",True)
    ConfigFlags.set("Trigger.L1Decoder.doMuon",True)
    ConfigFlags.set("Trigger.L1Decoder.forceEnableAllChains",True)
    ConfigFlags.set('global.InputFiles',["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",])
    ConfigFlags.lock()
    l1 = L1DecoderCfg( ConfigFlags )
    f=open("L1DecoderConf.pkl","w")
    l1.store(f)
    f.close()
