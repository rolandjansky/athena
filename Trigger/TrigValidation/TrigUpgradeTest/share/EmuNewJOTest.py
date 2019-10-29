# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaCommon.Constants import INFO, DEBUG, VERBOSE
from AthenaCommon.Logging import logging
from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
from TrigUpgradeTest.TriggerHistSvcConfig import TriggerHistSvcConfig
from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg, TGCCablingConfigCfg
from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvcCfg
from TriggerJobOpts.TriggerConfig import triggerSummaryCfg, triggerMonitoringCfg, \
    setupL1DecoderFromMenu, collectHypos, collectFilters
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import generateDecisionTree
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, createStepView
from AthenaCommon.CFElements import seqOR
from RegionSelector.RegSelConfig import regSelCfg
from TrigInDetConfig.InDetConfig import TrigInDetCondConfig
from TrigUpgradeTest.EmuStepProcessingConfig import generateL1DecoderAndChains

log = logging.getLogger('EmuNewJOTest')

flags.needFlagsCategory("Trigger")
flags.Input.isMC = False
flags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

flags.Trigger.L1Decoder.forceEnableAllChains = True

flags.lock()

acc = ComponentAccumulator()
acc.merge(TrigBSReadCfg(flags))
acc.merge(TriggerHistSvcConfig(flags))

l1DecoderAlg, OrigHLTChains = generateL1DecoderAndChains()

setupL1DecoderFromMenu( flags, l1DecoderAlg )

l1DecoderAcc = ComponentAccumulator()
l1DecoderAcc.merge( TGCCablingConfigCfg( flags ) )
l1DecoderAcc.merge( RPCCablingConfigCfg( flags ) )
l1DecoderAcc.merge( TrigConfigSvcCfg( flags ) )
acc.merge(l1DecoderAcc)


from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
toChainDictTranslator = DictFromChainName()
chainDicts = [toChainDictTranslator.getChainDict(chain.name) for chain in OrigHLTChains]


## Set ca in all sequences to none
changed_sequences = {}
HLTChains = []

# select only chains that work after the changes in defining the chains in the EmuStep:
# problem arises when two chains share the same step, exclty the same object, as will happen in the final menu
# since this function will be changed soon, I did not investigate more
# for now, I just exclude some chains in the test
excludeChainNames = ['HLT_mu10', 'HLT_mu20', 'HLT_mu6', 'HLT_mu8_1step', 'HLT_e8', 'HLT_e5_v3', 'HLT_mu8_e8']
for chain in OrigHLTChains:
    if chain.name not in excludeChainNames:
        HLTChains.append(chain)
        log.info("Selected chain %s", chain.name)


for chainIndex, chain in enumerate(HLTChains):
    for stepIndex, step in enumerate(chain.steps):
        for seqIndex, seq in enumerate(step.sequences):
            

            hypoAlg = seq.hypo.Alg.__class__(seq.hypo.Alg.name(), **seq.hypo.Alg.getValuedProperties())

            if seq.name in changed_sequences:
                hypoTool = changed_sequences[seq.name].hypoToolGen(chainDicts[chainIndex])
                hypoAlg.HypoTools = [hypoTool]
                continue
            else:
                conf = seq.hypoToolConf
                hypoTool = conf.hypoToolGen(chainDicts[chainIndex])
                hypoAlg.HypoTools = [hypoTool]
                changed_sequences[seq.name]=conf

 
            stepReco, stepView = createStepView(step.name)

            sequenceAcc = ComponentAccumulator()
            sequenceAcc.addSequence(stepView)
            sequenceAcc.addSequence(seq.sequence.Alg, parentName=stepReco.getName())
            sequenceAcc.addEventAlgo(hypoAlg, sequenceName=stepView.getName())
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

monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, filters, l1DecoderAlg )
acc.merge( monitoringAcc )

topSequenceName = "HLTTop"
HLTTopSequence = seqOR(topSequenceName, [l1DecoderAlg, HLTSteps, summaryAlg, monitoringAlg])
acc.addSequence(HLTTopSequence)

acc.merge(menuAcc)
acc.merge(regSelCfg(flags))
acc.merge(TrigInDetCondConfig(flags))

acc.printConfig()

fname = "EmuNewJOTest.pkl"
log.debug("Storing config in the config %s", fname)
with file(fname, "w") as p:
    acc.store( p, nEvents=4, useBootStrapFile=False, threaded=True )
    p.close()
