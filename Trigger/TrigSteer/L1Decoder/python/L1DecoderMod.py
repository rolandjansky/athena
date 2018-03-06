#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#


def L1DecoderMod(flags):
    flags.dump()
    from AthenaCommon.Configurable import Configurable,ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    from L1Decoder.L1DecoderConf import L1Decoder, CTPUnpackingTool, EMRoIsUnpackingTool, MURoIsUnpackingTool, METRoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring, RoIsUnpackingMonitoring

    acc = ComponentAccumulator()
    decoderAlg = L1Decoder()
    decoderAlg.ctpUnpacker = CTPUnpackingTool( ForceEnableAllChains = flags.get("L1Decoder.Flags.forceEnableAllChains"),
                                               MonTool = CTPUnpackingMonitoring(512, 200) )

    if flags.get("L1Decoder.Flags.doCalo") == True:
        decoderAlg.roiUnpackers += [ EMRoIsUnpackingTool( Decisions = "EMRoIDecisions",
                                                          OutputTrigRoIs = "EMRoIs",
                                                          MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )) ]

        decoderAlg.roiUnpackers += [METRoIsUnpackingTool( Decisions = "METRoIDecisions",
                                                          OutputTrigRoI = "METRoI") ]

    if flags.get("L1Decoder.Flags.doMuon") == True:
        decoderAlg.roiUnpackers += [ MURoIsUnpackingTool( Decisions = "MURoIDecisions",
                                                          OutputTrigRoIs = "MURoIs",
                                                          MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 ) ) ]        
    
    from AthenaCommon.Constants import DEBUG
    decoderAlg.OutputLevel=DEBUG
    for u in decoderAlg.roiUnpackers:
        u.OutputLevel=DEBUG

    acc.addEventAlgo(decoderAlg)
    

    from TrigConfigSvc.TrigConfigSvcMod import TrigConfigSvcMod
    acc.addConfig( TrigConfigSvcMod, flags )

    return acc

if __name__ == "__main__":
    from AthenaConfiguration.ConfigFlags import ConfigFlagContainer    
    cfgFlags = ConfigFlagContainer()
    cfgFlags.set("L1Decoder.Flags.doCalo",True)
    cfgFlags.set("L1Decoder.Flags.doMuon",True)
    cfgFlags.set("L1Decoder.Flags.forceEnableAllChains",True)
    l1 = L1DecoderMod( cfgFlags )
    f=open("L1DecoderConf.pkl","w")
    l1.store(f)
    f.close()
