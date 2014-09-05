#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Run a TAGMerge job and test key metadata in the output
#

import glob
import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg

sourceFiles = '/afs/cern.ch/work/g/graemes/ddm/data12_8TeV.00211670.express_express.recon.HIST.r5108_tid01383913_00/HIST.*'

class HISTMergetest(unittest.TestCase):
    
    def test_runHISTMerge(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 4)
        cmd = ['HISTMerge_tf.py', '--inputHISTFile']
        cmd.extend(inputs)
        cmd.extend(['--outputHIST_MRGFile', 'merged.HIST.root'])
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
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['nentries'], 873)

if __name__ == '__main__':
    unittest.main()
