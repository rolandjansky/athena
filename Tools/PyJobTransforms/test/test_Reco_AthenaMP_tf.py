#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Run a Reco job using AthenaMP and test key metadata in the output
# Note we deliberately keep the ESD as a temporary here, to check this
# works correctly (with no merging)

import glob
import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfReports import pyJobReportToFileDict
from PyJobTransforms.trfUtils import releaseIsOlderThan


sourceFiles = '/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1'

class Reco_tfAthenaMPtest(unittest.TestCase):
    
    def test_runReco_tf(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 1)
        cmd = ['Reco_tf.py', '--inputBSFile']
        cmd.extend(inputs)
        cmd.extend(['--autoConfiguration', 'everything'])
        cmd.extend(['--outputESDFile', 'my.ESD.pool.root'])
        cmd.extend(['--outputAODFile', 'my.AOD.pool.root'])
        cmd.extend(['--outputHISTFile', 'my.HIST.root'])
        cmd.extend(['--preExec', 'rec.doTrigger=False'])  # This is temporary while trigger doesn't work in r19
        cmd.extend(['--outputTAGFile', 'my.TAG.pool.root'])
        cmd.extend(['--maxEvents', '24',])
        cmd.append('--athenaopts=--nprocs=4')  
            
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line)
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)
        
        # Now load metadata and test a few important values
        with open('jobReport.json') as jr:
            md = json.load(jr)
            self.assertEqual(isinstance(md, dict), True)
            dataDict = pyJobReportToFileDict(md)
            self.assertTrue('ESD' in dataDict.keys())
            self.assertTrue('AOD' in dataDict.keys())
            self.assertTrue('HIST' in dataDict.keys())
            self.assertEqual(dataDict['ESD']['subFiles'][0]['geometry'], 'ATLAS-GEO-20-00-01')
            self.assertEqual(dataDict['ESD']['subFiles'][0]['conditions_tag'], 'COMCOND-BLKPA-006-01')
            self.assertEqual(dataDict['ESD']['subFiles'][0]['beam_type'], ['collisions'])
            self.assertEqual(dataDict['ESD']['subFiles'][0]['name'], 'my.ESD.pool.root')
            self.assertEqual(dataDict['AOD']['subFiles'][0]['geometry'], 'ATLAS-GEO-20-00-01')
            self.assertEqual(dataDict['AOD']['subFiles'][0]['conditions_tag'], 'COMCOND-BLKPA-006-01')
            self.assertEqual(dataDict['AOD']['subFiles'][0]['beam_type'], ['collisions'])
            self.assertEqual(dataDict['AOD']['subFiles'][0]['name'], 'my.AOD.pool.root')
            self.assertEqual(dataDict['HIST']['subFiles'][0]['nentries'], 24)

if __name__ == '__main__':
    unittest.main()
