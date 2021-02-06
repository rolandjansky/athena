# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

import unittest
class TestFlagsSetup(unittest.TestCase):
    def setUp(self):
        self.flags = AthConfigFlags()
        self.flags.addFlag("Atest", True)
        self.flags.addFlag("A.One", True)
        self.flags.addFlag("A.B.C", False)
        self.flags.addFlag("A.dependentFlag", lambda prevFlags: ["FALSE VALUE", "TRUE VALUE"][prevFlags.A.B.C] )

class TestAccess(TestFlagsSetup):
    def runTest(self):
        print("""... Test access""")
        self.assertFalse( self.flags.A.B.C, "Can't read A.B.C flag")
        self.flags.A.B.C = True
        self.assertTrue( self.flags.A.B.C, "Flag value not changed")


class TestWrongAccess(TestFlagsSetup):
    def runTest(self):
        print("""... Acess to the flag that are missnames should give an exception""")
        with self.assertRaises(RuntimeError):
            print(".... test printout {}".format( self.flags.A is True ))
            print(".... test printout {}".format( self.flags.A.B == 6 ))



class TestDependentFlag(TestFlagsSetup):
    def runTest(self):
        print("""... The dependent flags will use another flag value to establish its own value""")
        self.flags.A.B.C= True
        self.flags.lock()
        self.assertEqual(self.flags.A.dependentFlag, "TRUE VALUE", " dependent flag setting does not work")

class TestFlagsSetupDynamic(TestFlagsSetup):
    def setUp(self):
        super(TestFlagsSetupDynamic, self).setUp()

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
        print("... Setup of dynamic flags loading test")
        self.flags.dump()
        print("")

class TestDynamicFlagsDump(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dump with unloaded flags works""")
        self.flags.dump()
        print("")

class TestDynamicFlagsRead(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dynamic flags reading works""")
        self.assertEqual( self.flags.X.a, 17, "dynamically loaded flags have wrong value")
        print("")
        self.assertEqual( self.flags.Z.A, 7, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.Xclone1.b, 55, "dynamically loaded flags have wrong value")
        self.flags.Z.Xclone2.b = 56
        self.assertEqual( self.flags.Z.Xclone2.b, 56, "dynamically loaded flags have wrong value")
        self.flags.dump()
        print("")

class TestDynamicFlagsSet(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if dynamic flags setting works""")
        self.flags.Z.A = 15
        self.flags.Z.Xclone1.a = 20
        self.flags.X.a = 30
        self.assertEqual( self.flags.Z.Xclone1.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.X.a, 30, "dynamically loaded flags have wrong value")
        self.assertEqual( self.flags.Z.A, 15, "dynamically loaded flags have wrong value")
        self.flags.dump()
        print("")

class TestOverwriteFlags(TestFlagsSetupDynamic):
    def runTest(self):
        print("""... Check if overwiting works""")
        self.flags.Z.Xclone1.a = 20
        self.flags.X.a = 30
        copyf = self.flags.cloneAndReplace( "X", "Z.Xclone1")
        self.assertEqual( copyf.X.a, 20, "dynamically loaded flags have wrong value")
        self.assertEqual( copyf.T.Abool, False, "The flags clone does not have dynamic flags")
        copyf.dump()
        print("")

class TestDynamicDependentFlags(unittest.TestCase):
    def runTest(self):
        print("""... Check if dynamic dependent flags work""")
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

class flagsFromArgsTest(unittest.TestCase):
    def setUp(self):
        self.flags = AthConfigFlags()
        self.flags.addFlag('Exec.OutputLevel',3) #Global Output Level
        self.flags.addFlag('Exec.MaxEvents',-1)
        self.flags.addFlag("Exec.SkipEvents",0)
        self.flags.addFlag("Exec.DebugStage","")
        self.flags.addFlag('Input.Files',[])
        self.flags.addFlag('detA.flagB',0)

    def runTest(self):
        argline="-l VERBOSE --debug exec --evtMax=10 --skipEvents=3 --filesInput=bla1.data,bla2.data detA.flagB=7"
        print ("Interpreting arguments:")
        print (argline)
        self.flags.fillFromArgs(argline.split())
        self.assertEqual(self.flags.Exec.OutputLevel,1,"Failed to set output level from args")
        self.assertEqual(self.flags.Exec.MaxEvents,10,"Failed to set MaxEvents from args")
        self.assertEqual(self.flags.Exec.SkipEvents,3,"Failed to set SkipEvents from args")
        self.assertEqual(self.flags.Exec.DebugStage,"exec","Failed to set DebugStage from args")
        self.assertEqual(self.flags.Input.Files,["bla1.data","bla2.data"],"Failed to set FileInput from args")
        self.assertEqual(self.flags.detA.flagB,7,"Failed to set arbitrary from args")



if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(TestAccess())
    suite.addTest(TestWrongAccess())
    suite.addTest(TestDependentFlag())
    suite.addTest(TestDynamicFlagsDump())
    suite.addTest(TestDynamicFlagsRead())
    suite.addTest(TestDynamicFlagsSet())
    suite.addTest(TestOverwriteFlags())
    suite.addTest(TestDynamicDependentFlags())
    runner = unittest.TextTestRunner(failfast=False)
    runner.run(suite)
