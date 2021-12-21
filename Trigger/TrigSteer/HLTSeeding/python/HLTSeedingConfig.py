#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import math
from AthenaCommon.Logging import logging
log = logging.getLogger('HLTSeedingConfig')
from AthenaConfiguration.ComponentFactory import CompFactory

_mapL1ThresholdToDecisionCollection = {
    # Full-scan
    "FSNOSEED": "HLTNav_L1FSNOSEED",
    # Muon
    "MU" : "HLTNav_L1MU",
    "PROBEMU"  : "HLTNav_L1PROBEMU",
    # Phase-1 L1Calo
    "eEM": "HLTNav_L1eEM",
    "eTAU": "HLTNav_L1eTAU",
    "jTAU": "HLTNav_L1jTAU",
    "cTAU": "HLTNav_L1cTAU",
    "jJ": "HLTNav_L1jJ",
    "jLJ": "HLTNav_L1jLJ",
    "gJ": "HLTNav_L1gJ",
    "gLJ": "HLTNav_L1gLJ",
    "PROBEeEM" : "HLTNav_L1PROBEeEM",
    "PROBEeTAU" : "HLTNav_L1PROBEeTAU",
    "PROBEjTAU" : "HLTNav_L1PROBEjTAU",
    "PROBEcTAU" : "HLTNav_L1PROBEcTAU",
    # Run-2 L1Calo
    "EM" : "HLTNav_L1EM",
    "J"  : "HLTNav_L1J",
    "TAU": "HLTNav_L1TAU",
    "XE" : "HLTNav_L1MET",
    "XS" : "HLTNav_L1MET",
    "TE" : "HLTNav_L1MET",
    "PROBEEM"  : "HLTNav_L1PROBEEM",
    "PROBETAU" : "HLTNav_L1PROBETAU",
}

_mapL1ThresholdToRoICollection = {
    # Full-scan
    "FSNOSEED": "HLT_FSRoI",
    # Muon
    "MU" : "HLT_MURoIs",
    "PROBEMU"  : "HLT_MURoIs",
    # Phase-1 L1Calo
    "eEM": "HLT_eEMRoIs",
    "eTAU": "HLT_eTAURoIs",
    "jTAU": "HLT_jTAURoIs",
    "cTAU": "HLT_cTAURoIs",
    "jJ": "HLT_jJRoIs",
    "jLJ": "HLT_jLJRoIs",
    "gJ": "HLT_gJRoIs",
    "gLJ": "HLT_gLJRoIs",
    "PROBEeEM" : "HLT_eEMRoIs",
    "PROBEeTAU": "HLT_eTAURoIs",
    "PROBEjTAU": "HLT_jTAURoIs",
    "PROBEcTAU": "HLT_cTAURoIs",
    # Run-2 L1Calo
    "EM" : "HLT_EMRoIs",
    "J"  : "HLT_JETRoI",
    "TAU": "HLT_TAURoI",
    "XE" : "HLT_FSRoI",
    "XS" : "HLT_FSRoI",
    "TE" : "HLT_FSRoI",
    "PROBEEM"  : "HLT_EMRoIs",
    "PROBETAU" : "HLT_TAURoI",
}


def mapThresholdToL1DecisionCollection(threshold):
    """
    Translates L1 threshold  name of the DecisionsContainer name in the HLTSeeding unpacking tools
    """
    # remove actual threshold value from L1 threshold string
    for (thresholdType, l1Collection) in _mapL1ThresholdToDecisionCollection.items():
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold \""+ threshold + "\" not mapped to any Decision collection! Available are: " + str(_mapL1ThresholdToDecisionCollection.values()))


def mapThresholdToL1RoICollection(threshold):
    """
    Translates L1 threshold  name of the RoIDescriptor name in the HLTSeeding unpacking tools
    """
    # remove actual threshold value from L1 threshold string
    for (thresholdType, l1Collection) in _mapL1ThresholdToRoICollection.items():
        if threshold.startswith( thresholdType ):
            return l1Collection

    log.error("Threshold \""+ threshold + "\" not mapped to any ROI collection! Available are: " + str(_mapL1ThresholdToRoICollection.values()))


