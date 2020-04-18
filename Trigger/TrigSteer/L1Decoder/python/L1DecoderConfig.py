#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from six import iteritems
from AthenaCommon.Logging import logging
log = logging.getLogger('L1DecoderConfig')
from AthenaConfiguration.ComponentFactory import CompFactory

def mapThresholdToL1DecisionCollection(threshold):
    """
    Translates L1 threshold  name of the DecisionsContainer name in the L1Decoder unpacking tools
    """

    mapThresholdToL1Decoder = { "FSNOSEED": "HLTNav_L1FSNOSEED",
                                "EM" : "HLTNav_L1EM",
                                "MU" : "HLTNav_L1MU",
                                "J"  : "HLTNav_L1J",
                                "TAU": "HLTNav_L1TAU",
                                "XE" : "HLTNav_L1MET",
                                "XS" : "HLTNav_L1MET",
                                "TE" : "HLTNav_L1MET" }

    # remove actual threshold value from L1 threshold string
    for (thresholdType, l1Collection) in iteritems(mapThresholdToL1Decoder):
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold \""+ threshold + "\" not mapped to any Decision collection! Available are: " + str(mapThresholdToL1Decoder.values()))

def mapThresholdToL1RoICollection(threshold):
    """
    Translates L1 threshold  name of the RoIDescriptor name in the L1Decoder unpacking tools
    """

    mapThresholdToL1Decoder = { "FSNOSEED": "HLT_FSRoI",
                                "EM" : "HLT_EMRoIs",
                                "MU" : "HLT_MURoIs",
                                "J"  : "HLT_JETRoI",
                                "TAU": "HLT_TAURoI",
                                "XE" : "HLT_FSRoI",
                                "XS" : "HLT_FSRoI",
                                "TE" : "HLT_FSRoI" }

    # remove actual threshold value from L1 threshold string
    for (thresholdType, l1Collection) in iteritems(mapThresholdToL1Decoder):
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold \""+ threshold + "\" not mapped to any ROI collection! Available are: " + str(mapThresholdToL1Decoder.values()))


def createCaloRoIUnpackers():
    #from L1Decoder.L1DecoderConf import EMRoIsUnpackingTool, METRoIsUnpackingTool, JRoIsUnpackingTool, RerunRoIsUnpackingTool, TAURoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    emUnpacker = CompFactory.EMRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("EM"),
                                                 OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("EM")),
                                                 MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 ))

    #            emUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )

    emRerunUnpacker = CompFactory.RerunRoIsUnpackingTool("EMRerunRoIsUnpackingTool",
                                                         SourceDecisions=mapThresholdToL1DecisionCollection("EM"),
                                                         Decisions="HLTNav_RerunL1EM" )

    metUnpacker = CompFactory.METRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("XE"))


    tauUnpacker = CompFactory.TAURoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("TAU"),
                                                   OutputTrigRoIs = recordable("HLT_TAURoI"))

    tauUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="TAU", maxCount=30 )

    jUnpacker = CompFactory.JRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("J"),
                                               OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("J")) )

    jUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="J", maxCount=30 )

    return [emUnpacker, metUnpacker, tauUnpacker, jUnpacker ],[emRerunUnpacker]

def createMuonRoIUnpackers():
    #from L1Decoder.L1DecoderConf import MURoIsUnpackingTool, RerunRoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import RoIsUnpackingMonitoring

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    muUnpacker = CompFactory.MURoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("MU"),
                                     OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("MU")))

    muUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 )

    muRerunUnpacker =  CompFactory.RerunRoIsUnpackingTool("MURerunRoIsUnpackingTool",
                                                          SourceDecisions=mapThresholdToL1DecisionCollection("MU"),
                                                          Decisions="HLTNav_RerunL1MU" )
    return [muUnpacker],[muRerunUnpacker]


#from L1Decoder.L1DecoderConf import L1TriggerResultMaker


def setupL1TriggerResultMaker( alg ):
    alg.MuRoIKey = "LVL1MuonRoIs"
    alg.MuRoILinkName = "mu_roi"
#         # Placeholder for other L1 xAOD outputs:
#         # - CTP result
#         # - L1Topo result
#         # - L1Calo (Run3) RoIs

# from L1Decoder.L1DecoderConf import L1TriggerResultMaker
# class L1TriggerResultMaker(L1TriggerResultMaker):
#     def __init__(self, name='L1TriggerResultMaker', *args, **kwargs):
#         super(L1TriggerResultMaker, self).__init__(name, *args, **kwargs)
#         setupL1TriggerResultMaker( self )


#from L1Decoder.L1DecoderConf import L1Decoder
class L1Decoder(CompFactory.L1Decoder) :
    def __init__(self, name='L1Decoder', *args, **kwargs):
        super(L1Decoder, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags

        # CTP unpacker
        ctpUnpacker = CompFactory.CTPUnpackingTool()

        self.ctpUnpacker = ctpUnpacker
        self.roiUnpackers += [ CompFactory.FSRoIsUnpackingTool("FSRoIsUnpackingTool",
                                                               Decisions=mapThresholdToL1DecisionCollection("FSNOSEED"),
                                                               OutputTrigRoIs = mapThresholdToL1RoICollection("FSNOSEED") ) ]
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
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior += 1

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    #from L1Decoder.L1DecoderConf import L1Decoder, CTPUnpackingTool
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring

    acc = ComponentAccumulator()

    decoderAlg = CompFactory.L1Decoder()
    decoderAlg.L1DecoderSummaryKey = "L1DecoderSummary" # Transient, consumed by DecisionSummaryMakerAlg
    decoderAlg.ctpUnpacker = CompFactory.CTPUnpackingTool( ForceEnableAllChains = flags.Trigger.L1Decoder.forceEnableAllChains,
                                               MonTool = CTPUnpackingMonitoring(512, 200) )



    decoderAlg.roiUnpackers += [ CompFactory.FSRoIsUnpackingTool("FSRoIsUnpackingTool", Decisions=mapThresholdToL1DecisionCollection("FSNOSEED") ) ]

    unpackers, rerunUnpackers = createCaloRoIUnpackers()
    decoderAlg.roiUnpackers += unpackers
    decoderAlg.rerunRoiUnpackers += rerunUnpackers

    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg( flags ) )
    acc.merge( RPCCablingConfigCfg( flags ) )
    unpackers, rerunUnpackers = createMuonRoIUnpackers()
    decoderAlg.roiUnpackers += unpackers
    decoderAlg.rerunRoiUnpackers += rerunUnpackers

    decoderAlg.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
    decoderAlg.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

    from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg, HLTPrescaleCondAlgCfg
    acc.merge( TrigConfigSvcCfg( flags ) )
    acc.merge( HLTPrescaleCondAlgCfg( flags ) )

    # Add the algorithms producing the input RoIBResult (legacy L1) / L1TriggerResult (Run-3 L1)
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import RoIBResultDecoderCfg, L1TriggerByteStreamDecoderCfg
    # TODO: implement flags to allow disabling either RoIBResult or L1TriggerResult
    acc.merge( RoIBResultDecoderCfg(flags) )
    acc.merge( L1TriggerByteStreamDecoderCfg(flags) )

    resultMaker = CompFactory.L1TriggerResultMaker()
    setupL1TriggerResultMaker( resultMaker )
    acc.addEventAlgo( resultMaker )
    Configurable.configurableRun3Behavior -= 1

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

    f=open("L1DecoderConf.pkl","wb")
    acc.store(f)
    f.close()
