#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @Package test_trfUtils.py
#  @brief Unittests for trfUtils.py
#  @author graeme.andrew.stewart@cern.ch

import unittest
import os
import subprocess
import time

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfUtils import *

from PyJobTransforms.trfDecorators import timelimited

## Unittests for this module

class trfUtilsInfanticide(unittest.TestCase):
    def setUp(self):
        self.waitWrapper = "wait.sh"
        self.exitWrapper = "exit.sh"
        
        waitFile = open(self.waitWrapper, "w")
        print("#!/bin/sh", file=waitFile)
        print("sleep 100", file=waitFile)
        waitFile.close()
        os.chmod(self.waitWrapper, 0o0755)
    
        exitFile = open(self.exitWrapper, "w")
        print("#!/bin/sh", file=exitFile)
        print("sleep 100 &", file=exitFile)
        exitFile.close()
        os.chmod(self.exitWrapper, 0o0755)

 
    def teardown(self):
        for f in (self.waitWrapper, self.exitWrapper):
            os.unlink(f)

    def test_basicKiller(self):
        p = subprocess.Popen(['sleep', '100'])
#         print subprocess.check_output(['pstree', '-p', pwd.getpwuid(os.getuid())[0]])      
#         print subprocess.check_output(['ps', 'ax', '-o', 'pid,ppid,pgid,args', '-m'])
        myWeans = listChildren(listOrphans = False)
        self.assertEqual(len(myWeans), 1)
        self.assertEqual(myWeans[0], p.pid)
        infanticide(myWeans)
        p.wait()

    def test_multiKiller(self):
        p = subprocess.Popen(["./{0}".format(self.waitWrapper)])
        time.sleep(10) # Better let children get started!
#         print subprocess.check_output(['pstree', '-p', pwd.getpwuid(os.getuid())[0]])        
        myWeans = listChildren(listOrphans = False)
#         print myWeans
        self.assertEqual(len(myWeans), 2)
        self.assertEqual(myWeans[1], p.pid)
        infanticide(myWeans)
        p.wait()
        myWeans = listChildren(listOrphans = False)
#         print myWeans
#         print subprocess.check_output(['ps', 'guxw'])
        self.assertEqual(len(myWeans), 0)
        
class TestValgrindCommand(unittest.TestCase):
    def test_valgrindarguments(self):
        vgc=ValgrindCommand()
        self.assertTrue(vgc.startswith('valgrind'))
        
if __name__ == '__main__':
    unittest.main()
