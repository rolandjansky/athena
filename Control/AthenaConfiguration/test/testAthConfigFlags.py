#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AccumulatorCache import AccumulatorCache
from AthenaConfiguration.Enums import Format

import unittest

class FlagsSetup(unittest.TestCase):
    def setUp(self):
        self.flags = AthConfigFlags()
        self.flags.addFlag("Atest", True)
        self.flags.addFlag("A.One", True)
        self.flags.addFlag("A.B.C", False)
        self.flags.addFlag("A.dependentFlag", lambda prevFlags: ["FALSE VALUE", "TRUE VALUE"][prevFlags.A.B.C] )


class BasicTests(FlagsSetup):

    def test_Access(self):
        """Test access"""
        self.assertFalse( self.flags.A.B.C, "Can't read A.B.C flag")
        self.flags.A.B.C = True
        self.assertTrue( self.flags.A.B.C, "Flag value not changed")

    def test_wrongAccess(self):
        """Access to the flag that are missnames should give an exception"""
        with self.assertRaises(RuntimeError):
            print(".... test printout {}".format( self.flags.A is True ))
            print(".... test printout {}".format( self.flags.A.B == 6 ))

    def test_dependentFlag(self):
        """The dependent flags will use another flag value to establish its own value"""
        self.flags.A.B.C = True
        self.flags.lock()
        self.assertEqual(self.flags.A.dependentFlag, "TRUE VALUE", " dependent flag setting does not work")

    def test_lock(self):
        """Test flag locking"""
        self.flags.lock()
        with self.assertRaises(RuntimeError):
            self.flags.Atest = False
        with self.assertRaises(RuntimeError):
            self.flags.addFlag("X", True)

    def test_hash(self):
        """Test flag hashing"""
        self.assertEqual(self.flags.locked() , False)
        with self.assertRaises(RuntimeError):
            self.flags.athHash()
        self.flags.lock()
        self.assertIsNotNone(self.flags.athHash())
        self.assertEqual(self.flags.locked() , True)

        @AccumulatorCache
        def testAthHash(flags , number , key = 123):
            return number + key

        a = testAthHash(self.flags , 123 , key = 321)
        b = testAthHash(self.flags , 321 , key = 123)
        c = testAthHash(self.flags , 123 , key = 321)
        d = testAthHash(self.flags , 321 , key = 123)

        self.assertEqual(a , c)
        self.assertEqual(b , d)

        cacheInfo = testAthHash.getInfo()

        self.assertEqual(cacheInfo["misses"] , 2)
        self.assertEqual(cacheInfo["hits"] , 2)
        self.assertEqual(cacheInfo["cache_size"] , 2)

    def test_hash_invariance(self):
        """Test that hash doesn't change on dynamic flag loading"""
        def generator():
            extraFlags = AthConfigFlags()
            extraFlags.addFlag('Extra.X', 'foo')
            extraFlags.addFlag('Extra.Y', lambda flags : flags.Extra.X+'_bar')
            return extraFlags

        self.flags.addFlagsCategory('Extra', generator)
        self.flags.lock()
        hash_value = self.flags.athHash()
        self.assertEqual(self.flags.Extra.X, 'foo')
        self.assertEqual(self.flags.athHash() , hash_value)
        self.assertEqual(self.flags.Extra.Y, 'foo_bar')
        self.assertEqual(self.flags.athHash() , hash_value)

    def test_enums(self):
        """Test that enums are properly validated"""
        self.flags.addFlag("Format", Format.BS, enum=Format)
        self.flags.addFlag("FormatFun", lambda flags : Format.POOL if flags.Atest else Format.BS, enum=Format)
        self.flags.addFlag("FormatPOOL", Format.BS, enum=Format)
        self.flags.FormatPOOL = Format.POOL
        self.flags.lock()

    def test_enums_incorrect_assign(self):
        """Test that enums are properly validated (incorrect flags)"""
        self.flags.addFlag("FormatWrong", Format.BS, enum=Format)
        with self.assertRaises(Exception) as _:
            self.flags.FormatWrong == "BS"

        with self.assertRaises(Exception) as _:
            self.flags.FormatWrong = "POOL"

    def test_enums_incorrect_lambda(self):
        """Test that enums are properly validated (incorrect flags)"""
        self.flags.addFlag("FormatWrong", lambda flags : "ABC", enum=Format)
        with self.assertRaises(RuntimeError) as _:
            x=self.flags.FormatWrong 


