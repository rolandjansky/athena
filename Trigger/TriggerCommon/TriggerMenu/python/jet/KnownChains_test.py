# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from KnownChains import KnownChains


class TestKnownChains(unittest.TestCase):

    def setUp(self):
        self.known = KnownChains()
        part_d = {'recoType': 'tc',
                  'signature': 'Jet',
                  'locationInfo': 'j',
                  'extra': '',
                  'multiplicity': '1',
                  'FSinfo': '',
                  'locationInfo': 'j',
                  'topo': [],
                  'calib': 'had',
                  'L1item': '',
                  'threshold': '30',
                  'addInfo': [],
                  'chainPartName': 'j30',
                  'recoAlg': 'a4'}

        self.test_d = {'chainParts': [part_d],
                       'chainName': 'chainForKnownChains_test'}

    def test_0(self):
        """Test that incorrect dictinary format raises an exception."""

        part_d = self.test_d['chainParts'][0]
        
        self.assertRaises(RuntimeError, self.known.get_label, part_d)

    def test_1(self):
        """Test successful label finding"""

        label = self.known.get_label(self.test_d)
        self.assertEqual('jetdef_j30', label)

    def test_2(self):
        """Test unsuccessful label finding"""

        self.test_d['chainParts'][0].update({'threshold': '-1'})
        self.assertRaises(RuntimeError, self.known.get_label, self.test_d)


if __name__ == '__main__':
    unittest.main()
