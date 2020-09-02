# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# self test of ComponentAccumulator

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.CFElements import findSubSequence,findAlgorithm, seqAND, seqOR, parOR, findAllAlgorithms
from AthenaCommon.Configurable import Configurable # guinea pig algorithms
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG, INFO
from AthenaConfiguration.TestDriveDummies import dummyService, dummyTool
import unittest


TestAlgo = CompFactory.HelloAlg


Configurable.configurableRun3Behavior=1
class TestComponentAccumulator( unittest.TestCase ):
    def setUp(self):


        # trivial case without any nested sequences

        log.setLevel(DEBUG)

        dummyCfgFlags=AthConfigFlags()
        dummyCfgFlags.lock()


        def AlgsConf1(flags):
            acc = ComponentAccumulator()
            a1=TestAlgo("Algo1")
            a2=TestAlgo("Algo2")
            return acc,[a1,a2]


        def AlgsConf2(flags):
            acc = ComponentAccumulator()
            result,algs=AlgsConf1( flags )
            acc.merge(result)
            a = TestAlgo("Algo3")
            print("algo3 when created %s" % id(a))
            algs.append(a)
            return acc,algs

        acc = ComponentAccumulator()

        # top level algs
        acc1,algs=AlgsConf2(dummyCfgFlags)
        acc.merge(acc1)
        acc.addEventAlgo(algs)

        def AlgsConf3(flags):
            acc = ComponentAccumulator()
            na1=TestAlgo("NestedAlgo1")
            return acc,na1

        def AlgsConf4(flags):
            acc,na1= AlgsConf3( flags )
            NestedAlgo2 = TestAlgo("NestedAlgo2")
            NestedAlgo2.OutputLevel=7
            return acc,na1,NestedAlgo2

        acc.addSequence( seqAND("Nest") )
        acc.addSequence( seqAND("subSequence1"), parentName="Nest" )
        acc.addSequence( parOR("subSequence2"), parentName="Nest" )

        acc.addSequence( seqAND("sub2Sequence1"), parentName="subSequence1")
        acc.addSequence( seqAND("sub3Sequence1"), parentName="subSequence1")
        acc.addSequence( seqAND("sub4Sequence1"), parentName="subSequence1")

        accNA1=AlgsConf4(dummyCfgFlags)
        acc.merge(accNA1[0])
        acc.addEventAlgo(accNA1[1:],"sub2Sequence1" )
        outf = open("testFile.pkl", "wb")
        acc.store(outf)
        outf.close()
        self.acc = acc


    def test_algorithmsAreAdded( self ):
        self.assertEqual( findAlgorithm( self.acc.getSequence(), "Algo1", 1).name, "Algo1", "Algorithm not added to a top sequence" )
        self.assertEqual( findAlgorithm( self.acc.getSequence(), "Algo2", 1).name,  "Algo2", "Algorithm not added to a top sequence" )
        self.assertEqual( findAlgorithm( self.acc.getSequence(), "Algo3", 1).name, "Algo3", "Algorithm not added to a top sequence" )

    def test_sequencesAreAdded( self ):
        self.assertIsNotNone( self.acc.getSequence("subSequence1" ), "Adding sub-sequence failed" )
        self.assertIsNotNone( self.acc.getSequence("subSequence2" ), "Adding sub-sequence failed" )
        self.assertIsNotNone( self.acc.getSequence("sub2Sequence1"), "Adding sub-sequence failed" )
        self.assertIsNotNone( findSubSequence( self.acc.getSequence("subSequence1"), "sub2Sequence1"), "Adding sub-sequence done in a wrong place" )

    def test_algorithmsInNestedSequences( self ):
        self.assertIsNotNone( findAlgorithm( self.acc.getSequence(), "NestedAlgo1" ), "Algorithm added to nested sequence" )
        self.assertIsNotNone( findAlgorithm( self.acc.getSequence(), "NestedAlgo1", 1 ) is None, "Algorithm mistakenly in top sequence" )
        self.assertIsNotNone( findAlgorithm( findSubSequence( self.acc.getSequence(), "sub2Sequence1"), "NestedAlgo1", 1 ), "Algorithm not in right sequence" )


    def test_readBackConfiguration( self ):
        import pickle
        f = open('testFile.pkl', 'rb')
        s = pickle.load( f )
        self.assertIsNotNone( s, "The pickle has no content")

    def test_foreach_component( self ):
        from AthenaCommon.Logging import logging
        logging.getLogger('foreach_component').setLevel(DEBUG)
        algo3 = self.acc.getEventAlgo("Algo3")
        algo3.OutputLevel = INFO
        self.acc.foreach_component("*/Algo3").OutputLevel = DEBUG # restet to debug level
        self.assertEqual( algo3.OutputLevel, DEBUG, "wrong OutputLevel value for Algo3")
        self.acc.foreach_component("*sub2*/*").OutputLevel = INFO
        self.assertEqual(self.acc.getEventAlgo("NestedAlgo1").OutputLevel, INFO, "wrong OutputLevel value for NestedAlgo1")
        self.assertEqual(self.acc.getEventAlgo("NestedAlgo2").OutputLevel, INFO, "wrong OutputLevel value for NestedAlgo1")



