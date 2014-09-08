# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from InstantiatorFactory import instantiatorFactory


class TestInstantiatorFactory(unittest.TestCase):

    def test_0(self):
        """Create an ATLAS config object instantiator"""

        use_atlas_config = True
        i = instantiatorFactory(use_atlas_config)
        self.assertTrue(i.__class__.__name__ == 'Instantiator')

    def test_1(self):
        """Create an Algroithm to string converter"""

        use_atlas_config = False
        i = instantiatorFactory(use_atlas_config)
        self.assertTrue(
            i.__class__.__name__ == 'JetDefAlgToStringInstantiator')

if __name__ == '__main__':
    unittest.main()
