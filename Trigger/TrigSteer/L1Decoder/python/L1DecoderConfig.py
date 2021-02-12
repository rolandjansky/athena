#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

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
    for (thresholdType, l1Collection) in mapThresholdToL1Decoder.items():
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
    for (thresholdType, l1Collection) in mapThresholdToL1Decoder.items():
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold \""+ threshold + "\" not mapped to any ROI collection! Available are: " + str(mapThresholdToL1Decoder.values()))


def createCaloRoIUnpackers():
    #from L1Decoder.L1DecoderConf import EMRoIsUnpackingTool, METRoIsUnpackingTool, JRoIsUnpackingTool, TAURoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    emUnpacker = CompFactory.EMRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("EM"),
                                                 OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("EM")),
                                                 MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 ))

    #            emUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )


    metUnpacker = CompFactory.METRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("XE"))


    tauUnpacker = CompFactory.TAURoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("TAU"),
                                                   OutputTrigRoIs = recordable("HLT_TAURoI"))

    tauUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="TAU", maxCount=30 )

    jUnpacker = CompFactory.JRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("J"),
                                               OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("J")) )

    jUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="J", maxCount=30 )

    return [emUnpacker, metUnpacker, tauUnpacker, jUnpacker ]

def createMuonRoIUnpackers(flags):
    #from L1Decoder.L1DecoderConf import MURoIsUnpackingTool
    from L1Decoder.L1DecoderMonitoring import RoIsUnpackingMonitoring

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    muUnpacker = CompFactory.MURoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("MU"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("MU")))

    muUnpacker.OutputRecRoIs = "" if flags.Trigger.enableL1Phase1 else "HLT_RecMURoIs"
    muUnpacker.MuRoILinkName = "LVL1MuonRoIs" if flags.Trigger.enableL1Phase1 else ""
    muUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 )

    return [muUnpacker]

def createPrescalingTool():
    from L1Decoder.L1DecoderMonitoring import PrescalingMonitoring

    prescaler = CompFactory.PrescalingTool(MonTool = PrescalingMonitoring())
    return prescaler

def createKeyWriterTool():
    keyWriter = CompFactory.getComp('TrigConf::KeyWriterTool')('KeyWriterToolOnline')
    keyWriter.ConfKeys = 'TrigConfKeysOnline'
    keyWriter.IncludeL1PrescaleKey = False
    return keyWriter

def getL1TriggerResultMaker():
    l1trMaker = CompFactory.L1TriggerResultMaker()

    # Muon RoIs
    l1trMaker.MuRoIKey = "LVL1MuonRoIs"

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
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

        # CTP unpacker
        ctpUnpacker = CompFactory.CTPUnpackingTool()

        self.ctpUnpacker = ctpUnpacker
        from TrigEDMConfig.TriggerEDMRun3 import recordable
        self.RoIBRoIUnpackers += [
            CompFactory.FSRoIsUnpackingTool("FSRoIsUnpackingTool",
                                            Decisions=mapThresholdToL1DecisionCollection("FSNOSEED"),
                                            OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("FSNOSEED") )) ]
        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            unpackers = createCaloRoIUnpackers()
            self.RoIBRoIUnpackers += unpackers

        # MU unpacker
        if TriggerFlags.doMuon():
            unpackers = createMuonRoIUnpackers(flags)
            if flags.Trigger.enableL1Phase1:
                self.xAODRoIUnpackers += unpackers
            else:
                self.RoIBRoIUnpackers += unpackers

        self.prescaler = createPrescalingTool()
        self.KeyWriterTool = createKeyWriterTool()

        self.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
        self.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

        self.L1DecoderSummaryKey = "L1DecoderSummary"


def L1DecoderCfg(flags, seqName = None):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior += 1

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    if seqName:
        from AthenaCommon.CFElements import parOR
        acc = ComponentAccumulator(sequence=parOR(seqName)) # TODO - once rec-ex-common JO are phased out this can also be dropped
    else:
        acc = ComponentAccumulator()
    from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring
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
    decoderAlg.RoIBRoIUnpackers += [
        CompFactory.FSRoIsUnpackingTool("FSRoIsUnpackingTool", Decisions=mapThresholdToL1DecisionCollection("FSNOSEED"),
                                        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("FSNOSEED")) ) ]

    if flags.Trigger.doCalo:
        unpackers = createCaloRoIUnpackers()
        decoderAlg.RoIBRoIUnpackers += unpackers

    if flags.Trigger.doMuon:
        unpackers = createMuonRoIUnpackers(flags)
        if flags.Trigger.enableL1Phase1:
            decoderAlg.xAODRoIUnpackers += unpackers
        else:
            decoderAlg.RoIBRoIUnpackers += unpackers
        from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg
        acc.merge( TGCCablingConfigCfg( flags ) )
        acc.merge( RPCCablingConfigCfg( flags ) )

    decoderAlg.prescaler = createPrescalingTool()
    decoderAlg.KeyWriterTool = createKeyWriterTool()
    decoderAlg.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
    decoderAlg.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

    if flags.Input.Format == "BS" and not flags.Trigger.doLVL1:
        # Add the algorithm decoding ByteStream into xAOD (Run-3 L1) and/or RoIBResult (legacy L1)
        from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
        acc.merge( L1TriggerByteStreamDecoderCfg(flags), sequenceName = seqName )

    # Add the algorithm creating L1TriggerResult which is the input to L1Decoder (Run-3 L1)
    if flags.Trigger.enableL1Phase1:
        acc.addEventAlgo( getL1TriggerResultMaker(), sequenceName = seqName )

    acc.addEventAlgo( decoderAlg, sequenceName = seqName )

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


    f=open("L1DecoderConf.pkl","wb")
    acc.store(f)
    f.close()
