#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Simple test of Echo_tf.py
#

import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg

class Echotest(unittest.TestCase):
    
    def test_runEcho(self):
        cmd = ['Echo_tf.py']
        cmd.extend(['--testInt', '1234'])
        cmd.extend(['--testFloat', '-1.212'])
        cmd.extend(['--testIntList', '1,2,3,4,5,6'])
        cmd.extend(['--testSubstepList', 'all:juice', 'jane:apple', 'bob:orange', 'alice:pear'])
        cmd.extend(['--testSubstepInt', 'all:34', 'jane:1', 'bob:2', 'alice:-3'])
        cmd.extend(['--testSubstepBool', 'all:True', 'jane:false', 'bob:tRuE', 'alice:FaLse'])
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

if __name__ == '__main__':
    unittest.main()
