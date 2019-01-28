# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# None of this works, but keep it around in case someone wants to resurrect it later...
# - PO 20180419

import unittest
import sys, os, shutil

TESTING_DIR = '/afs/cern.ch/user/a/atlasdqm/dqmdisk1/testing_references'

class DQUTestCase(unittest.TestCase):
    def setUp(self):
        self.outfile = ''
        
    def test_01_Merging(self):
        '''Test that histogram merging + postprocessing works'''
        print
        print 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
        print 'Running merge test ....'
        print 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
        outdir = os.environ.get('TMPDIR', '.')
        inlist = os.path.join(TESTING_DIR, 'test_merging')
        self.outfile = os.path.join(outdir, 'data09_calophys.00128005.physics_CosmicMuons.root')
        rv = os.system('cd %s ; DQHistogramMerge.py %s %s True' % (outdir, inlist, self.outfile))
        self.failUnless(rv==0, "DQHistogramMerge.py return code is nonzero")
        
    def test02_WebDisplay(self):
        '''Test that a terminal web display job works'''
        print
        print 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
        print 'Running web display test ....'
        print 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
        outdir = os.environ.get('TMPDIR', '.')
        infile = os.path.join(TESTING_DIR, 'data09_calophys.00128005.physics_CosmicMuons.root')
        rv = os.system('cd %s ; DQWebDisplay.py %s TestDisplay 123' % (outdir, infile))
        self.failUnless(rv==0, "DQWebDisplay.py return code is nonzero")
        
    def test_03_WebDisplay(self):
        '''Test that a terminal web display job works in temporary accumulation mode'''
        print
        print 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
        print 'Running intermediate web display test ....'
        print 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
        outdir = os.environ.get('TMPDIR', '.')
        infile = os.path.join(TESTING_DIR, 'data09_calophys.00128005.physics_CosmicMuons.root')
        from DataQualityConfigurations.TestDisplay import dqconfig
        cachedir = dqconfig.histogramCache
        if cachedir == '':
            self.fail('Unable to test intermediate web displays; no histogramCache directory set')
        if not os.access(cachedir, os.W_OK):
            print "No write permissions for cache dir %s; skipping test" % cachedir
            return
        cachefilename = os.path.basename(infile).rsplit('.',1)[-2] + '.CACHE_1'
        shutil.copy(infile, os.path.join(cachedir, cachefilename))

        rv = os.system('cd %s ; DQWebDisplay.py %s TestDisplay 123 True' % (outdir, infile))
        self.failUnless(rv==0, "DQWebDisplay.py return code is nonzero")

    def tearDown(self):
        try:
            if self.outfile != '':
                os.unlink(self.outfile)
        except OSError:
            pass

def suite():
    suite = unittest.TestLoader().loadTestsFromTestCase(DQUTestCase)
    return suite

if __name__ == '__main__':
    unittest.main()
