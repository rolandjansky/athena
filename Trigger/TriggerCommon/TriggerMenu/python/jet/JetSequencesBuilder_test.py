# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from mock import MagicMock

from JetSequencesBuilder import (AlgList,
                                 JetSequencesBuilder)


class Alg(object):
    pass


class Instantiator(object):
    def __call__(self, alg):
        return 'instantiated_alg'


class TestJetSequencesBuilder(unittest.TestCase):

    def setUp(self):

        class AntiKt:
            def __init__(self):
                pass

        jr_menudata = MagicMock()
        jr_menudata.fex = AntiKt()
        jr_menudata.fex.fex_key = ''
        
        jr_menudata.scantype = 'FS'
        chain_config = MagicMock()
        chain_config.jr_menudata = jr_menudata
        chain_config.jr_menudata.scan_type = ''
        chain_config.tt_menudata = None
        
        alg_factory = MagicMock()
        self.router = JetSequencesBuilder(alg_factory, chain_config)

        # legal combinations of trigger tower and jet rec
        # menu data objects

        self.implemented = (
            (True, True),  # tt, jr
            (False, True),
            )


    def test_0(self):
        'Excercise AlgList creation'
        a = AlgList(alias='alias', alg_list=[])
        self.assertTrue(a.alias == 'alias')
        self.assertTrue(a.alg_list == [])
    #
    # def test_1(self):
    #    'test that illegal MenuData objects combinatations raise an exception'
    #
    #    tf = (True, False)
    #
    #    chain_config = MagicMock()
    #    for tt in tf:
    #        for jr in tf:
    #            chain_config.tt_menudata = tt
    #            chain_config.jr_menudata = jr
    #
    #            if (tt, jr) not in self.implemented:
    #                self.assertRaises(RuntimeError,
    #                                  self.router.make_alglists,
    #                                  chain_config,
    #                                  )
    #            elif (tt, jr):
    #                pass
                         
    def _test_2(self):
        'test that legal MenuData objects combinatations'

        #2017-05-22 commenting out because chain_config is undefined and causes an error with flake8 unit testing
        #self.router.make_alglists(chain_config)

if __name__ == '__main__':
    unittest.main()
