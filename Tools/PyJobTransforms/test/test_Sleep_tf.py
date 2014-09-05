#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Simple test of Sleep_tf.py
# $Id: test_Sleep_tf.py 573333 2013-12-02 16:20:46Z graemes $
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
        cmd = ['Sleep_tf.py', '--dust', '10']
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

if __name__ == '__main__':
    unittest.main()
