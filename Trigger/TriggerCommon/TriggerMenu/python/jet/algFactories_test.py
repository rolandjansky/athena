# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from mock import MagicMock
from algFactories import (alg_dispatcher,
                          Alg)


def make_menu_data(k):
    mult_1 = ('jr_hypo_single',
              'l15_hypo_implicit0',
              'l15_hypo_implicit1',
              'cellDiagnostics',
              'clusterDiagnostics',
              'jetRecDiagnostics',
              'jetRecDiagnostics',
              'jetHypoDiagnostics',
              'roiDiagnostics',
              )

    mult_many = ('l2_hypo_implicit1',
                 'jr_hypo_multi',
                 )

    def _make_ja():
        ja = MagicMock()
        ja.region = 'j'
        return ja

    menu_data = MagicMock()
    menu_data.hypo = MagicMock()
    menu_data.hypo.jet_attributes = [_make_ja()]
    menu_data.hypo.region.return_value = 'j'

    if k in mult_1:
        menu_data.hypo.jet_attributes = [_make_ja()]
        return menu_data

    if k in mult_many:
        menu_data.hypo.jet_attributes = [_make_ja(), _make_ja()]
        return menu_data

    return None


class TestAlgFactories(unittest.TestCase):

    def test_0(self):

        for k, v in alg_dispatcher.items():
            menu_data = make_menu_data(k)

            if menu_data:
                a = v(menu_data)
            else:
                a = v()
            self.assertTrue(isinstance(a, Alg))

if __name__ == '__main__':
    unittest.main()
