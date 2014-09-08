# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
INCOMPLETE
"""

import copy
import unittest
from mock import MagicMock
from fex_factory import (fex_factory,
                         JetFex,
                         AntiKt,
                         Cone,
                         JetRec,)


class Test_fex_factory(unittest.TestCase):

    def setUp(self):

        jet_attribute = MagicMock()
        jet_attribute.threshold = 50
        jet_attribute.region = 'j'
        
        self.fex_args = {
            'merge_param': 4,
            'calib': 'calib',
            }

    def test_0(self):
        """test HypoAlg construction"""

        fex = JetFex(self.fex_args)
        self.assertTrue(fex.__class__.__name__ == 'HypoAlg')
        str(fex)  # exercise string method

    def test_20(self):
        """test factory function"""

        fex = fex_factory('antikt', self.fex_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetRecHypoAlg')

        fex = fex_factory('cone', self.fex_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetRecHypoAlg')

        fex = fex_factory('jetrec', self.fex_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetRecHypoAlg')


if __name__ == '__main__':
    unittest.main()
