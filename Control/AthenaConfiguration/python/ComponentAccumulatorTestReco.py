# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Configurable import Configurable
import unittest

Configurable.configurableRun3Behavior=1
class TestDataDependenciesCollection( unittest.TestCase ):
    def setUp(self):
        ca = ComponentAccumulator()
        alg = CompFactory.TauProcessorAlg("Algo") # use this one as example because it has rich I/O
        alg.Key_jetInputContainer = "TestJets"
        alg.CellMakerTool = CompFactory.CaloCellTimeCorrTool("TestPrivateTool", Folder="TestFolderA")
        alg.ExtraInputs = ["ImaginedExtraType#ExInput"]
        alg.ExtraOutputs = ["ImaginedExtraType#ExOutput"]

        ca.addEventAlgo(alg)
        ca.addPublicTool(CompFactory.CaloCellTimeCorrTool("SomeOtherTool", Folder="TestFolderB"))
        self.io = ca.getIO()
        import pprint
        pprint.pprint(self.io)

    def test_input(self):
        testJets = [ e for e in self.io if e["key"] == "TestJets" ]
        self.assertEqual(len(testJets), 1, "Not found the set value")
        testTaus = [ e for e in self.io if e["key"] == "StoreGateSvc+tmp_TauJets" ] #default set in c++ header
        self.assertEqual( len(testTaus), 1, "Not found default valued handle")

    def test_output(self):
        testJets = [ e for e in self.io if e["key"] == "TestJets" ]
        self.assertEqual(testJets[0]["mode"], "R", "Not found the set value")
        testTaus = [ e for e in self.io if e["key"] == "StoreGateSvc+tmp_TauJets" ]
        self.assertEqual( testTaus[0]["mode"], "W", "Wrong mode")

    def test_extra(self):
        e = [e for e in self.io if e ["key"] == "ExInput"][0]
        self.assertEqual(e["mode"], "R", "Wrong mode")
        self.assertEqual(e["prop"], "ExtraInputs", "Wrong prop")
        self.assertEqual(e["type"], "ImaginedExtraType", "Wrong type")
        e = [e for e in self.io if e ["key"] == "ExOutput"][0]
        self.assertEqual(e["mode"], "W", "Wrong mode")
        self.assertEqual(e["prop"], "ExtraOutputs", "Wrong prop")
        self.assertEqual(e["type"], "ImaginedExtraType", "Wrong type")

    def test_tools(self):
        priv = [e for e in self.io if e["key"] == "TestFolderA"]
        self.assertEqual(len(priv), 1, "Missing private tools props")
        pub = [e for e in self.io if e["key"] == "TestFolderB"]
        self.assertEqual(len(pub), 1, "Missing public tools props")

if __name__ == "__main__":
    unittest.main()