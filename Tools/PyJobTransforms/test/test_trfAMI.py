#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfAMI.py
#  @brief Unittests for trfAMI.py
#  @author bjorn.sarrazin@cern.ch
#  @version $Id: test_trfAMI.py 648031 2015-02-19 09:57:41Z graemes $

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
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,17.2.0.3')
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
        self.maxDiff = None
        physics = {'conditionsTag': {'all': 'CONDBR2-ES1PA-2014-01'}, 'beamType': 'cosmics', 'ignoreErrors': True, 'autoConfiguration': ['everything'], 'maxEvents': '-1', 'AMITag': 'q220', 'preExec': {'all': ['from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArHVCorr=False;jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;from JetRec.JetRecFlags import jetFlags;jetFlags.useTracks=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doCTPMon=False;']}, 'geometryVersion': {'all': 'ATLAS-R2-2015-02-00-00'}}
        tag=TagInfo("q220")
        self.assertTrue(isinstance(tag.trfs[0], TrfConfig))
        self.assertEqual(tag.isProdSys, False)
        self.assertEqual(tag.trfs[0].name, 'Reco_tf.py')
        self.assertEqual(tag.trfs[0].release, 'AtlasProduction,19.1.3.7')
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
        
 

