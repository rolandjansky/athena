#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfAMI.py
#  @brief Unittests for trfAMI.py
#  @author bjorn.sarrazin@cern.ch
#  @version $Id: test_trfAMI.py 626621 2014-11-06 13:47:51Z graemes $

import unittest


import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfAMI import TagInfo, TrfConfig
from PyJobTransforms.trfArgs import addAthenaArguments

## Unittests for trfAMI module
class trfAMIUnitTests(unittest.TestCase):

    # test T0 tag
    def test_info_q120(self):
        physics={'AMITag':'q120',
                 'maxEvents': -1,
                 'autoConfiguration':'everything',
                 'preExec':'rec.doFloatingPointException.set_Value_and_Lock(True)'}
        
        tag=TagInfo('q120')
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_trf.py')
        self.assertEqual(tag.trfs[0].release, '17.2.0.3')
        self.assertEqual(tag.trfs[0].physics, physics)

    # test multiple transforms tag from ProdSys
    def test_info_a180(self):
        physics={'conditionsTag':'OFLCOND-MC12-SIM-00',
                  'postInclude':'FastCaloSimHit/postInclude.AF2FilterHitItems.py FastSimulationJobTransforms/jobConfig.FastCaloSim_ID_cuts.py FastSimulationJobTransforms/jobConfig.egamma_lateral_shape_tuning.config20.py',
                  'DBRelease':'20.2.2',
                  'preInclude':'FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py FastCaloSimHit/preInclude.AF2Hit.py',
                  'geometryVersion':'ATLAS-GEO-20-00-01'}

        
        tag=TagInfo('a180')
        self.assertEqual(len(tag.trfs),2)
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertTrue(isinstance(tag.trfs[1], TrfConfig))
        self.assertEqual(tag.isProdSys, True)
        self.assertEqual(tag.trfs[0].name, 'Merging_trf.py')
        self.assertEqual(tag.trfs[0].release, '17.2.1.4.2,TrigMC')
        self.assertEqual(tag.trfs[0].physics, physics)
        self.assertEqual(tag.trfs[1].name, 'DigiMReco_trf.py')
        self.assertEqual(tag.trfs[1].release, '17.2.1.4.2,TrigMC')
        
    # test a new transform tag from AMI
    def test_info_q220(self):
        physics = {'conditionsTag': {'all': 'OFLCOND-RUN1-SDR-05'}, 'AMITag': 'q220', 'postInclude': {'all': ['RecJobTransforms/UseFrontier.py']}, 'preExec': {'ESDtoAOD': ['TriggerFlags.AODEDMSet="AODSLIM"'], 'all': ['rec.Commissioning.set_Value_and_Lock(True);jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.)'], 'HITtoRDO': ['userRunLumiOverride={"run":212272, "lb":1, "starttstamp":1349603811, "mu":0.000}'], 'RAWtoESD': ['from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False']}, 'autoConfiguration': ['everything'], 'maxEvents': '-1', 'preInclude': {'HITtoRDO': ['RunDependentSimData/configLumi_user.py', 'SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py']}, 'digiSeedOffset2': 39, 'inputHITSFile': ['/afs/cern.ch/atlas/groups/validation/RTT/HITS.01454684._000014.pool.root.1'], 'digiSeedOffset1': 39, 'triggerConfig': {'RAWtoESD': 'NONE'}, 'inputCavernHitsFile': ['/afs/cern.ch/atlas/groups/validation/RTT/HITS.01415612._000121.pool.root.2'], 'geometryVersion': {'all': 'ATLAS-R1-2012-02-00-00'}, 'numberOfCavernBkg': 0, 'jobNumber': 1}
        tag=TagInfo("q220")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, '19.1.3.7')
        self.assertEqual(tag.trfs[0].physics, physics)
        

    # test setup of transform     
    def test_transform(self):
        from PyJobTransforms.transform import transform
        from PyJobTransforms.trfArgClasses import argFactory, argString
        from PyJobTransforms.trfDefaultFiles import getInputFileName

        tf=transform()
        addAthenaArguments(tf.parser)
        tf.parser.add_argument('--inputBSFile', type=argFactory(argString))
        tf.parser.add_argument('--outputBSFile', type=argFactory(argString))
        tf.parser.add_argument('--extraArg', type=argFactory(argString))
        
        tf.parseCmdLineArgs('--AMIConfig=p1346 --extraArg=special --outputBSFile=myOutput'.split())

        # not part of tag 
        self.assertEqual(tf.argdict['extraArg'].value, 'special')
        #  part of tag 
        self.assertEqual(tf.argdict['inputBSFile'].value, getInputFileName('inputBSFile'))
        #  part of tag but overwritten at command line 
        self.assertEqual(tf.argdict['outputBSFile'].value, 'myOutput')


if __name__ == '__main__':
    unittest.main()
        
 

