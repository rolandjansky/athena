#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfAMI.py
#  @brief Unittests for trfAMI.py
#  @author bjorn.sarrazin@cern.ch
#  @version $Id: test_trfAMI.py 749614 2016-05-25 10:46:26Z lerrenst $

import unittest


import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfAMI import TagInfo, TrfConfig
from PyJobTransforms.trfArgs import addAthenaArguments

## Unittests for trfAMI module
class trfAMIUnitTests(unittest.TestCase):

    # test T0 tag
    def test_info_q120(self):
        physics = {'AMITag': 'q120',
                   'maxEvents': '25',
                   'autoConfiguration': ['everything'],
                   'preExec': {'all': ['rec.doTrigger=False;rec.doFloatingPointException.set_Value_and_Lock(True)']}}

        tag = TagInfo('q120')
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_trf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,17.2.0.3')
        self.assertEqual(tag.trfs[0].physics, physics)

    # test a new transform tag from AMI
    def test_info_q220(self):
        self.maxDiff = None
        physics = {'conditionsTag': {'all': 'CONDBR2-BLKPA-2018-03'},
                   'beamType': 'cosmics',
                   'ignoreErrors': False,
                   'autoConfiguration': ['everything'],
                   'maxEvents': '25',
                   'AMITag': 'q220',
                   'preExec': {'all': ['from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doCTPMon=False;']},
                   'geometryVersion': {'all': 'ATLAS-R2-2016-01-00-01'}}

        tag = TagInfo("q220")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,19.1.3.7')
        self.assertEqual(tag.trfs[0].physics, physics)

    # test a new transform tag from AMI
    def test_info_x306(self):
        self.maxDiff = None
        physics = {'conditionsTag': {'all': 'CONDBR2-ES1PA-2014-03'},
                   'beamType': 'cosmics',
                   'ignoreErrors': True,
                   'autoConfiguration': ['everything'],
                   'maxEvents': '-1',
                   'AMITag': 'x306',
                   'preExec': {'all': ['from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doCTPMon=False;']},
                   'geometryVersion': {'all': 'ATLAS-R2-2015-02-00-00'}}

        tag = TagInfo("x306")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,20.1.3.3')
        self.assertEqual(tag.trfs[0].physics, physics)

    def test_info_m1380(self):
        self.maxDiff = None
        physics = {}

        tag = TagInfo("m1380")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'NTUPMerge_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,20.1.0.4,gcc48')
        self.assertEqual(tag.trfs[0].physics, physics)

    def test_info_f559(self):
        self.maxDiff = None
        physics = {'AMITag': 'f',
                   'autoConfiguration': 'everything',
                   'conditionsTag': 'CONDBR2-BLKPA-2014-03',
                   'ignoreErrors': 'True',
                   'maxEvents': '-1',
                   'postExec': 'CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]',
                   'preExec': 'rec.doTrigger.set_Value_and_Lock(False);'}

        tag = TagInfo("f559")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,19.3.0.4')
        self.assertEqual(tag.trfs[0].newTransform, True)
        self.assertEqual(tag.trfs[0].physics, physics)
        self.assertEqual(tag.trfs[0].inFiles, {'inputRDOFile': '/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root'})
        self.assertEqual(tag.trfs[0].outFiles, {'outputAODFile': 'myAOD.pool.root', 'outputESDFile': 'myESD.pool.root'})
        self.assertEqual(tag.trfs[0].outputs, "{'outputESDFile': {'dstype': 'ESD', 'ifMatch': '(?!.*DRAW.*)'}, 'outputAODFile': {'dstype': 'AOD', 'ifMatch': 'data[0-9][0-9]_(cos|1beam|.*eV|comm)(?!.(.*DRAW.*|.*ZeroBias.*x[0-9].*))'}}")
        self.assertEqual(tag.trfs[0].inDS, None)
        self.assertEqual(sorted(tag.trfs[0].outfmts), ['AOD', 'ESD'])

    def test_info_r6390(self):
        self.maxDiff = None
        physics = {'postInclude': 'all:RecJobTransforms/UseFrontier.py',
                   'conditionsTag': 'all:OFLCOND-RUN12-SDR-14',
                   'numberOfHighPtMinBias': '0.12311',
                   'postExec': ['all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"] HITtoRDO:job.StandardPileUpToolsAlg pTools["MergeMcEventCollTool"].OnlySaveSignalTruth=True RAWtoESD:ToolSvc.egammaConversionFinderTool.VertexFitterTo lSvc.InDetAdaptiveVxFitterTool;ToolSvc.egammaAmbiguityTool.minNoPixHits=3       -> --postinclude = RecJobTransforms/UseFrontier.py'],
                   'preExec': ['all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties; perties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25);from LAr rRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.NumberOfCollisio _Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4) RAWtoESD:from TriggerJobOpts.TriggerFlags import% gerFlags;TriggerFlags.triggerMenuSetup="DC14_loose_mc_prescale";from CaloRec.CaloCellFlags import jobproperties% roperties.CaloCellFlags.doLArCellEmMisCalib=False ESDtoAOD:TriggerFlags.AODEDMSet="AODFULL"'],
                   'geometryVersion': 'all:ATLAS-R2-2015-01-01-00',
                   'numberOfLowPtMinBias': '39.87688',
        }

        tag = TagInfo("r6390")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,19.1.1.5')
        self.assertEqual(tag.trfs[0].newTransform, True)
        self.assertEqual(tag.trfs[0].physics, physics)
        self.assertEqual(tag.trfs[0].inFiles, {})
        self.assertEqual(tag.trfs[0].outFiles, {})
        self.assertEqual(tag.trfs[0].outputs, {})
        self.assertEqual(tag.trfs[0].inDS, None)
        self.assertEqual(tag.trfs[0].outfmts, [])

    def test_info_r6411(self):
        self.maxDiff = None
        physics = {'steering': 'RAWtoESD:in-RDO,in+RDO_TRIG,in-BS',
                   'conditionsTag': 'default:OFLCOND-RUN12-SDR-14',
                   'numberOfHighPtMinBias': '0.12311',
                   'autoConfiguration': 'everything',
                   'numberOfLowPtMinBias': '39.87688',
                   'DBRelease': 'default:current',
                   'geometryVersion': 'default:ATLAS-R2-2015-01-01-00',
                   'preInclude': 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsUpgradeConfig1_25ns.py,RunDependentSimData/configLumi_run222222.py',
                   'postExec': ['default:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]',
                                'HITtoRDO:job.StandardPileUpToolsAlg.PileUpTools["MergeMcEventCollTool"].OnlySaveSignalTruth=True'],
                   'preExec': ['default:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25);from LArROD.LArRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4)',
                               'RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="DC14_loose_mc_prescale";from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False',
                               'ESDtoAOD:TriggerFlags.AODEDMSet="AODFULL"'],
                   'postInclude': 'default:RecJobTransforms/UseFrontier.py'
        }

        tag = TagInfo("r6411")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,19.1.1.9')
        self.assertEqual(tag.trfs[0].newTransform, True)
        self.assertEqual(tag.trfs[0].physics, physics)
        self.assertEqual(tag.trfs[0].inFiles, {
            'inputHighPtMinbiasHitsFile': 'mc14_13TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3038_s1967_s2007',
            'inputLowPtMinbiasHitsFile': 'mc14_13TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3038_s1967_s2007'})
        self.assertEqual(tag.trfs[0].outFiles, {})
        self.assertEqual(tag.trfs[0].outputs, {})
        self.assertEqual(tag.trfs[0].inDS, None)
        self.assertEqual(tag.trfs[0].outfmts, [])


    def test_info_r6540(self):
        self.maxDiff = None
        physics = {'DataRunNumber': '222525',
                   'autoConfiguration': 'everything',
                   'conditionsTag': 'default:OFLCOND-RUN12-SDR-25',
                   'geometryVersion': 'default:ATLAS-R2-2015-03-01-00',
                   'numberOfCavernBkg': '0',
                   'postExec': ['RAWtoESD:ToolSvc.LArAutoCorrTotalToolDefault.deltaBunch=1',
                                'HITtoRDO:job.StandardPileUpToolsAlg.PileUpTools["MdtDigitizationTool"].LastXing=150;ToolSvc.LArAutoCorrTotalToolDefault.deltaBunch=1',
                                'RDOtoRDOTrigger:from AthenaCommon.AlgSequence import AlgSequence;AlgSequence().LVL1TGCTrigger.TILEMU=True'],
                   'postInclude': 'default:RecJobTransforms/UseFrontier.py',
                   'preExec': ['all:rec.Commissioning.set_Value_and_Lock(True);rec.doTrigger.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)',
                               'RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v5_loose_mc_prescale";from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False',
                               'ESDtoAOD:TriggerFlags.AODEDMSet="AODFULL"',
                               'RDOtoRDOTrigger:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v5_loose_mc_prescale";'],
                   'steering': 'RAWtoESD:in-RDO,in+RDO_TRIG,in-BS'
        }

        tag = TagInfo("r6540")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,20.1.4.3')
        self.assertEqual(tag.trfs[0].newTransform, True)
        self.assertEqual(tag.trfs[0].physics, physics)
        self.assertEqual(tag.trfs[0].inFiles, {})
        self.assertEqual(tag.trfs[0].outFiles, {})
        self.assertEqual(tag.trfs[0].outputs, {})
        self.assertEqual(tag.trfs[0].inDS, None)
        self.assertEqual(tag.trfs[0].outfmts, [])

    # test proper handling of substeparglist in preInclude command
    def test_info_r6594(self):
        self.maxDiff = None
        physics = {'autoConfiguration': 'everything',
                   'conditionsTag': 'default:OFLCOND-RUN12-SDR-28',
                   'geometryVersion': 'default:ATLAS-R2-2015-03-01-00',
                   'numberOfCavernBkg': '0',
                   'numberOfHighPtMinBias': '0.12268057',
                   'numberOfLowPtMinBias': '39.8773194',
                   'postExec': ['all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]',
                                'HITtoRDO:job.StandardPileUpToolsAlg.PileUpTools["MergeMcEventCollTool"].OnlySaveSignalTruth=True',
                                'RAWtoESD:ToolSvc.LArAutoCorrTotalToolDefault.deltaBunch=1',
                                'HITtoRDO:job.StandardPileUpToolsAlg.PileUpTools["MdtDigitizationTool"].LastXing=150;ToolSvc.LArAutoCorrTotalToolDefault.deltaBunch=1'],
                   'postInclude': 'default:RecJobTransforms/UseFrontier.py',
                   'preExec': ['all:rec.Commissioning.set_Value_and_Lock(True);rec.doTrigger.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)',
                               'RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v5_loose_mc_prescale";from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False',
                               'ESDtoAOD:TriggerFlags.AODEDMSet="AODFULL"',
                               'RDOtoRDOTrigger:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup="MC_pp_v5_loose_mc_prescale";',
                               'HITtoRDO:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50);'],
                   'preInclude': ['HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_50ns_Config1.py,RunDependentSimData/configLumi_run222510.py',
                                  'RDOtoRDOTrigger:RecExPers/RecoOutputMetadataList_jobOptions.py'],
                   'steering': 'RAWtoESD:in-RDO,in+RDO_TRIG,in-BS'
        }

        tag = TagInfo("r6594")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,20.1.4.5')
        self.assertEqual(tag.trfs[0].newTransform, True)
        self.assertEqual(tag.trfs[0].physics, physics)
        self.assertEqual(tag.trfs[0].inFiles, {
            'inputHighPtMinbiasHitsFile': 'mc15_valid.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2169_tid05098387_00',
            'inputLowPtMinbiasHitsFile': 'mc15_valid.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2169_tid05098374_00'})
        self.assertEqual(tag.trfs[0].outFiles, {})
        self.assertEqual(tag.trfs[0].outputs, {})
        self.assertEqual(tag.trfs[0].inDS, None)
        self.assertEqual(tag.trfs[0].outfmts, [])

    def test_transform(self):
        from PyJobTransforms.transform import transform
        from PyJobTransforms.trfArgClasses import argFactory, argString, argBool
        from PyJobTransforms.trfDefaultFiles import getInputFileName

        tf = transform()
        addAthenaArguments(tf.parser)
        tf.parser.add_argument('--inputBSFile', type = argFactory(argString))
        tf.parser.add_argument('--outputBS_MRGFile', type = argFactory(argString))
        tf.parser.add_argument('--extraArg', type = argFactory(argString))
        tf.parser.add_argument('--maskEmptyInputs', type=argFactory(argBool))

        tf.parseCmdLineArgs('--AMIConfig=m1831 --extraArg=special --outputBS_MRGFile=myOutput'.split())

        # not part of tag
        self.assertEqual(tf.argdict['extraArg'].value, 'special')
        #  part of tag
        self.assertEqual(tf.argdict['inputBSFile'].value, getInputFileName('inputBSFile'))
        #  part of tag but overwritten at command line
        self.assertEqual(tf.argdict['outputBS_MRGFile'].value, 'myOutput')


if __name__ == '__main__':
    unittest.main()
