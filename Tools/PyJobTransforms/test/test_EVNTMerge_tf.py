#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: test_EVNTMerge_tf.py 573315 2013-12-02 15:45:55Z graemes $
# Run a EVNTMerge job and test key metadata in the output
#

import glob
import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg

sourceFiles = '/afs/cern.ch/work/g/graemes/ddm/mc11_7TeV.117838.TTbar_MT1675_PowHeg_Pythia_P2011C.evgen.EVNT.e1736_tid01148343_00/EVNT.*'

class EVNTMergetest(unittest.TestCase):
    
    def test_runEVNTMerge(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 3)
        cmd = ['EVNTMerge_tf.py', '--inputEVNTFile']
        cmd.extend(inputs)
        cmd.extend(['--outputEVNT_MRGFile', 'merged.EVNT.pool.root'])
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
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['nentries'], 15000)
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['name'], 'merged.EVNT.pool.root')

if __name__ == '__main__':
    unittest.main()
