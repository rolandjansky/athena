#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Run a Reco job and test key metadata in the output, sources on EOS
#
# This test only makes ESD, as that is sufficient to test the bytestream
# access
# $Id: test_Reco_EOS_tf.py 573368 2013-12-02 18:56:04Z graemes $
#

import glob
import json
import subprocess
import os
import os.path
import cPickle as pickle
import sys
import unittest

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfReports import pyJobReportToFileDict

sourceFile = 'root://eosatlas.cern.ch//eos/atlas/user/g/graemes/data12_8TeV.00211620.physics_ZeroBias.merge.RAW/data12_8TeV.00211620.physics_ZeroBias.merge.RAW._lb0916._SFO-ALL._0001.1'

class RecoTier0test(unittest.TestCase):
    
    def test_Reco_Tier0_tf(self):
        pFile = 'job.pkl'
        
        cmd = "Reco_tf.py --inputBSFile /afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1 --maxEvents 5 --autoConfiguration everything --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'rec.doCalo=False' 'rec.doInDet=False' 'rec.doMuon=False' 'rec.doJetMissingETTag=False' 'rec.doEgamma=False' 'rec.doMuonCombined=False' 'rec.doTau=False' 'rec.doTrigger=False' --outputESDFile myESD.pool.root --dumpPickle {0}".format(pFile).split()
        msg.info('Will run this transform: {0}'.format(cmd))
        p = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
        while p.poll() is None:
            line = p.stdout.readline()
            sys.stdout.write(line)
        # Hoover up remaining buffered output lines
        for line in p.stdout:
            sys.stdout.write(line)
        self.assertEqual(p.returncode, 0)
        
        # Check the pickle was produced
        self.assertEqual(os.access(pFile, os.R_OK), True)
        unpickleFile = open(pFile, 'r')
        contents = pickle.load(unpickleFile)
        unpickleFile.close()
        self.assertEqual(isinstance(contents, dict), True)
        
        # Now configure and run the transform from the pickle file
        cmd = "Reco_tf.py --argdict {0} --outputESDFile newESD.pool.root".format(pFile).split()
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
            self.assertEqual(dataDict['ESD']['subFiles'][0]['nentries'], 5)
            self.assertEqual(dataDict['ESD']['subFiles'][0]['name'], 'newESD.pool.root')
            

if __name__ == '__main__':
    unittest.main()
