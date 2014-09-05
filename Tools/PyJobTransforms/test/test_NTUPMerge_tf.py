#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
# Run a NTUPMerge job and test key metadata in the output
#

import glob
import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg

sourceFiles = '/afs/cern.ch/work/g/graemes/ddm/mc12_8TeV.107650.AlpgenJimmy_AUET2CTEQ6L1_ZeeNp0.merge.NTUP_ZPRIMEEE.e1218_s1469_s1470_r3542_r3549_p1344_tid01126179_00/NTUP*'

class NTUPMergetest(unittest.TestCase):
    
    def test_runNTUPMerge(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 3)
        cmd = ['NTUPMerge_tf.py', '--inputNTUP_ZPRIMEEEFile']
        cmd.extend(inputs)
        cmd.extend(['--outputNTUP_ZPRIMEEE_MRGFile', 'merged.NTUP_ZPRIMEEE.pool.root'])
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
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['nentries'],23254)
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['name'], 'merged.NTUP_ZPRIMEEE.pool.root')

if __name__ == '__main__':
    unittest.main()
