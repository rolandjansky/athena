# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Test import TestChainConfiguration
from AthenaConfiguration.AthConfigFlags import AthConfigFlags

from DecisionHandling.DecisionHandlingConf import ComboHypo, InputMakerForRoI, HLTTest__TestRecoAlg, HLTTest__TestHypoAlg

import unittest

def creator(flags, name):
    return 'creator_'+name

def creator2(flags, name):
    return 'creator2_'+name

def creatorDH(flags):
    return ComboHypo(), InputMakerForRoI(), HLTTest__TestRecoAlg(), HLTTest__TestHypoAlg()

def emptyFlags():
    flags = AthConfigFlags()
    flags.lock()
    return flags

class MyClass1:
    @classmethod
    def creator(cls, flags, name):
        return 'creator_'+name

class MyClass2:
    @classmethod
    def creator(cls, flags, name):
        return 'creator2_'+name

class RecoFragmentsPoolTest(unittest.TestCase):

    def test_flagMandatory(self):
        """Test for mandatory flags"""
        with self.assertRaises(TypeError):
            RecoFragmentsPool.retrieve(creator, name='A')
        with self.assertRaises(TypeError):
            RecoFragmentsPool.retrieve(creator, flags=None, name='A')

        RecoFragmentsPool.retrieve(creator, emptyFlags(), name='A')

    def test_ignoreFlags(self):
        """Test ignoring of flags"""
        flags1 = AthConfigFlags()
        flags1.addFlag('Flag', 1)
        flags1.lock()

        flags2 = AthConfigFlags()
        flags2.addFlag('Flag', 2)
        flags2.lock()

        f1 = RecoFragmentsPool.retrieve(creator, flags1, name='A')
        f2 = RecoFragmentsPool.retrieve(creator, flags2, name='A')
        self.assertEqual(f1, f2)

    def test_cache(self):
        """Test caching of fragments"""
        flags = emptyFlags()
        f1 = RecoFragmentsPool.retrieve(creator, flags, name='A')
        f2 = RecoFragmentsPool.retrieve(creator, flags, name='B')
        self.assertNotEqual(f1, f2)

        f1 = RecoFragmentsPool.retrieve(creator, flags, name='A')
        f2 = RecoFragmentsPool.retrieve(creator2, flags, name='A')
        self.assertNotEqual(f1, f2)

    def test_namespace(self):
        """Test nested namespaces"""
        flags = emptyFlags()
        f1 = RecoFragmentsPool.retrieve(MyClass1.creator, flags, name='A')
        f2 = RecoFragmentsPool.retrieve(MyClass2.creator, flags, name='A')
        self.assertNotEqual(f1, f2)

        # same creator function name in different modules:
        f1 = RecoFragmentsPool.retrieve(creator, flags, name='A')
        f2 = RecoFragmentsPool.retrieve(TestChainConfiguration.creator, flags, name='A')
        self.assertNotEqual(f1, f2)

    def test_lock(self):
        """Test locking of returned fragments"""
        ch, im, tr, th = RecoFragmentsPool.retrieve(creatorDH, emptyFlags())
        self.assertFalse(ch.isLocked())
        self.assertFalse(im.isLocked())
        self.assertTrue(tr.isLocked())
        self.assertFalse(th.isLocked())


if __name__ == '__main__':
    unittest.main()
