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

sourceFiles = '/afs/cern.ch/work/g/graemes/ddm/data12_8TeV.00207865.physics_JetTauEtmiss.merge.TAG.r4065_p1278_tid01030417_00/TAG.*'

class TAGMergetest(unittest.TestCase):
    
    def test_runTAGMerge(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 2)
        cmd = ['TAGMerge_tf.py', '--inputTAGFile']
        cmd.extend(inputs)
        cmd.extend(['--outputTAG_MRGFile', 'merged.TAG.pool.root'])
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
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['nentries'], 78232)

if __name__ == '__main__':
    unittest.main()
