#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @Package test_substepIntegration.py
#  @brief Test substep import works properly for common compound transforms
#  @author atlas-comp-transforms-dev@cern.ch

import re
import subprocess
import unittest

import logging
msg = logging.getLogger(__name__)



## Unittests for this module
class trfExitTests(unittest.TestCase):
    
    def test_Reco_tf(self):
        cmd = ['Reco_tf.py', '--showSteps']
        msg.info('Will run this transform: {0}'.format(cmd))
        steps = []
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        p.wait()
        for line in p.stdout:
            print(line, end=' ')
            m = re.match(r"Executor Step: (\w+)", line.decode())
            if m:
                steps.append(m.group(1))
        self.assertEqual(p.returncode, 0)
        self.assertTrue("HITtoRDO" in steps)
        self.assertTrue("RAWtoESD" in steps)

    def test_FullChain_tf(self):
        cmd = ['FullChain_tf.py', '--showSteps']
        msg.info('Will run this transform: {0}'.format(cmd))
        steps = []
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        p.wait()
        for line in p.stdout:
            print(line, end=' ')
            m = re.match(r"Executor Step: (\w+)", line.decode())
            if m:
                steps.append(m.group(1))
        self.assertEqual(p.returncode, 0)
        self.assertTrue("EVNTtoHITS" in steps)
        self.assertTrue("HITtoRDO" in steps)
        self.assertTrue("RAWtoESD" in steps)
        
if __name__ == '__main__':
    unittest.main()
    
