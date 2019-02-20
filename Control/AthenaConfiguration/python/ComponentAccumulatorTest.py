# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# self test of ComponentAccumulator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, foreach_component
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.CFElements import findSubSequence,findAlgorithm
from AthenaCommon.Configurable import Configurable, ConfigurablePyAlgorithm # guinea pig algorithms
from AthenaCommon.CFElements import seqAND, seqOR, parOR
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG, INFO

import unittest

class TestComponentAccumulator( unittest.TestCase ):
    def setUp(self):


        # trivial case without any nested sequences

        Configurable.configurableRun3Behavior+=1

        log.setLevel(DEBUG)

        dummyCfgFlags=AthConfigFlags()
        dummyCfgFlags.lock()

        class Algo(ConfigurablePyAlgorithm):
            def __init__(self, name):
                super( ConfigurablePyAlgorithm, self ).__init__( name )

        def AlgsConf1(flags):
            acc = ComponentAccumulator()
            a1=Algo("Algo1")
            a2=Algo("Algo2")
            return acc,[a1,a2]


        def AlgsConf2(flags):
            acc = ComponentAccumulator()
            result,algs=AlgsConf1( flags )
            acc.merge(result)
            a = Algo("Algo3")
            print "algo3 when created", id(a)
            algs.append(a)
            return acc,algs

        acc = ComponentAccumulator()

        # top level algs
        acc1,algs=AlgsConf2(dummyCfgFlags)
        acc.merge(acc1)
        acc.addEventAlgo(algs)

        def AlgsConf3(flags):
            acc = ComponentAccumulator()
            na1=Algo("NestedAlgo1")
            return acc,na1

        def AlgsConf4(flags):
            acc,na1= AlgsConf3( flags )
            NestedAlgo2 = Algo("NestedAlgo2")
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
        acc.store(open("testFile.pkl", "w"))
        self.acc = acc


    def test_algorihmsAreAdded( self ):
        self.assertEqual( findAlgorithm( self.acc.getSequence(), "Algo1", 1).name(), "Algo1", "Algorithm not added to a top sequence" )
        self.assertEqual( findAlgorithm( self.acc.getSequence(), "Algo2", 1).name(),  "Algo2", "Algorithm not added to a top sequence" )
        self.assertEqual( findAlgorithm( self.acc.getSequence(), "Algo3", 1).name(), "Algo3", "Algorithm not added to a top sequence" )

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
        with open( "testFile.pkl" ) as f:
            s = pickle.load( f )
            self.assertIsNotNone( s, "The pickle has no content")

    def test_foreach_component( self ):
        from AthenaCommon.Logging import logging
        logging.getLogger('foreach_component').setLevel(DEBUG)
        algo3 = self.acc.getEventAlgo("Algo3")        
        algo3.OutputLevel = INFO
        foreach_component(self.acc, "*/Algo3").OutputLevel = DEBUG # restet to debug level
        self.assertEqual( algo3.OutputLevel, DEBUG, "wrong OutputLevel value for Algo3")
        foreach_component(self.acc, "*sub2*/*").OutputLevel = INFO 
        self.assertEqual(self.acc.getEventAlgo("NestedAlgo1").OutputLevel, INFO, "wrong OutputLevel value for NestedAlgo1")
        self.assertEqual(self.acc.getEventAlgo("NestedAlgo2").OutputLevel, INFO, "wrong OutputLevel value for NestedAlgo1")

        

class TestHLTCF( unittest.TestCase ):
    def runTest( self ):
        # replicate HLT issue, it occured because the sequnces were recorded in the order of storing in the dict and thus the
        # some of them (in this case hltSteps) did not have properties recorded


        Configurable.configurableRun3Behavior=1
        acc = ComponentAccumulator()
        acc.addSequence( seqOR("hltTop") )
        algos2 = ConfigurablePyAlgorithm( "RecoAlgInTop" )
        acc.addEventAlgo( algos2, sequenceName="hltTop" ) # some algo
        acc.addSequence( seqAND("hltSteps"), parentName="hltTop" )
        acc.addSequence( parOR("hltStep_1"), parentName="hltSteps" )
        acc.addSequence( seqAND("L2CaloEgammaSeq"), "hltStep_1" )
        acc.addSequence( parOR("hltStep_2"), parentName="hltSteps" )
        acc.moveSequence( "L2CaloEgammaSeq", "hltStep_2" )

        acc.store(open("testFile2.pkl", "w"))
        import pickle
        f = open("testFile2.pkl")
        s = pickle.load(f)
        f.close()
        self.assertNotEqual( s['hltSteps']['Members'], '[]', "Empty set of members in hltSteps, Sequences recording order metters" )