class TestHLTCF( unittest.TestCase ):
    def runTest( self ):
        # replicate HLT issue, it occured because the sequnces were recorded in the order of storing in the dict and thus the
        # some of them (in this case hltSteps) did not have properties recorded


        acc = ComponentAccumulator()
        acc.addSequence( seqOR("hltTop") )
        algos2 = TestAlgo( "RecoAlgInTop" )
        acc.addEventAlgo( algos2, sequenceName="hltTop" ) # some algo
        acc.addSequence( seqAND("hltSteps"), parentName="hltTop" )
        acc.addSequence( parOR("hltStep_1"), parentName="hltSteps" )
        acc.addSequence( seqAND("L2CaloEgammaSeq"), "hltStep_1" )
        acc.addSequence( parOR("hltStep_2"), parentName="hltSteps" )
        acc.moveSequence( "L2CaloEgammaSeq", "hltStep_2" )

        fout = open("testFile2.pkl", "wb")
        acc.store(fout)
        fout.close()
        #import pickle
        #f = open("testFile2.pkl", 'rb')
        #s = pickle.load(f)
        #f.close()
        #TODO revisit after we settle on the pickle content
        #self.assertNotEqual( s['hltSteps']['Members'], '[]', "Empty set of members in hltSteps, Sequences recording order metters" )


class MultipleParentsInSequences( unittest.TestCase ):
    def runTest( self ):
       # test if an algorithm (or sequence) can be controlled by more than one sequence

        accTop = ComponentAccumulator()

        recoSeq = seqAND("seqReco")
        recoAlg = TestAlgo( "recoAlg" )
        recoSeq.Members.append( recoAlg )

        acc1 = ComponentAccumulator()
        acc1.addSequence( seqAND("seq1") )
        acc1.addSequence( recoSeq, parentName="seq1" )

        acc2 = ComponentAccumulator()
        acc2.addSequence( seqAND("seq2") )
        acc2.addSequence( recoSeq, parentName="seq2" )

        accTop.merge( acc1 )
        accTop.merge( acc2 )

        accTop.printConfig()

        self.assertIsNotNone( findAlgorithm( accTop.getSequence( "seq1" ), "recoAlg" ), "Algorithm missing in the first sequence" )
        self.assertIsNotNone( findAlgorithm( accTop.getSequence( "seq2" ), "recoAlg" ), "Algorithm missing in the second sequence" )
        s = accTop.getSequence( "seqReco" )
        self.assertEqual( len( s.Members ), 1, "Wrong number of algorithms in reco seq: %d " % len( s.Members ) )
        self.assertIs( findAlgorithm( accTop.getSequence( "seq1" ), "recoAlg" ), findAlgorithm( accTop.getSequence( "seq2" ), "recoAlg" ), "Algorithms are cloned" )
        self.assertIs( findAlgorithm( accTop.getSequence( "seq1" ), "recoAlg" ), recoAlg, "Clone of the original inserted in sequence" )

        fout = open("dummy.pkl", "wb")
        accTop.store( fout )
        fout.close()
        #import pickle
        # check if the recording did not harm the sequences
        #with open("dummy.pkl", 'rb') as f:
        #    s = pickle.load( f )
            # TODO revisit once settle on pickle file constent
            #self.assertEqual( s['seq1']["Members"], "['AthSequencer/seqReco']", "After pickling recoSeq missing in seq1 " + s['seq1']["Members"])
            #self.assertEqual( s['seq2']["Members"], "['AthSequencer/seqReco']", "After pickling recoSeq missing in seq2 " + s['seq2']["Members"])
            #self.assertEqual( s['seqReco']["Members"], "['HelloAlg/recoAlg']", "After pickling seqReco is corrupt " + s['seqReco']["Members"] )