def createLegacyCaloRoIUnpackers():
    #from HLTSeeding.HLTSeedingConf import EMRoIsUnpackingTool, METRoIsUnpackingTool, JRoIsUnpackingTool, TAURoIsUnpackingTool
    from HLTSeeding.HLTSeedingMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    emUnpacker = CompFactory.EMRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("EM"),
                                                 DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEEM"),
                                                 OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("EM")),
                                                 MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 ))

    metUnpacker = CompFactory.METRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("XE"))


    tauUnpacker = CompFactory.TAURoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("TAU"),
                                                   DecisionsProbe = mapThresholdToL1DecisionCollection("PROBETAU"),
                                                   OutputTrigRoIs = recordable("HLT_TAURoI"))

    tauUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="TAU", maxCount=30 )

    jUnpacker = CompFactory.JRoIsUnpackingTool(Decisions = mapThresholdToL1DecisionCollection("J"),
                                               OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("J")) )

    jUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="J", maxCount=30 )

    return [emUnpacker, metUnpacker, tauUnpacker, jUnpacker ]

def createCaloRoIUnpackers():
    from HLTSeeding.HLTSeedingMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    maxRoICount = 150  # used for histogram range, 144 is the hardware limit
    eFexEMUnpacker = CompFactory.eFexEMRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("eEM"),
        DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEeEM"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("eEM")),
        RoIHalfWidthEta = 0.1,
        RoIHalfWidthPhi = 0.1,
        MonTool = RoIsUnpackingMonitoring(prefix="eEM", maxCount=maxRoICount))
    eFexTauUnpacker = CompFactory.eFexTauRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("eTAU"),
        DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEeTAU"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("eTAU")),
        RoIHalfWidthEta = 0.4,
        RoIHalfWidthPhi = math.pi/8,
        MonTool = RoIsUnpackingMonitoring(prefix="eTAU", maxCount=maxRoICount))
    jFexTauUnpacker = CompFactory.jFexTauRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("jTAU"),
        DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEjTAU"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("jTAU")),
        RoIHalfWidthEta = 0.4,
        RoIHalfWidthPhi = math.pi/8,
        MonTool = RoIsUnpackingMonitoring(prefix="jTAU", maxCount=maxRoICount))
    cTauUnpacker = CompFactory.cTauRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("cTAU"),
        DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEcTAU"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("cTAU")),
        RoIHalfWidthEta = 0.4,
        RoIHalfWidthPhi = math.pi/8,
        MonTool = RoIsUnpackingMonitoring(prefix="cTAU", maxCount=maxRoICount))
    jFexSRJetUnpacker = CompFactory.jFexSRJetRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("jJ"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("jJ")),
        RoIHalfWidthEta = 0.1,
        RoIHalfWidthPhi = 0.1,
        MonTool = RoIsUnpackingMonitoring(prefix="jJ", maxCount=maxRoICount))
    gFexLRJetUnpacker = CompFactory.gFexLRJetRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("jLJ"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("jLJ")),
        RoIHalfWidthEta = 0.1,
        RoIHalfWidthPhi = 0.1,
        MonTool = RoIsUnpackingMonitoring(prefix="jLJ", maxCount=maxRoICount))
    gFexSRJetUnpacker = CompFactory.gFexSRJetRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("gJ"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("gJ")),
        RoIHalfWidthEta = 0.1,
        RoIHalfWidthPhi = 0.1,
        MonTool = RoIsUnpackingMonitoring(prefix="gJ", maxCount=maxRoICount))
    jFexLRJetUnpacker = CompFactory.jFexLRJetRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("gLJ"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("gLJ")),
        RoIHalfWidthEta = 0.1,
        RoIHalfWidthPhi = 0.1,
        MonTool = RoIsUnpackingMonitoring(prefix="gLJ", maxCount=maxRoICount))

    return [eFexEMUnpacker, eFexTauUnpacker, jFexTauUnpacker, cTauUnpacker,
            jFexSRJetUnpacker, gFexLRJetUnpacker, gFexSRJetUnpacker, jFexLRJetUnpacker]

