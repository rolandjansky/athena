#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Run a Reco job and test key metadata in the output, sources on EOS
#
# This test only makes ESD, as that is sufficient to test the bytestream
# access
# $Id: test_Reco_EOS_tf.py 604373 2014-07-01 09:32:30Z graemes $
#

import glob
import json
import subprocess
import os
import os.path
import sys
import unittest

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfReports import pyJobReportToFileDict

sourceFile = 'root://eosatlas.cern.ch//eos/atlas/user/g/graemes/data12_8TeV.00211620.physics_ZeroBias.merge.RAW/data12_8TeV.00211620.physics_ZeroBias.merge.RAW._lb0916._SFO-ALL._0001.1'

class Reco_tftest(unittest.TestCase):
    
    def test_runReco_tf(self):
        cmd = ['Reco_tf.py', '--inputBSFile']
        cmd.append(sourceFile)
        cmd.extend(['--outputESDFile', 'my.ESD.pool.root', '--autoConfiguration', 'everything'])
        cmd.extend(['--maxEvents', '10'])
        cmd.extend(['--preExec', 'rec.doTrigger=False'])  # This is temporary while trigger doesn't work in r19
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
            self.assertTrue('ESD' in dataDict.keys())
            self.assertEqual(dataDict['ESD']['subFiles'][0]['nentries'], 10)
            self.assertEqual(dataDict['ESD']['subFiles'][0]['geometry'], 'ATLAS-GEO-20-00-01')
            self.assertEqual(dataDict['ESD']['subFiles'][0]['conditions_tag'], 'COMCOND-BLKPA-006-01')
            self.assertEqual(dataDict['ESD']['subFiles'][0]['beam_type'], ['collisions'])
            self.assertEqual(dataDict['ESD']['subFiles'][0]['name'], 'my.ESD.pool.root')
            

if __name__ == '__main__':
    unittest.main()
