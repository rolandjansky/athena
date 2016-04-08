# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import os, sys

class DQCTestCase(unittest.TestCase):
    def setUp(self):
        self.modlist = []

    def do_imports(self, verbose=False):
        import DataQualityConfigurations
        for i in DataQualityConfigurations.__all__:
            if verbose: print 'Importing', i
            modname = 'DataQualityConfigurations.%s' % i
            __import__(modname)
            self.modlist.append(sys.modules[modname])
        
    def test_01_ConfigsReadable(self):
        '''Test that we can read the python config files ok'''
        self.do_imports(verbose=True)
        self.assert_(len(self.modlist) > 0, 'Should have more than one config module')

    def test_02_ConfigsHaveConfigs(self):
        self.do_imports()
        for mod in self.modlist:
            self.assert_(mod.dqconfig)

    def test_03_ConfigsPointAtGoodFiles(self):
        self.do_imports()
        for mod in self.modlist:
            for f in ('hcfg', 'hcfg_min10', 'hcfg_min30'):
                mf = getattr(mod.dqconfig, f)
                if mf != '':
                    self.failUnless(os.access(mf, os.R_OK),
                                    'File %s not readable in configuration %s' % (mf, mod.__name__) )
                    self.failUnless('/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/' in mf,
                                    'Invalid location %s for a production hcfg file in %s' % (mf, mod.__name__) )

    def test_04_ModuleHandling(self):
        self.do_imports()
        from DataQualityConfigurations import getmodule
        # OK for modules that should explicitly be around?
        for mod in self.modlist:
            self.assertTrue(getmodule(mod.__name__.replace('DataQualityConfigurations.', '')))

        # OK for some modules that do not exist?
        for mname in ('data15_hi',
                      'data15_hip',
                      'data15_900GeV',
                      'data15_2p76TeV',
                      'data15_5TeV',
                      'data15_7TeV',
                      'data15_8TeV',
                      'data15_13TeV',
                      'data15_1beam',
                      'data15_cos',
                      'data15_calib',
                      'data15_calocomm',
                      'data15_larcomm',
                      'data15_tilecomm',
                      'data15_muoncomm',
                      'data15_idcomm',
                      'data15_comm',
                      'data15',
                      ):
            self.assertTrue(getmodule(mname))

        # Fails otherwise?
        self.assertRaises(ValueError, getmodule, 'NonExistentPtag')
        
def suite():
    suite = unittest.TestLoader().loadTestsFromTestCase(DQCTestCase)
    return suite

if __name__ == '__main__':
    unittest.main()