def createLegacyMuonRoIUnpackers(flags):
    from HLTSeeding.HLTSeedingMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    muUnpacker = CompFactory.MURoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("MU"),
        DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEMU"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("MU")),
        MonTool = RoIsUnpackingMonitoring(prefix="MU", maxCount=20))

    return [muUnpacker]

def createMuonRoIUnpackers(flags):
    from HLTSeeding.HLTSeedingMonitoring import RoIsUnpackingMonitoring
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    muUnpacker = CompFactory.MuonRoIsUnpackingTool(
        Decisions = mapThresholdToL1DecisionCollection("MU"),
        DecisionsProbe = mapThresholdToL1DecisionCollection("PROBEMU"),
        OutputTrigRoIs = recordable(mapThresholdToL1RoICollection("MU")),
        RoIHalfWidthEta = 0.1,
        RoIHalfWidthPhi = 0.1,
        MonTool = RoIsUnpackingMonitoring(prefix="MU", maxCount=20))
    return [muUnpacker]

def createPrescalingTool():
    from HLTSeeding.HLTSeedingMonitoring import PrescalingMonitoring

    prescaler = CompFactory.PrescalingTool(MonTool = PrescalingMonitoring())
    return prescaler

def createKeyWriterTool():
    keyWriter = CompFactory.getComp('TrigConf::KeyWriterTool')('KeyWriterToolOnline')
    keyWriter.ConfKeys = 'TrigConfKeysOnline'
    keyWriter.IncludeL1PrescaleKey = False
    return keyWriter

def getL1TriggerResultMaker(flags):
    l1trMaker = CompFactory.L1TriggerResultMaker()

    # Muon RoIs
    if flags.Trigger.enableL1MuonPhase1:
       l1trMaker.MuRoIKey = "LVL1MuonRoIs"
       from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import getTrigThresholdDecisionTool
       l1trMaker.ThresholdPatternTools += [getTrigThresholdDecisionTool()]
    else:
       l1trMaker.MuRoIKey = ""

    # L1Calo RoIs
    if flags.Trigger.enableL1CaloPhase1:
        l1trMaker.eFexEMRoIKey = "L1_eEMRoI"
        l1trMaker.eFexTauRoIKey = "L1_eTauRoI"
        l1trMaker.jFexTauRoIKey = "L1_jFexTauRoI"
        l1trMaker.jFexSRJetRoIKey = "L1_jFexSRJetRoI"
        l1trMaker.jFexLRJetRoIKey = "L1_jFexLRJetRoI"
        l1trMaker.gFexSRJetRoIKey = "L1_gFexSRJetRoI"
        l1trMaker.gFexLRJetRoIKey = "L1_gFexLRJetRoI"
        l1trMaker.cTauRoIKey = "L1_cTauRoI"  # Note: WriteHandle
        l1trMaker.cjTauLinkKey = "L1_cTauRoI.jTauLink"  # Note: WriteDecorHandle
        l1trMaker.ThresholdPatternTools += [
            CompFactory.eFexEMRoIThresholdsTool(),
            CompFactory.eFexTauRoIThresholdsTool(),
            CompFactory.jFexTauRoIThresholdsTool(),
            CompFactory.cTauRoIThresholdsTool(),
            CompFactory.jFexSRJetRoIThresholdsTool(),
            CompFactory.jFexLRJetRoIThresholdsTool(),
            CompFactory.gFexSRJetRoIThresholdsTool(),
            CompFactory.gFexLRJetRoIThresholdsTool(),
        ]
    else:
       l1trMaker.eFexEMRoIKey = ""
       l1trMaker.eFexTauRoIKey = ""
       l1trMaker.jFexTauRoIKey = ""
       l1trMaker.jFexSRJetRoIKey = ""
       l1trMaker.jFexLRJetRoIKey = ""
       l1trMaker.gFexSRJetRoIKey = ""
       l1trMaker.gFexLRJetRoIKey = ""
       l1trMaker.cTauRoIKey = ""
       l1trMaker.cjTauLinkKey = ""

    # Placeholder for other L1 xAOD outputs:
    # - CTP result
    # - L1Topo result
    # - the remaining Run-3 L1Calo RoIs

    return l1trMaker


