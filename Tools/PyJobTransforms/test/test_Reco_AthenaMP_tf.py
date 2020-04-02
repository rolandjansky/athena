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

class Reco_tfAthenaMPtest(unittest.TestCase):
    
    def test_runReco_tf(self):
        cmd = ['Reco_tf.py']
        cmd.extend(['--AMI', 'q222'])
        cmd.extend(['--maxEvents', '24'])
        cmd.append('--athenaopts=--nprocs=4')  
        cmd.extend(['--athenaMPMergeTargetSize', 'ESD:0'])  
            
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line.decode())
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)
        
        # Now load metadata and test a few important values
        with open('jobReport.json') as jr:
            md = json.load(jr)
            self.assertEqual(isinstance(md, dict), True)
            dataDict = pyJobReportToFileDict(md)
            self.assertTrue('ESD' in dataDict)
            self.assertTrue('AOD' in dataDict)
            self.assertTrue('HIST' in dataDict)
            self.assertTrue(len(dataDict['ESD']['subFiles']), 4)
            self.assertEqual(dataDict['AOD']['subFiles'][0]['nentries'], 24)
            self.assertEqual(dataDict['HIST']['subFiles'][0]['nentries'], 24)

if __name__ == '__main__':
    unittest.main()
