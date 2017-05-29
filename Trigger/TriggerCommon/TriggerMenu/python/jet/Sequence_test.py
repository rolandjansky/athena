# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest

from Sequence import Sequence


class Alg(object):
    pass


class Instantiator(object):
    def __call__(self, alg):
        return 'instantiated_alg'


class TestSequence(unittest.TestCase):

    def setUp(self):
        self.alg_list = [Alg(), Alg()]
        self.sequence = Sequence('te_in', self.alg_list, 'alias', 'te_out')

    def test_0(self):

        # excercise __str__
        #s = str(self.sequence)

        instantiator = Instantiator()
        self.sequence.instantiateAlgs(instantiator)

        expected = ['instantiated_alg', 'instantiated_alg']
        self.assertTrue(self.sequence.alg_list == expected)


if __name__ == '__main__':
    unittest.main()