class MultipleParentsInSequences( unittest.TestCase ):
    def runTest( self ):
       # test if an algorithm (or sequence) can be controlled by more than one sequence
        Configurable.configurableRun3Behavior=1

        accTop = ComponentAccumulator()

        recoSeq = seqAND("seqReco")
        recoAlg = ConfigurablePyAlgorithm( "recoAlg" )
        recoSeq += recoAlg

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
        self.assertEqual( len( s.getChildren() ), 1, "Wrong number of algorithms in reco seq: %d " % len( s.getChildren() ) )
        self.assertIs( findAlgorithm( accTop.getSequence( "seq1" ), "recoAlg" ), findAlgorithm( accTop.getSequence( "seq2" ), "recoAlg" ), "Algorithms are cloned" )
        self.assertIs( findAlgorithm( accTop.getSequence( "seq1" ), "recoAlg" ), recoAlg, "Clone of the original inserted in sequence" )

        accTop.store( open("dummy.pkl", "w") )
        import pickle
        # check if the recording did not harm the sequences
        with open("dummy.pkl") as f:
            s = pickle.load( f )
            self.assertEquals( s['seq1']["Members"], "['AthSequencer/seqReco']", "After pickling recoSeq missing in seq1 " + s['seq1']["Members"])
            self.assertEquals( s['seq2']["Members"], "['AthSequencer/seqReco']", "After pickling recoSeq missing in seq2 " + s['seq2']["Members"])
            self.assertEquals( s['seqReco']["Members"], "['ConfigurablePyAlgorithm/recoAlg']", "After pickling seqReco is corrupt " + s['seqReco']["Members"] )

class ForbidRecursiveSequences( unittest.TestCase ):
    def runTest( self ):
        #Can't add a sequence with the same name below itself, e.g.
        # \__ AthAlgSeq (seq: PAR AND)
        #    \__ seq1 (seq: SEQ AND)
        #       \__ seq1 (seq: SEQ AND)
        def selfSequence():
            Configurable.configurableRun3Behavior=1
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
            Configurable.configurableRun3Behavior=1
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
            Configurable.configurableRun3Behavior=1
            from AthenaCommon.CFElements import seqAND
            accTop = ComponentAccumulator()
            seq1 = seqAND("seq1")
            seq2 = seqAND("seq2")
            seq1_again = seqAND("seq1")
            accTop.addSequence(seq1)
            accTop.addSequence(seq2, parentName = "seq1")
            accTop.addSequence(seq1_again, parentName = "seq2")
            accTop.wasMerged()

        #Can't merge sequences with the same name two steps below itself, e.g.
        # \__ AthAlgSeq (seq: PAR AND)
        #    \__ seq1 (seq: SEQ AND)
        #       \__ seq2 (seq: SEQ AND)
        #          \__ seq1 (seq: SEQ AND)
        def selfMergedGrandParentSequence():
            Configurable.configurableRun3Behavior=1
            from AthenaCommon.CFElements import seqAND
            acc1=ComponentAccumulator()
            acc1.wasMerged()
            acc1.addSequence(seqAND("seq1"))
            acc2=ComponentAccumulator()
            acc2.wasMerged()
            acc2.addSequence(seqAND("seq2"))
            acc2.addSequence(seqAND("seq1"), "seq2")
            acc1.merge(acc2)

        self.assertRaises(RuntimeError, selfSequence )
        self.assertRaises(RuntimeError, selfGrandParentSequence )
        self.assertRaises(RuntimeError, selfMergedGrandParentSequence )

class FailedMerging( unittest.TestCase ):
    def runTest( self ):
        topCA = ComponentAccumulator()

        def badMerge():
            someCA = ComponentAccumulator()
            topCA.merge(  (someCA, 1, "hello")  )
        self.assertRaises(RuntimeError, badMerge )
        topCA.wasMerged()


class MergeMovingAlgorithms( unittest.TestCase ):
    def runTest( self ):
        Configurable.configurableRun3Behavior=1
        destinationCA = ComponentAccumulator()
        destinationCA.addSequence( seqAND("dest") )

        sourceCA = ComponentAccumulator()
        sourceCA.addEventAlgo(ConfigurablePyAlgorithm("alg1"))
        sourceCA.addEventAlgo(ConfigurablePyAlgorithm("alg2"))
        sourceCA.addSequence( seqAND("innerSeq") )
        sourceCA.addEventAlgo(ConfigurablePyAlgorithm("alg3"), sequenceName="innerSeq" )

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
        from AthenaCommon.Configurable import ConfigurablePyAlgorithm,ConfigurableAlgTool
        ca.addEventAlgo(ConfigurablePyAlgorithm("alg1"))
        
        self.assertIsNotNone( ca.getEventAlgo(), "Found single alg")
        self.assertEquals( len(ca.getEventAlgos()), 1 , "Found single alg")
# no idea why this assersts do not recognise exceptions        
#        self.assertRaises(ConfigurationError, ca.getEventAlgo("alg2")) 
        
        ca.addEventAlgo(ConfigurablePyAlgorithm("alg2"))

        self.assertIsNotNone( ca.getEventAlgo("alg2"), "Found single alg")
        self.assertEquals( len(ca.getEventAlgos()), 2 , "Found single alg")
 #       self.assertRaises(ConfigurationError, ca.getEventAlgo(), "Single Alg API ambiguity")

        class Tool(ConfigurableAlgTool):
            def __init__(self, *args, **kwargs):
                super(Tool, self).__init__(*args, **kwargs)
            def getDlls(self):
                return None


        ca.addPublicTool( Tool(name="tool1") )
        self.assertIsNotNone( ca.getPublicTool(), "Found single tool")
        ca.addPublicTool( Tool(name="tool2") )
#        self.assertRaises(ConfigurationError, ca.getPublicTool(), "Found single tool")


if __name__ == "__main__":
    unittest.main()
