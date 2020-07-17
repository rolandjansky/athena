#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Test the various DBRelease scenarios
# $Id: test_trfUtilsDBRelease.py 740535 2016-04-15 11:21:07Z graemes $
#

import json
import subprocess
import os
import os.path
import shutil
import sys
import unittest

from PyJobTransforms.trfExitCodes import trfExit as trfExit

from PyJobTransforms.trfLogger import msg

class DBReleasetest(unittest.TestCase):
    
    # Standard setup using CVMFS
    def test_cvmfsStandard(self):
        cmd = ['Athena_tf.py', '--DBRelease', '23.3.1']
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line.decode())
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)
    
    # Setup using CVMFS 'current'
    def test_cvmfsCurrent(self):
        cmd = ['Athena_tf.py', '--DBRelease', 'current']
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line.decode())
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)
    
    # Test using a DBRelease file which exists
    def test_tarball(self):
        try:
            os.symlink('/afs/cern.ch/work/g/graemes/ddm/ddo.000001.Atlas.Ideal.DBRelease.v220701/DBRelease-22.7.1.tar.gz', 'DBRelease-22.7.1.tar.gz')
        except (IOError, OSError) as xxx_todo_changeme:
            # Ignore file exists - if that happens the link was already there
            (errno, errMsg) = xxx_todo_changeme.args
            # Ignore file exists - if that happens the link was already there
            if errno == 17:
                pass
            else:
                raise
        cmd = ['Athena_tf.py', '--DBRelease', 'DBRelease-22.7.1.tar.gz']
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line.decode())
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)

#     # Test using a DBRelease file which exists, absolute path (this should _not_ rerun the setup script, of course)
#     def test_tarballAbsPath(self):
#         cmd = ['Athena_tf.py', '--DBRelease', '/afs/cern.ch/work/g/graemes/ddm/ddo.000001.Atlas.Ideal.DBRelease.v220701/DBRelease-22.7.1.tar.gz']
#         msg.info('Will run this transform: {0}'.format(cmd))
#         p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line.decode())
#         # Hoover up remaining buffered output lines
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
#     
#     # Test using the next Pcache release DBRelease file, a soft link to a candidate
#     def test_tarballPcacheCurrent(self):
#         cmd = ['Athena_tf.py', '--DBRelease', '/afs/cern.ch/atlas/www/GROUPS/DATABASE/pacman4/DBRelease/DBRelease-pcache-current.tar.gz']
#         msg.info('Will run this transform: {0}'.format(cmd))
#         p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line.decode())
#         # Hoover up remaining buffered output lines
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
#     
#     # Test using a DBRelease file which doesn't exist, but should fallback to CVMFS
#     def test_tarballFallback(self):
#         cmd = ['Athena_tf.py', '--DBRelease', 'DBRelease-23.3.1.tar.gz']
#         msg.info('Will run this transform: {0}'.format(cmd))
#         p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line.decode())
#         # Hoover up remaining buffered output lines
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
    
    # Negative test - use an illegal name format
    def test_illegalName(self):
        cmd = ['Athena_tf.py', '--DBRelease', 'FailMeHarder']
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line.decode())
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'))
        
def tearDownModule():
    if os.path.isdir('DBRelease'):
        shutil.rmtree('DBRelease')

if __name__ == '__main__':
    import os
    testname = 'test_trfUtilsDBRelease'
    if not os.path.exists (testname):
        os.mkdir (testname)
        os.chdir (testname)
    unittest.main()
