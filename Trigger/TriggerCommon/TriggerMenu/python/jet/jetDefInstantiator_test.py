# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from mock import MagicMock
from jetDefInstantiator import Instantiator

class Test_jetDefInstantiator(unittest.TestCase):

    def test_0(self):
        """test instantiation and caching if instatiation is ok"""

        alg = MagicMock()
        alg.asString = MagicMock(return_value = 'DummyAlgo("RoiCreator")')
        inst = Instantiator()
        self.assertTrue(len(inst.cache) == 0)
        self.assertTrue(
            inst(alg).__class__.__name__ == 'PESA__DummyUnseededAllTEAlgo')
        self.assertTrue(len(inst.cache) == 1)
        inst(alg)
        self.assertTrue(
            inst(alg).__class__.__name__ == 'PESA__DummyUnseededAllTEAlgo')
        

    def test_1(self):
        """test instantiation if instatiation is fails"""

        alg = MagicMock()
        alg.asString = MagicMock(return_value = 'Junk')
        inst = Instantiator()
        self.assertRaises(RuntimeError, inst, alg)


if __name__ == '__main__':
    unittest.main()
