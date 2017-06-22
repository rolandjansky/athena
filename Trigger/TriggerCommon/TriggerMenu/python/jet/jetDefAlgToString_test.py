# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from AlgFactory import Alg
from jetDefAlgToString import JetDefAlgToStringInstantiator


class TestJetDefAlgToString(unittest.TestCase):

    def setUp(self):
        self.instantiator = JetDefAlgToStringInstantiator()

    def test_0(self):
        ffn = 'factory_fn'
        alg = Alg(ffn, (), {})
        print alg.asString()
        self.assertTrue(self.instantiator(alg) == ffn + '()')

    def test_1(self):
        alg = 'alg'  # wrong type
        self.assertRaises(AttributeError, self.instantiator, alg)


if __name__ == '__main__':
    unittest.main()
