#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Run a Sim job and test output metadata
# $Id: test_FullChain_tf.py 747041 2016-05-13 14:29:48Z lerrenst $
#

import json
import subprocess
import sys
import unittest

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfReports import pyJobReportToFileDict

sourceFile = '/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root'

class SimTTBar_tftest(unittest.TestCase):
    
    def test_runReco_tf(self):
        cmd = ['FullChain_tf.py', '--inputEVNTFile', sourceFile]
        cmd.extend(['--outputAODFile', 'test.AOD.pool.root'])
        cmd.extend(['--geometryVersion', 'ATLAS-GEO-20-00-01'])
        cmd.extend(['--conditionsTag', 'OFLCOND-MC12-SIM-00'])
        cmd.extend(['--randomSeed', '10'])
        cmd.extend(['--skipEvents', '0'])
        cmd.extend(['--maxEvents', '2']) # 2 events - this is a slow job in G4
        ## Event counting currently broken for multi-step transforms
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
            dataDict = pyJobReportToFileDict(md)
            self.assertEqual(dataDict["AOD"]['subFiles'][0]['nentries'], 2) 
            # Outdated. argValues ar not a part of jobReport anymore
            #self.assertEqual(dataDict["AOD"]['subFiles'][0]['geometry'], 'ATLAS-GEO-20-00-01')
            #self.assertEqual(dataDict["AOD"]['subFiles'][0]['conditions_tag'], 'OFLCOND-MC12-SIM-00')

if __name__ == '__main__':
    unittest.main()
