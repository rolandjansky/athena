#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
log = logging.getLogger('L1DecoderConfig')

def mapThresholdToL1DecisionCollection(threshold):
    """
    Translates L1 threshold  name of the DecisionsContainer name in the L1Decoder unpacking tools
    """
    if threshold == "" or threshold == "FS":
        return "HLTNav_L1FS"

    mapThresholdToL1Decoder = { "EM" : "HLTNav_L1EM",
                                "MU" : "HLTNav_L1MU",
                                "J"  : "HLTNav_L1J",
                                "TAU": "HLTNav_L1TAU",
                                "XE" : "HLTNav_L1MET",
                                "XS" : "HLTNav_L1MET",
                                "TE" : "HLTNav_L1MET" }

    # remove actual threshold value from L1 threshold string
    for thresholdType, l1Collection in mapThresholdToL1Decoder.iteritems():
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold "+ threshold + " not mapped to any Decision collection! Available are: " + str(mapThresholdToL1Decoder.values()))

def mapThresholdToL1RoICollection(threshold):
    """
    Translates L1 threshold  name of the RoIDescriptor name in the L1Decoder unpacking tools
    """
    if threshold == "" or threshold == "FS":
        return "HLT_FSRoI"

    mapThresholdToL1Decoder = { "EM" : "HLT_EMRoIs",
                                "MU" : "HLT_MURoIs",
                                "J"  : "HLT_JETRoI",
                                "TAU": "HLT_TAURoI",
                                "XE" : "HLT_FSRoI",
                                "XS" : "HLT_FSRoI",
                                "TE" : "HLT_FSRoI" }

    # remove actual threshold value from L1 threshold string
    for thresholdType, l1Collection in mapThresholdToL1Decoder.iteritems():
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold "+ threshold + " not mapped to any ROI collection! Available are: " + str(mapThresholdToL1Decoder.values()))


def createCaloRoIUnpackers():
    from L1Decoder.L1DecoderConf import EMRoIsUnpackingTool, METRoIsUnpackingTool, JRoIsUnpackingTool, RerunRoIsUnpackingTool, TAURoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    emUnpacker = EMRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("EM"),
                                     OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("EM")),
                                     MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 ))

    #            emUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )

    emRerunUnpacker = RerunRoIsUnpackingTool("EMRerunRoIsUnpackingTool",
                                             SourceDecisions=mapThresholdToL1DecisionCollection("EM"),
                                             Decisions="HLTNav_RerunL1EM" )

    metUnpacker = METRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("XE"))


    tauUnpacker = TAURoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("TAU"),
                                       OutputTrigRoIs = recordable("HLT_TAURoI"))

    tauUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="TAU", maxCount=30 )

    jUnpacker = JRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("J"),
                                   OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("J")) )

    jUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="J", maxCount=30 )


    return [emUnpacker, metUnpacker, tauUnpacker, jUnpacker ],[emRerunUnpacker]

def createMuonRoIUnpackers():
    from L1Decoder.L1DecoderConf import MURoIsUnpackingTool, RerunRoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import RoIsUnpackingMonitoring

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    muUnpacker = MURoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("MU"),
                                     OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("MU")))

    muUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 )

    muRerunUnpacker =  RerunRoIsUnpackingTool("MURerunRoIsUnpackingTool",
                                              SourceDecisions=mapThresholdToL1DecisionCollection("MU"),
                                              Decisions="HLTNav_RerunL1MU" )
    return [muUnpacker],[muRerunUnpacker]



from L1Decoder.L1DecoderConf import L1Decoder

class L1Decoder(L1Decoder) :
    def __init__(self, name='L1Decoder', *args, **kwargs):
        super(L1Decoder, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from L1Decoder.L1DecoderConf import CTPUnpackingTool

        # CTP unpacker

        ctpUnpacker = CTPUnpackingTool(OutputLevel = self.getDefaultProperty("OutputLevel"),
                                       ForceEnableAllChains = True)

        self.ctpUnpacker = ctpUnpacker

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            unpackers, rerunUnpackers = createCaloRoIUnpackers()            
            self.roiUnpackers += unpackers
            self.rerunRoiUnpackers += rerunUnpackers

        # MU unpacker
        if TriggerFlags.doMuon():
            unpackers, rerunUnpackers = createMuonRoIUnpackers()
            self.roiUnpackers += unpackers
            self.rerunRoiUnpackers += rerunUnpackers

        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        self.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
        self.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

        self.L1DecoderSummaryKey = "L1DecoderSummary"


def L1DecoderCfg(flags):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    from L1Decoder.L1DecoderConf import L1Decoder, CTPUnpackingTool
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring

    acc = ComponentAccumulator()
    decoderAlg = L1Decoder()
    decoderAlg.L1DecoderSummaryKey = "L1DecoderSummary" # Transient, consumed by DecisionSummaryMakerAlg
    decoderAlg.ctpUnpacker = CTPUnpackingTool( ForceEnableAllChains = flags.Trigger.L1Decoder.forceEnableAllChains,
                                               MonTool = CTPUnpackingMonitoring(512, 200) )

    decoderAlg.roiUnpackers, decoderAlg.rerunRoiUnpackers = createCaloRoIUnpackers()

    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg( flags ) )
    acc.merge( RPCCablingConfigCfg( flags ) )
    unpackers, rerunUnpackers = createMuonRoIUnpackers()
    decoderAlg.roiUnpackers += unpackers
    decoderAlg.rerunRoiUnpackers += rerunUnpackers

    decoderAlg.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
    decoderAlg.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

    from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvcCfg
    acc.merge( TrigConfigSvcCfg( flags ) )

    # Add the algorithm producing the input RoIBResult
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import RoIBResultDecoderCfg
    acc.merge( RoIBResultDecoderCfg(flags) )

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
