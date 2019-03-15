# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaCommon.Constants import INFO, DEBUG
from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
from TrigUpgradeTest.TriggerHistSvcConfig import TriggerHistSvcConfig
from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg
from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvcCfg
from TriggerJobOpts.TriggerConfig import triggerSummaryCfg, triggerMonitoringCfg, collectDecisionObjects, \
    triggerAddMissingEDMCfg, triggerOutputStreamCfg, setupL1DecoderFromMenu, collectHypos, collectFilters
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import generateDecisionTree
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import decisionTree_From_Chains, generateDecisionTreeOld
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import HypoAlgNode, MenuSequence
from AthenaCommon.CFElements import seqOR, seqAND, findAlgorithm
from RegionSelector.RegSelConfig import regSelCfg
from TrigUpgradeTest.InDetConfig import TrigInDetCondConfig
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg

from TrigUpgradeTest.EmuStepProcessingConfig import generateL1DecoderAndChains


flags.needFlagsCategory("Trigger")
flags.Input.isMC = False
flags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

flags.Trigger.L1Decoder.forceEnableAllChains = True

flags.lock()

acc = ComponentAccumulator()
acc.merge(TrigBSReadCfg(flags))
acc.merge(TriggerHistSvcConfig(flags))

l1DecoderAlg, HLTChains = generateL1DecoderAndChains()
setupL1DecoderFromMenu( flags, l1DecoderAlg )

l1DecoderAcc = ComponentAccumulator()
l1DecoderAcc.mergeAll( TGCCablingConfigCfg( flags ) )
l1DecoderAcc.mergeAll( RPCCablingConfigCfg( flags ) )
l1DecoderAcc.merge( TrigConfigSvcCfg( flags ) )
acc.merge(l1DecoderAcc)


from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
toChainDictTranslator = DictFromChainName()
chainDicts = [toChainDictTranslator.getChainDict(chain.name) for chain in HLTChains]

## Set ca in all sequences to none
for index, chain in enumerate(HLTChains):
    for step in chain.steps:
        for seqIndex, seq in enumerate(step.sequences):
            hypoAlg = seq.hypo.Alg.__class__(seq.hypo.Alg.name(), **seq.hypo.Alg.getValuedProperties())
            hypoTool = seq.hypoToolConf.hypoToolGen(chainDicts[index])
            hypoAlg.HypoTools = [hypoTool]

            sequenceAcc = ComponentAccumulator()
            sequenceAcc.addSequence(seq.sequence.Alg)
            seq.ca = sequenceAcc
            sequenceAcc.wasMerged()

            ms = MenuSequence( Sequence = seq.sequence.Alg,
                               Maker    = seq.maker.Alg,
                               Hypo     =  hypoAlg,
                               HypoToolGen = None,
                               CA = sequenceAcc)

            step.sequences[seqIndex] = ms

menuAcc = generateDecisionTree(HLTChains)

HLTSteps = menuAcc.getSequence("HLTAllSteps")
hypos = collectHypos(HLTSteps)
filters = collectFilters(HLTSteps)

summaryAcc, summaryAlg = triggerSummaryCfg(flags, hypos)
acc.merge(summaryAcc)

monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, l1DecoderAlg )
acc.merge( monitoringAcc )

topSequenceName = "HLTTop"
HLTTopSequence = seqOR(topSequenceName, [l1DecoderAlg, HLTSteps, summaryAlg, monitoringAlg])
acc.addSequence(HLTTopSequence)

acc.merge(menuAcc)
acc.merge(regSelCfg(flags))
acc.merge(TrigInDetCondConfig(flags))

acc.getEventAlgo( "TrigSignatureMoniMT").OutputLevel=DEBUG

acc.printConfig()

fname = "EmuNewJOTest.pkl"
print("Storing config in the config", fname)
with file(fname, "w") as p:
    acc.store( p, nEvents=4, useBootStrapFile=False, threaded=True )
    p.close()
