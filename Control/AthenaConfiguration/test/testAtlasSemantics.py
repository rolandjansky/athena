#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import unittest

from GaudiConfig2 import Configurables
from GaudiKernel.DataHandle import DataHandle

from AthenaConfiguration import AtlasSemantics # noqa: F401

class TestVarHandleKey(unittest.TestCase):
   """Test VarHandleKey properties"""

   def test_handle(self):
      alg = Configurables.HiveAlgC()
      alg.Key_R1 = "R1"
      alg.Key_W1 = "W1"
      self.assertIsInstance(alg.Key_R1, DataHandle)
      self.assertIsInstance(alg.Key_W1, DataHandle)
      self.assertEqual(alg.Key_R1, "R1")
      self.assertEqual(alg.Key_R1.type(), "HiveDataObj")
      self.assertFalse(alg.Key_R1.isCondition())
      self.assertEqual(alg.Key_R1.mode(), "R")
      self.assertEqual(alg.Key_W1.mode(), "W")

   def test_handleAssign(self):
      alg = Configurables.HiveAlgC()
      alg.Key_W1 = "W1"
      self.assertIsInstance(alg.Key_W1, DataHandle)
      alg.Key_R1 = alg.Key_W1
      self.assertEqual(alg.Key_R1, "W1")          # key changes
      self.assertEqual(alg.Key_R1.mode(), "R")    # but still a ReadHandle
      self.assertIsInstance(alg.Key_R1.Path, str) # value type remains string

   def test_condHandle(self):
      alg = Configurables.CondAlgZ()
      alg.Key_RCH1 = "R1"
      alg.Key_WCH = "W1"
      self.assertIsInstance(alg.Key_RCH1, DataHandle)
      self.assertIsInstance(alg.Key_WCH, DataHandle)
      self.assertTrue(alg.Key_RCH1.isCondition())
      self.assertEqual(alg.Key_RCH1.mode(), "R")
      self.assertEqual(alg.Key_WCH.mode(), "W")


if __name__ == "__main__":
    unittest.main()
