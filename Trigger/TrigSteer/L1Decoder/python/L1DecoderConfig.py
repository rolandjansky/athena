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

def createPrescalingTool():
    from L1Decoder.L1DecoderMonitoring import PrescalingMonitoring

    prescaler = CompFactory.PrescalingTool(MonTool = PrescalingMonitoring())
    return prescaler


def getL1TriggerResultMaker():
    l1trMaker = CompFactory.L1TriggerResultMaker()

    # Muon RoIs
    l1trMaker.MuRoIKey = "LVL1MuonRoIs"
    l1trMaker.MuRoILinkName = "mu_roi"

    # Placeholder for other L1 xAOD outputs:
    # - CTP result
    # - L1Topo result
    # - L1Calo (Run3) RoIs

    return l1trMaker


#from L1Decoder.L1DecoderConf import L1Decoder
class L1Decoder(CompFactory.L1Decoder) :
    def __init__(self, name='L1Decoder', *args, **kwargs):
        super(L1Decoder, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags

        # CTP unpacker
        ctpUnpacker = CompFactory.CTPUnpackingTool()

        self.ctpUnpacker = ctpUnpacker
        from TrigEDMConfig.TriggerEDMRun3 import recordable
        self.roiUnpackers += [ CompFactory.FSRoIsUnpackingTool("FSRoIsUnpackingTool",
                                                               Decisions=mapThresholdToL1DecisionCollection("FSNOSEED"),
                                                               OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("FSNOSEED") )) ]
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

        self.prescaler = createPrescalingTool()

        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        self.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
        self.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

        self.L1DecoderSummaryKey = "L1DecoderSummary"


def L1DecoderCfg(flags, seqName = None):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior += 1

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    #from L1Decoder.L1DecoderConf import L1Decoder, CTPUnpackingTool
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring

    acc = ComponentAccumulator(sequenceName = seqName)

    decoderAlg = CompFactory.L1Decoder()
    decoderAlg.RoIBResult = "RoIBResult" if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1Phase1 else ""
    decoderAlg.L1TriggerResult = "L1TriggerResult" if flags.Trigger.enableL1Phase1 else ""
    decoderAlg.L1DecoderSummaryKey = "L1DecoderSummary" # Transient, consumed by DecisionSummaryMakerAlg
    decoderAlg.ctpUnpacker = CompFactory.CTPUnpackingTool( ForceEnableAllChains = flags.Trigger.L1Decoder.forceEnableAllChains,
                                                           MonTool = CTPUnpackingMonitoring(512, 200) )
    #Transient bytestream
    if flags.Input.Format == "POOL":
        transTypeKey = ("TransientBSOutType","StoreGateSvc+TransientBSOutKey")
        decoderAlg.ExtraInputs += [transTypeKey]


    from TrigEDMConfig.TriggerEDMRun3 import recordable
    decoderAlg.roiUnpackers += [ CompFactory.FSRoIsUnpackingTool("FSRoIsUnpackingTool", Decisions=mapThresholdToL1DecisionCollection("FSNOSEED"),
                                  OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("FSNOSEED")) ) ]

    if flags.Trigger.doCalo:
        unpackers, rerunUnpackers = createCaloRoIUnpackers()
        decoderAlg.roiUnpackers += unpackers
        decoderAlg.rerunRoiUnpackers += rerunUnpackers

    if flags.Trigger.doMuon:
        unpackers, rerunUnpackers = createMuonRoIUnpackers()
        decoderAlg.roiUnpackers += unpackers
        decoderAlg.rerunRoiUnpackers += rerunUnpackers

    decoderAlg.prescaler = createPrescalingTool()
    decoderAlg.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
    decoderAlg.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

    acc.addEventAlgo( decoderAlg, sequenceName = seqName )

    if flags.Input.Format == "BS":
        # Add the algorithm decoding ByteStream into xAOD (Run-3 L1) and/or RoIBResult (legacy L1)
        from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
        acc.merge( L1TriggerByteStreamDecoderCfg(flags), sequenceName = seqName )

    # Add the algorithm creating L1TriggerResult which is the input to L1Decoder (Run-3 L1)
    if flags.Trigger.enableL1Phase1:
        acc.addEventAlgo( getL1TriggerResultMaker(), sequenceName = seqName )

    from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg, HLTPrescaleCondAlgCfg
    acc.merge( TrigConfigSvcCfg( flags ) )
    acc.merge( HLTPrescaleCondAlgCfg( flags ) )


    Configurable.configurableRun3Behavior -= 1

    return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Trigger.L1Decoder.forceEnableAllChains= True
    ConfigFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",]
    ConfigFlags.lock()
    acc = L1DecoderCfg( ConfigFlags )
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg( ConfigFlags ) )
    acc.merge( RPCCablingConfigCfg( ConfigFlags ) )


    f=open("L1DecoderConf.pkl","wb")
    acc.store(f)
    f.close()