class TestFlagsSetupDynamic(FlagsSetup):
    def setUp(self):
        super().setUp()

        def theXFlags():
            nf = AthConfigFlags()
            nf.addFlag("a", 17)
            nf.addFlag("b", 55)
            nf.addFlag("c", "Hello")
            return nf

        def theZFlags():
            nf = AthConfigFlags()
            nf.addFlag("Z.A", 7)
            nf.addFlag("Z.B", True)
            nf.addFlag("Z.C.setting", 99)
            nf.addFlagsCategory( 'Z.Xclone1', theXFlags, prefix=True )
            nf.addFlagsCategory( 'Z.Xclone2', theXFlags, prefix=True )
            return nf

        def theTFlags():
            nf = AthConfigFlags()
            nf.addFlag("T.Abool", False)
            return nf

        self.flags.addFlagsCategory( 'Z', theZFlags )
        self.flags.addFlagsCategory( 'X', theXFlags, prefix=True )
        self.flags.addFlagsCategory( 'T', theTFlags )
        print("\nFlag values before test:")
        print("-"*80)
        self.flags.dump()
        print("-"*80)

    def tearDown(self):
        print("\nFlag values after test:")
        print("-"*80)
        self.flags.dump()
        print("-"*80)

    def test_dynamicFlagsRead(self):
        """Check if dynamic flags reading works"""
        self.assertEqual( self.flags.X.a, 17, "dynamically loaded flags have wrong value")
        print("")
        self.assertEqual( self.flags.Z.A, 7, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.Xclone1.b, 55, "dynamically loaded flags have wrong value")
        self.flags.Z.Xclone2.b = 56
        self.assertEqual( self.flags.Z.Xclone2.b, 56, "dynamically loaded flags have wrong value")

    def test_dynamicFlagsSet(self):
        """Check if dynamic flags setting works"""
        self.flags.Z.A = 15
        self.flags.Z.Xclone1.a = 20
        self.flags.X.a = 30
        self.assertEqual( self.flags.Z.Xclone1.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.X.a, 30, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.A, 15, "dynamically loaded flags have wrong value")

    def test_overwriteFlags(self):
        """Check if overwriting works"""
        self.flags.Z.Xclone1.a = 20
        self.flags.Z.Xclone2.a = 40

        self.flags.X.a = 30
        copyf = self.flags.cloneAndReplace( "X", "Z.Xclone1")
        self.assertEqual( copyf.X.a, 20, "dynamically loaded flags have wrong value")
#        self.assertEqual( copyf.T.Abool, False, "The flags clone does not have dynamic flags")
        copyf.dump()

        self.flags.lock()
        copyf = self.flags.cloneAndReplace( "X", "Z.Xclone2")
        self.assertEqual( copyf.X.a, 40, "dynamically loaded flags have wrong value")
        self.assertEqual( copyf.T.Abool, False, "The flags clone does not have dynamic flags")


class TestDynamicDependentFlags(unittest.TestCase):
    def test(self):
        """Check if dynamic dependent flags work"""
        flags = AthConfigFlags()
        flags.addFlag("A", True)
        flags.addFlag("B", lambda prevFlags: 3 if prevFlags.A is True else 10 )
        flags.addFlag("C", lambda prevFlags: 'A' if prevFlags.A is True else 'B' )
        assert flags.B == 3
        flags.A = False
        assert flags.B == 10
        flags.A = True
        flags.lock()
        assert flags.C == 'A'
        print("")


class FlagsFromArgsTest(unittest.TestCase):
    def setUp(self):
        self.flags = AthConfigFlags()
        self.flags.addFlag('Exec.OutputLevel',3) #Global Output Level
        self.flags.addFlag('Exec.MaxEvents',-1)
        self.flags.addFlag("Exec.SkipEvents",0)
        self.flags.addFlag("Exec.DebugStage","")
        self.flags.addFlag('Input.Files',[])
        self.flags.addFlag('detA.flagB',0)
        self.flags.addFlag("detA.flagC","")

    def test(self):
        argline="-l VERBOSE --debug exec --evtMax=10 --skipEvents=3 --filesInput=bla1.data,bla2.data detA.flagB=7 detA.flagC=a.2"
        print (f"Interpreting arguments: '{argline}'")
        self.flags.fillFromArgs(argline.split())
        self.assertEqual(self.flags.Exec.OutputLevel,1,"Failed to set output level from args")
        self.assertEqual(self.flags.Exec.MaxEvents,10,"Failed to set MaxEvents from args")
        self.assertEqual(self.flags.Exec.SkipEvents,3,"Failed to set SkipEvents from args")
        self.assertEqual(self.flags.Exec.DebugStage,"exec","Failed to set DebugStage from args")
        self.assertEqual(self.flags.Input.Files,["bla1.data","bla2.data"],"Failed to set FileInput from args")
        self.assertEqual(self.flags.detA.flagB,7,"Failed to set arbitrary from args")
        self.assertEqual(self.flags.detA.flagC,"a.2","Failed to set arbitrary unquoted string from args")



if __name__ == "__main__":
    unittest.main(verbosity=2)
