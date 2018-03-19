# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
INCOMPLETE
"""

import unittest
from mock import MagicMock
from fexargs_factory import fexargs_factory


class Test_fexargs_factory(unittest.TestCase):

    def setUp(self):

        jet_attribute = MagicMock()
        jet_attribute.threshold = 50
        jet_attribute.region = 'j'
        
        self.fex_args = {
            'merge_param': 4,
            'calib': {'cluster_dolc': False, 'do_jes': True},
            'fex_label': 'fex_label',
            'level': 'tc',
            'fex_input': 'tc',
            }

    def test_20(self):
        """test factory function"""

        fexargs = fexargs_factory('antikt', self.fex_args)
        self.assertTrue(fexargs.__class__.__name__ == '_AntiKtArgs')

        fexargs = fexargs_factory('cone', self.fex_args)
        self.assertTrue(fexargs.__class__.__name__ == '_ConeArgs')

        fexargs = fexargs_factory('jetrec', self.fex_args)
        self.assertTrue(fexargs.__class__.__name__ == '_JetRecArgs')


if __name__ == '__main__':
    unittest.main()
