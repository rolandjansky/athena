#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Test generation of transform signatures
# $Id: test_trfSignatures.py 630090 2014-11-21 12:06:31Z graemes $
#

import os
import sys
import subprocess
import unittest
import six

from PyJobTransforms.trfLogger import msg

class SignatureTest(unittest.TestCase):
    
    def tearDown(self):
        for f in 'test.json', :
            try:
                os.unlink(f)
            except OSError:
                pass
    
    def test_Signatures(self):
        cmd = ['makeTrfJSONSignatures.py',  '--output', 'test.json']
        msg.info('Will generate transform signatures: {0}'.format(cmd))
        if six.PY2:
            p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
            while p.poll() is None:
                line = p.stdout.readline()
                sys.stdout.write(line.decode())
            # Hoover up remaining buffered output lines
            for line in p.stdout:
                sys.stdout.write(line)
            self.assertEqual(p.returncode, 0)
        else:
            with subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT) as p:
                while p.poll() is None:
                    line = p.stdout.readline()
                    sys.stdout.write(line.decode())
                # Hoover up remaining buffered output lines
                for line in p.stdout:
                    sys.stdout.write(line)
                self.assertEqual(p.returncode, 0)

if __name__ == '__main__':
    unittest.main()