class HLTSeeding(CompFactory.HLTSeeding) :
    def __init__(self, name='HLTSeeding', *args, **kwargs):
        super(HLTSeeding, self).__init__(name, *args, **kwargs)

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
        if flags.Trigger.doID or flags.Trigger.doCalo:
            if flags.Trigger.enableL1CaloPhase1:
                self.xAODRoIUnpackers += createCaloRoIUnpackers()
            if flags.Trigger.enableL1CaloLegacy:
                self.RoIBRoIUnpackers += createLegacyCaloRoIUnpackers()

        # MU unpacker
        if flags.Trigger.doMuon:
            if flags.Trigger.enableL1MuonPhase1:
                self.xAODRoIUnpackers += createMuonRoIUnpackers(flags)
            else:
                self.RoIBRoIUnpackers += createLegacyMuonRoIUnpackers(flags)

        self.prescaler = createPrescalingTool()
        self.KeyWriterTool = createKeyWriterTool()

        self.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
        self.CostMonitoringChain = flags.Trigger.CostMonitoring.chain

        self.HLTSeedingSummaryKey = "HLTSeedingSummary"


def HLTSeedingCfg(flags, seqName = None):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior += 1

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    if seqName:
        from AthenaCommon.CFElements import parOR
        acc = ComponentAccumulator(sequence=parOR(seqName)) # TODO - once rec-ex-common JO are phased out this can also be dropped
    else:
        acc = ComponentAccumulator()
    from HLTSeeding.HLTSeedingMonitoring import CTPUnpackingMonitoring
    decoderAlg = CompFactory.HLTSeeding()
    decoderAlg.RoIBResult = "RoIBResult" if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1MuonPhase1 else ""
    decoderAlg.L1TriggerResult = "L1TriggerResult" if flags.Trigger.enableL1MuonPhase1 or flags.Trigger.enableL1CaloPhase1 else ""
    decoderAlg.HLTSeedingSummaryKey = "HLTSeedingSummary" # Transient, consumed by DecisionSummaryMakerAlg
    decoderAlg.ctpUnpacker = CompFactory.CTPUnpackingTool( ForceEnableAllChains = flags.Trigger.HLTSeeding.forceEnableAllChains,
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
        if flags.Trigger.enableL1CaloPhase1:
            decoderAlg.xAODRoIUnpackers += createCaloRoIUnpackers()
        if flags.Trigger.enableL1CaloLegacy:
            decoderAlg.RoIBRoIUnpackers += createLegacyCaloRoIUnpackers()

    if flags.Trigger.doMuon:
        if flags.Trigger.enableL1MuonPhase1:
            decoderAlg.xAODRoIUnpackers += createMuonRoIUnpackers(flags)
        else:
            decoderAlg.RoIBRoIUnpackers += createLegacyMuonRoIUnpackers(flags)
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

    # Add the algorithm creating L1TriggerResult which is the input to HLTSeeding (Run-3 L1)
    if flags.Trigger.enableL1MuonPhase1 or flags.Trigger.enableL1CaloPhase1:
        acc.addEventAlgo( getL1TriggerResultMaker(flags), sequenceName = seqName )

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

    ConfigFlags.Trigger.HLTSeeding.forceEnableAllChains= True
    ConfigFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",]
    ConfigFlags.lock()
    acc = HLTSeedingCfg( ConfigFlags )


    f=open("HLTSeedingConf.pkl","wb")
    acc.store(f)
    f.close()
