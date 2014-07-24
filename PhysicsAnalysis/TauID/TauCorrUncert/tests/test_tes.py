# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from os import path
from glob import glob
import unittest
import ROOT
from . import get_file, SHARE_DIR, TauCorrUncert as TCU


class TESTest(unittest.TestCase):
    """
    class defining test for TES uncertainty
    """
    def setUp(self):

        self.tes = TCU.TESUncertainty(path.join(
            SHARE_DIR, 'TES',
            'mc12_p1344_medium.root'))

    def testAllInputs(self):
        for inputfile in glob(path.join(SHARE_DIR, 'TES', '*.root')):
            tes = TCU.TESUncertainty(inputfile)
            tes.GetTESUncertainty(30000., 1.8, 1)

    def testBounds(self):
        # eta out of bounds
        assert self.tes.GetTESUncertainty(30000., 2.6, 3) == 1.

    def testDefaultBehaviour(self):
        self.assertEqual(self.tes.GetTESUncertainty(30000., 1.6, 1),
                         self.tes.GetTESUncertainty(30000., 1.6, 1, TCU.TESUncertainty.FINAL))

if __name__ == '__main__':
    suite1 = unittest.TestLoader().loadTestsFromTestCase(TESTest)
    for suite in [suite1]:
        unittest.TextTestRunner(verbosity=2).run(suite)
