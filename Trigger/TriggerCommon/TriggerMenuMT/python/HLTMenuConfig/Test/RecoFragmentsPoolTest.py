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

        RecoFragmentsPool.retrieve(creator, flags=None, name='A')

    def test_ignoreFlags(self):
        """Test ignoring of flags"""
        flags1 = AthConfigFlags()
        flags1.addFlag('Flag', 1)

        flags2 = AthConfigFlags()
        flags2.addFlag('Flag', 2)

        f1 = RecoFragmentsPool.retrieve(creator, flags1, name='A')
        f2 = RecoFragmentsPool.retrieve(creator, flags2, name='A')
        self.assertEqual(f1, f2)

    def test_cache(self):
        """Test caching of fragments"""
        f1 = RecoFragmentsPool.retrieve(creator, flags=None, name='A')
        f2 = RecoFragmentsPool.retrieve(creator, flags=None, name='B')
        self.assertNotEqual(f1, f2)

        f1 = RecoFragmentsPool.retrieve(creator, flags=None, name='A')
        f2 = RecoFragmentsPool.retrieve(creator2, flags=None, name='A')
        self.assertNotEqual(f1, f2)

    def test_namespace(self):
        """Test nested namespaces"""
        f1 = RecoFragmentsPool.retrieve(MyClass1.creator, flags=None, name='A')
        f2 = RecoFragmentsPool.retrieve(MyClass2.creator, flags=None, name='A')
        self.assertNotEqual(f1, f2)

        # same creator function name in different modules:
        f1 = RecoFragmentsPool.retrieve(creator, flags=None, name='A')
        f2 = RecoFragmentsPool.retrieve(TestChainConfiguration.creator, flags=None, name='A')
        self.assertNotEqual(f1, f2)

    def test_lock(self):
        """Test locking of returned fragments"""
        ch, im, tr, th = RecoFragmentsPool.retrieve(creatorDH, flags=None)
        self.assertFalse(ch.isLocked())
        self.assertFalse(im.isLocked())
        self.assertTrue(tr.isLocked())
        self.assertFalse(th.isLocked())


if __name__ == '__main__':
    unittest.main()
