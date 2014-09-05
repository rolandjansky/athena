#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Run a few simple "Athena_tf.py" jobs and check that logfile scanning produces the correct results
# $Id: test_LogFileScanner.py 576626 2013-12-21 23:29:31Z graemes $
#

import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg

class LogscanTest(unittest.TestCase):
    
    def test_athenaNormalScan(self):
        cmd = ['Athena_tf.py']
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line)
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)
        

    def test_athenaDisableFilter(self):
        cmd = ['Athena_tf.py', '--ignoreFiles', 'None']
        msg.info('Will run this transform: {0}'.format(cmd))
        cmd.extend(['--reportName', 'jobReportDisable'])
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line)
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 68)


    def test_athenaManualFilter(self):
        cmd = ['Athena_tf.py', '--ignoreFiles', 'None']
        cmd.extend(['--ignorePatterns', 'An ERROR message', 'A FATAL error message'])
        cmd.extend(['--reportName', 'jobReportManual'])
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line)
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)


    def test_athenaManualRegexp(self):
        cmd = ['Athena_tf.py', '--ignoreFiles', 'None']
        cmd.extend(['--ignorePatterns', 'An? (ERROR|FATAL) .*message'])
        cmd.extend(['--reportName', 'jobReportRegexp'])
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line)
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)


if __name__ == '__main__':
    unittest.main()