class ForbidRecursiveSequences( unittest.TestCase ):
    def runTest( self ):
        #Can't add a sequence with the same name below itself, e.g.
        # \__ AthAlgSeq (seq: PAR AND)
        #    \__ seq1 (seq: SEQ AND)
        #       \__ seq1 (seq: SEQ AND)
        print("")
        def selfSequence():
            from AthenaCommon.CFElements import seqAND
            accTop = ComponentAccumulator()
            accTop.wasMerged()
            seq1 = seqAND("seq1")
            seq1_again = seqAND("seq1")
            accTop.addSequence(seq1)
            accTop.addSequence(seq1_again, parentName = "seq1")

        #Allowed to add a sequence with the same name at same depth, e.g.
        # \__ AthAlgSeq (seq: PAR AND)
        #    \__ seq1 (seq: SEQ AND)
        #       \__ seq2 (seq: SEQ AND)
        #       \__ seq2 (seq: SEQ AND)
        # should not raise any exceptions
        def selfTwinSequence():
            from AthenaCommon.CFElements import seqAND
            accTop = ComponentAccumulator()
            accTop.wasMerged()
            seq1 = seqAND("seq1")
            seq2 = seqAND("seq2")
            seq2_again = seqAND("seq1")
            accTop.addSequence(seq1)
            accTop.addSequence(seq2, parentName = "seq1")
            accTop.addSequence(seq2_again, parentName = "seq1")
            accTop.wasMerged()


        #Can't add a sequence with the same name two steps below itself, e.g.
        # \__ AthAlgSeq (seq: PAR AND)
        #    \__ seq1 (seq: SEQ AND)
        #       \__ seq2 (seq: SEQ AND)
        #          \__ seq1 (seq: SEQ AND)
        def selfGrandParentSequence():
            from AthenaCommon.CFElements import seqAND
            accTop = ComponentAccumulator()
            accTop.wasMerged()
            seq1 = seqAND("seq1")
            seq2 = seqAND("seq2")
            seq1_again = seqAND("seq1")
            accTop.addSequence(seq1)
            accTop.addSequence(seq2, parentName = "seq1")
            accTop.addSequence(seq1_again, parentName = "seq2")


        #Can't merge sequences with the same name two steps below itself, e.g.
        # \__ AthAlgSeq (seq: PAR AND)
        #    \__ seq1 (seq: SEQ AND)
        #       \__ seq2 (seq: SEQ AND)
        #          \__ seq1 (seq: SEQ AND)
        def selfMergedGrandParentSequence():
            from AthenaCommon.CFElements import seqAND
            acc1=ComponentAccumulator()
            acc1.wasMerged()
            acc1.addSequence(seqAND("seq1"))

            acc2=ComponentAccumulator()
            acc2.wasMerged()
            acc2.addSequence(seqAND("seq2"))
            acc2.addSequence(seqAND("seq1"), parentName = "seq2")
            acc1.merge(acc2, sequenceName="seq1")

        print("selfSequence")
        self.assertRaises(RuntimeError, selfSequence )
        print("selfSequence done")

        print("selfGrandParentSequence")
        self.assertRaises(RuntimeError, selfGrandParentSequence )
        print("selfGrandParentSequence done")

        print("selfMergedGrandParentSequence")
        self.assertRaises(RuntimeError, selfMergedGrandParentSequence )
        print("selfMergedGrandParentSequence done")

class FailedMerging( unittest.TestCase ):
    def runTest( self ):
        topCA = ComponentAccumulator()

        def badMerge():
            someCA = ComponentAccumulator()
            topCA.merge(  (someCA, 1, "hello")  )
        self.assertRaises(TypeError, badMerge )
        topCA.wasMerged()


