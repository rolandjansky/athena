#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: test_AODMerge_tf.py 617963 2014-09-22 13:13:07Z graemes $
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

sourceFiles = '/afs/cern.ch/work/g/graemes/ddm/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5539_tid01483607_00/AOD.*'

class AODMergetest(unittest.TestCase):
    
    def test_runAODMerge(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 5)
        cmd = ['AODMerge_tf.py', '--inputAODFile']
        cmd.extend(inputs)
        cmd.extend(['--outputAOD_MRGFile', 'merged.AOD.pool.root'])
        cmd.extend(['--outputTAGFile', 'TAG.pool.root'])
        cmd.extend(['--reportName', 'jobReportFast'])
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
        with open('jobReportFast.json') as jr:
            md = json.load(jr)
            self.assertEqual(isinstance(md, dict), True)
            self.assertEqual(md['files']['output']['common']['nentries'], 2500)

    def test_runAODMergeSlow(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 5)
        cmd = ['AODMerge_tf.py', '--inputAODFile']
        cmd.extend(inputs)
        cmd.extend(['--outputAOD_MRGFile', 'slowmerged.AOD.pool.root'])
        cmd.extend(['--outputTAGFile', 'slowTAG.pool.root'])
        cmd.extend(['--fastPoolMerge', 'False'])
        cmd.extend(['--reportName', 'jobReportSlow'])
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
        with open('jobReportSlow.json') as jr:
            md = json.load(jr)
            self.assertEqual(isinstance(md, dict), True)
            self.assertEqual(md['files']['output']['common']['nentries'], 2500)


if __name__ == '__main__':
    unittest.main()
