#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Run a Reco job and test key metadata in the output
#

import glob
import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfReports import pyJobReportToFileDict

class Reco_tftest(unittest.TestCase):
    
    def test_runReco_q222_tf(self):
        cmd = ['Reco_tf.py']
        cmd.extend(['--AMI', 'q222'])
        cmd.extend(['--maxEvents', '2'])
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
            self.assertTrue('resource' in md.keys())
            self.assertEqual(md['resource']['executor']['AODtoTAG']['nevents'], 2)
            self.assertEqual(md['resource']['executor']['ESDtoAOD']['nevents'], 2)
            self.assertEqual(md['resource']['executor']['RAWtoESD']['nevents'], 2)

if __name__ == '__main__':
    unittest.main()