class MergeMovingAlgorithms( unittest.TestCase ):
    def runTest( self ):
        destinationCA = ComponentAccumulator()
        destinationCA.addSequence( seqAND("dest") )

        sourceCA = ComponentAccumulator()
        sourceCA.addEventAlgo(TestAlgo("alg1"))
        sourceCA.addEventAlgo(TestAlgo("alg2"))
        sourceCA.addSequence( seqAND("innerSeq") )
        sourceCA.addEventAlgo(TestAlgo("alg3"), sequenceName="innerSeq" )

        destinationCA.merge( sourceCA, sequenceName="dest"  )

        #destinationCA.merge( sourceCA )
        self.assertIsNotNone( findAlgorithm( destinationCA.getSequence("dest"), "alg1" ), "Algorithm not placed in sub-sequence" )
        self.assertIsNotNone( findSubSequence( destinationCA.getSequence(), "innerSeq" ), "The sequence is not added" )
        self.assertIsNotNone( findAlgorithm( destinationCA.getSequence("dest"), "alg3" ), "Algorithm deep in thesource CA not placed in sub-sequence of destiantion CA" )
        destinationCA.wasMerged()
        sourceCA.wasMerged()


class TestComponentAccumulatorAccessors( unittest.TestCase ):
    def runTest( self ):
        ca = ComponentAccumulator()
        ca.addEventAlgo(TestAlgo("alg1"))

        self.assertEqual( len(ca.getEventAlgos()), 1 , "Found single alg")
# no idea why this assersts do not recognise exceptions
#        self.assertRaises(ConfigurationError, ca.getEventAlgo("alg2"))

        ca.addEventAlgo(TestAlgo("alg2"))

        self.assertIsNotNone( ca.getEventAlgo("alg2"), "Found single alg")
        self.assertEqual( len(ca.getEventAlgos()), 2 , "Found single alg")
 #       self.assertRaises(ConfigurationError, ca.getEventAlgo(), "Single Alg API ambiguity")



        ca.addPublicTool( dummyTool(name="tool1") )
        self.assertIsNotNone( ca.getPublicTool(), "Found single tool")
        ca.addPublicTool( dummyTool(name="tool2") )
#        self.assertRaises(ConfigurationError, ca.getPublicTool(), "Found single tool")

class TestDeduplication( unittest.TestCase ):
    def runTest( self ):
        IOVDbSvc=CompFactory.IOVDbSvc #Test de-duplicating folder-list
        result1=ComponentAccumulator()
        result1.addService(IOVDbSvc(Folders=["/foo"]))
        result1.wasMerged()
        
        result2=ComponentAccumulator()
        result2.addService(IOVDbSvc(Folders=["/bar"]))
        result2.merge(result1)

        self.assertIn("/bar",result2.getService("IOVDbSvc").Folders)
        self.assertIn("/foo",result2.getService("IOVDbSvc").Folders)

        #The merge should be also updated the result1
        self.assertIn("/bar",result1.getService("IOVDbSvc").Folders)
        self.assertIn("/foo",result1.getService("IOVDbSvc").Folders)
        
        svc3=IOVDbSvc(Folders=["/barrr"])
        result2.addService(svc3)
        self.assertIn("/foo",svc3.Folders)
        self.assertIn("/barrr",result2.getService("IOVDbSvc").Folders)

        #The IOVDbSvc in the componentAccumulator is the same instance than the one we have here
        #Modifying svc3 touches also the ComponentAccumulator
        svc3.Folders+=["/fooo"]
        self.assertIn("/fooo",result2.getService("IOVDbSvc").Folders)

        result2.wasMerged()
        
        #Trickier case: Recursive de-duplication of properties of tools in a Tool-handle array
        result3=ComponentAccumulator()
        result3.addService(dummyService("dummyService",
                                        AString="bla",
                                        AList=["l1","l2"],
                                        SomeTools=[dummyTool("tool1",BList=["lt1","lt2"]),],))

        #Exactly the same again:
        result3.addService(dummyService("dummyService",
                                        AString="bla",
                                        AList=["l1","l2"],
                                        SomeTools=[dummyTool("tool1",BList=["lt1","lt2"]),],))

        self.assertEqual(len(result3._services),1) #Verify that we have only one service


        #Add a service with a different name
        result3.addService(dummyService("NewService", AString="blabla",
                                        AList=["new1","new2"],
                                        OneTool=dummyTool("tool2"),
                                        SomeTools=[dummyTool("tool1"),],
                                    )
                       )
      
        self.assertEqual(len(result3._services),2)
      
      
        #Add the same service again, with a sightly differently configured private tool:
        result3.addService(dummyService(AString="bla",
                                        AList=["l1","l3"],
                                        SomeTools=[dummyTool("tool1",BList=["lt3","lt4"]),],
                                    )
                       )
        # TODO revistit how this can be tested in "python" service implementations
        self.assertEqual(len(result3.getService("dummyService").SomeTools),1)
        #self.assertEqual(set(result3.getService("dummyService").SomeTools[0].BList),set(["lt1","lt2","lt3","lt4"])) 
        #self.assertEqual(set(result3.getService("dummyService").AList),set(["l1","l2","l3"]))
        
        #with  self.assertRaises(DeduplicationFailed):
        #    result3.addService(dummyService("dummyService", AString="blaOther"))

        [ ca.wasMerged() for ca in [result1, result2, result3]]


