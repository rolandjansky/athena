# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
INCOMPLETE
"""

import unittest
from mock import MagicMock
from fexparams_factory import fexparams_factory


class Test_fexparams_factory(unittest.TestCase):

    def setUp(self):

        jet_attribute = MagicMock()
        jet_attribute.threshold = 50
        jet_attribute.region = 'j'
        
        self.fex_args = {
            'merge_param': 4,
            'jet_calib': 'subjes',
            'fex_label': 'fex_label',
            'level': 'tc',
            'data_type': 'tc',
            'fex_alg_name': 'a4',
            }

    def test_20(self):
        """test factory function"""

        fexparams = fexparams_factory('antikt', self.fex_args)
        self.assertTrue(fexparams.__class__.__name__ == '_AntiKtParams')

        fexparams = fexparams_factory('cone', self.fex_args)
        self.assertTrue(fexparams.__class__.__name__ == '_ConeParams')

        fexparams = fexparams_factory('jetrec', self.fex_args)
        self.assertTrue(fexparams.__class__.__name__ == '_JetRecParams')


if __name__ == '__main__':
    unittest.main()
