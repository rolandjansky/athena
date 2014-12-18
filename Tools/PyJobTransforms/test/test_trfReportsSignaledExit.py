#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: test_trfReportsSignaledExit.py 634986 2014-12-10 12:43:21Z graemes $
# Test the return status and job report for a script that
# dies with a signal

import json
import subprocess
import sys
import unittest

from PyJobTransforms.trfLogger import msg

class signaledExit(unittest.TestCase):
    
    def test_suicideScript(self):
        cmd = ['ExeWrap_tf.py']
        cmd.extend(['--exe', 'Suicide.py'])
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        p.wait()
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 65)
        
        # Now load metadata and test a few important values
        with open('jobReport.json') as jr:
            md = json.load(jr)
            self.assertEqual(isinstance(md, dict), True)
            self.assertTrue("SIGKILL signal" in md['exitMsg'])
            self.assertEqual(md['exitCode'], 65)

if __name__ == '__main__':
    unittest.main()