class TestMergeComponentsFromDifferentBranches( unittest.TestCase ):

    def setUp(self):
        pass

    # TODO - prepare better example
    def skip_test_algorithms_merging(self):

        class MergeableAlgorithm( TestAlgo ):
            def __init__( self, name, **kwargs ):
                super( TestAlgo, self ).__init__( name )
                #self._jobOptName = name
                for n, v in kwargs.items():
                    setattr(self, n, v)

                self._set_attributes = kwargs.keys()

            def getValuedProperties(self):
                d = {}
                for attrib in self._set_attributes:
                    d[attrib] = getattr(self, attrib)
                return d

            def __eq__(self,rhs):
                if self is rhs:
                    return True
                if not rhs or not isinstance(rhs,Configurable) or self.getFullName() != rhs.getFullName():
                    return False
                for attr, value in self.getValuedProperties().items():
                    if getattr(rhs, attr) != value:
                        return False
                return True

            def __ne__(self,rhs):
                return not self.__eq__(rhs)

        ca = ComponentAccumulator()

        seq1 = seqAND("seq1")
        innerSeq1 = seqAND("innerSeq1")
        level2Seq1 = seqAND("level2Seq1")

        seq2 = seqAND("seq2")
        innerSeq2 = seqAND("innerSeq2")

        firstAlg = MergeableAlgorithm("alg1", InputMakerInputDecisions=["input1"])

        ca.addSequence(seq1)
        ca.addSequence(innerSeq1, parentName=seq1.name())
        ca.addSequence(level2Seq1, parentName=innerSeq1.name())
        ca.addEventAlgo(firstAlg, sequenceName=level2Seq1.name())

        ca.addSequence(seq2)
        ca.addSequence(innerSeq2, parentName=seq2.name())

        innerSeqCopy = seqAND("innerSeq2")
        level2SeqCopy = seqAND("level2Seq2")

        secondAlg = MergeableAlgorithm("alg1", InputMakerInputDecisions=["input2"])

        secondCa = ComponentAccumulator()
        secondCa.addSequence(innerSeqCopy)
        secondCa.addSequence(level2SeqCopy, parentName=innerSeqCopy.name())
        secondCa.addEventAlgo(secondAlg, sequenceName=level2SeqCopy.name())

        ca.merge(secondCa)

        foundAlgs = findAllAlgorithms(ca.getSequence(), 'alg1')

        self.assertEqual(len(foundAlgs), 2)

        self.assertEqual(set(foundAlgs[0].InputMakerInputDecisions), {"input1", "input2"})
        self.assertEqual(set(foundAlgs[1].InputMakerInputDecisions), {"input1", "input2"})

        ca.printConfig()
        ca.wasMerged()


class TestSequencesMerging( unittest.TestCase ):
    def setUp(self):
        pass

    def skip_test_sequences_merging(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.lock()
        from AthenaCommon.Logging import logging
        logging.getLogger('ComponentAccumulator').setLevel(DEBUG)
        

        print("ca1")
        ca1 = ComponentAccumulator()
        ca1.addEventAlgo(TestAlgo("alg1"))
        ca1.printConfig()
        ca1.addSequence(seqAND("someSequence"))

        print("ca2")
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        ca2 = OutputStreamCfg(ConfigFlags, "RDO", ItemList = [
            "SCT_RDO_Container#SCT_RDOs",
            "InDetSimDataCollection#SCT_SDO_Map"])
        ca2.printConfig()

        print("after merge")
        ca1.merge(ca2)
        ca1.printConfig()
        
        self.assertEqual( len(ca1._allSequences), 2, "Dangling sequences not maintained" )
                
        print("Instantiating top CA")
        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        topca = MainServicesCfg( ConfigFlags )
        topca.printConfig()

        print("Merging to the top level CA")        
        topca.merge( ca1 )
        topca.printConfig()
        topca.wasMerged()
        
if __name__ == "__main__":
    unittest.main()
