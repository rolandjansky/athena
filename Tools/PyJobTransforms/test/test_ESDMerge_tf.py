#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: test_ESDMerge_tf.py 573315 2013-12-02 15:45:55Z graemes $
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

sourceFiles = '/afs/cern.ch/work/g/graemes/ddm/mc12_8TeV.147033.AlpgenPythia_Auto_P2011C_WmunuNp0.recon.DESD_SGLMU.e1880_s1581_s1586_r4767_tid01302235_00/DESD*'

class ESDMergetest(unittest.TestCase):
    
    def test_runESDMerge(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 6)
        cmd = ['ESDMerge_tf.py', '--inputESDFile']
        cmd.extend(inputs)
        cmd.extend(['--outputESD_MRGFile', 'merged.ESD.pool.root'])
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
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['nentries'], 66)

    def test_runESDMergeSlow(self):
        inputs = glob.glob(sourceFiles)
        self.assertEqual(len(inputs), 6)
        cmd = ['ESDMerge_tf.py', '--inputESDFile']
        cmd.extend(inputs)
        cmd.extend(['--outputESD_MRGFile', 'slowmerged.ESD.pool.root'])
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
            self.assertEqual(md['files']['output'][0]['subFiles'][0]['nentries'], 66)


if __name__ == '__main__':
    unittest.main()
