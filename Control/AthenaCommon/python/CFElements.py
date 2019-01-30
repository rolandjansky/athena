from AthenaCommon.AlgSequence import AthSequencer
import collections

def parOR(name, subs=[]):
    """ parallel OR sequencer """
    seq = AthSequencer( name )
    seq.ModeOR = True
    seq.Sequential = False
    seq.StopOverride = True
    for s in subs:
        seq += s
    return seq
    
def seqAND(name, subs=[]):
    """ sequential AND sequencer """
    seq = AthSequencer( name )
    seq.ModeOR = False
    seq.Sequential = True
#    seq.StopOverride = True
    seq.StopOverride = False
    for s in subs:
        seq += s
    return seq

def getAllSequenceNames(seq, depth=0):
    """ Generate a list of sequence names and depths in the graph, e.g.
    [('AthAlgSeq', 0), ('seq1', 1), ('seq2', 1), ('seq1', 2)]
    represents
    \\__ AthAlgSeq (seq: PAR AND)
        \\__ seq1 (seq: SEQ AND)
           \\__ seq2 (seq: SEQ AND)
    """

    seqNameList = [(seq.name(), depth)]
    for c in seq.getChildren():
      if isSequence(c):
        seqNameList +=  getAllSequenceNames(c, depth+1)

    return seqNameList

def checkSequenceConsistency( seq ):
    """ Enforce rules for sequence graph - identical items can only appear at same depth """
    seqNameList = getAllSequenceNames(seq)
    names = [sNL[0] for sNL in seqNameList]

    for item, count in collections.Counter(names).items():
        if count > 1:
            depths = set()
            for (name, depth) in seqNameList:
                if name == item:
                  depths.add(depth)
            if len(depths) > 1:
                if names[0] == name:
                    raise RuntimeError("Sequence %s contains sub-sequence %s" %(name, name) )
                else:
                    raise RuntimeError("Sequence %s contains sub-sequence %s at different depths" %(names[0], item,) )

def seqOR(name, subs=[]):
    """ sequential OR sequencer, used when a barrier needs to be set but all subs reached irrespective of the decision """
    seq = AthSequencer( name )
    seq.ModeOR = True
    seq.Sequential = True
    seq.StopOverride = True
    for s in subs:
        seq += s
    return seq

def stepSeq(name, filterAlg, rest):
    """ elementary HLT step sequencer, filterAlg is gating, rest is anything that needs to happen within the step """
    stepReco = parOR(name+"_reco", rest)
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd

def isSequence( obj ):
    return 'AthSequence' in type( obj ).__name__
    
def findSubSequence( start, nameToLookFor ):
    """ Traverse sequences tree to find a sequence of a given name. The first one is returned. """
    if start.name() == nameToLookFor:
        return start
    for c in start.getChildren():
        #print type(c), c.name()
        if isSequence( c ):
            if  c.name() == nameToLookFor:
                return c
            found = findSubSequence( c, nameToLookFor )
            if found:
                return found
    return None

def findOwningSequence( start, nameToLookFor ):
    """ find sequence that owns the sequence nameTooLookFor"""
    for c in start.getChildren():
        if  c.name() == nameToLookFor:
            return start
        if isSequence( c ):
            found = findOwningSequence( c, nameToLookFor )
            if found:
                return found
    return None

def findAlgorithm( startSequence, nameToLookFor, depth = 1000000 ):
    """ Traverse sequences tree to find the algorithm of given name. The first encountered is returned. 

    The name() method is used to obtain the algorithm name, that one has to match to the request.
    Depth of the search can be controlled by the depth parameter. 
    Typical use is to limit search to the startSequence with depth parameter set to 1
    """
    for c in startSequence.getChildren():
        if not isSequence( c ):
            if  c.name() == nameToLookFor:
                return c
        else:
            if depth > 1:
                found = findAlgorithm( c, nameToLookFor, depth-1 )
                if found:
                    return found                

    return None

def flatAlgorithmSequences( start ):
    """ Converts tree like structure of sequences into dictionary 
    keyed by top/start sequence name containing lists of of algorithms & sequences."""

    def __inner( seq, collector ):
        for c in seq.getChildren():        
            collector[seq.name()].append( c )
            if isSequence( c ):                        
                __inner( c, collector )
                
    from collections import defaultdict
    c = defaultdict(list)
    __inner(start, c)
    return c

def flatSequencers( start ):
    """ Flattens sequences """
    
    def __inner( seq, collector ):
        if seq.name() not in collector:
            collector[seq.name()] = []
        for c in seq.getChildren():
            collector[seq.name()].append( c )
            if isSequence( c ):            
                if c.name() in collector: # already visited
                    pass
                else:       
                    __inner( c, collector )

    from collections import defaultdict
    c = defaultdict(list)
    __inner(start, c)
    return c

# self test
import unittest
class TestCF( unittest.TestCase ):
    def setUp( self ):
        from AthenaCommon.Configurable import ConfigurablePyAlgorithm
        from AthenaCommon.Configurable import Configurable
        Configurable.configurableRun3Behavior=1

        top = parOR("top")
        top += parOR("nest1")
        nest2 = seqAND("nest2")
        top += nest2
        top += ConfigurablePyAlgorithm("SomeAlg0")
        nest2 += parOR("deep_nest1")
        nest2 += parOR("deep_nest2")
        
        nest2 += ConfigurablePyAlgorithm("SomeAlg1")
        nest2 += ConfigurablePyAlgorithm("SomeAlg2")
        nest2 += ConfigurablePyAlgorithm("SomeAlg3")
        self.top = top

    def test_findTop( self ):
        f = findSubSequence( self.top, "top")
        self.assertIsNotNone( f, "Can not find sequence at start" )
        self.assertEqual( f.name(), "top", "Wrong sequence" )
        # a one level deep search
        nest2 = findSubSequence( self.top, "nest2" )
        self.assertIsNotNone( nest2, "Can not find sub sequence" )
        self.assertEqual( nest2.name(), "nest2", "Sub sequence incorrect" )

    def test_findDeep( self ):
        # deeper search
        d = findSubSequence( self.top, "deep_nest2")
        self.assertIsNotNone( d, "Deep searching for sub seqeunce fails" )
        self.assertEqual( d.name(), "deep_nest2", "Wrong sub sequence in deep search" )

    def test_findMissing( self ):
        # algorithm is not a sequence
        d = findSubSequence( self.top, "SomeAlg1") 
        self.assertIsNone( d, "Algorithm confused as a sequence" )
    
        # no on demand creation
        inexistent = findSubSequence( self.top, "not_there" )
        self.assertIsNone( inexistent, "ERROR, found sub sequence that does not relay exist" )
    
        # owner finding
        inexistent = findOwningSequence(self.top, "not_there")
        self.assertIsNone( inexistent, "ERROR, found owner of inexistent sequence " )

    def test_findRespectingScope( self ):
        owner = findOwningSequence( self.top, "deep_nest1")
        self.assertEqual( owner.name(), "nest2", "Wrong owner %s" % owner.name() )

        owner = findOwningSequence( self.top, "deep_nest2")
        self.assertEqual( owner.name(), "nest2", "Wrong owner %s" % owner.name() )

        owner = findOwningSequence( self.top, "SomeAlg1")
        self.assertEqual( owner.name(), "nest2", "Wrong owner %s" % owner.name() )

        owner = findOwningSequence( self.top, "SomeAlg0")
        self.assertEqual( owner.name() , "top", "Wrong owner %s" % owner.name() )

    def test_flatCollectors( self ):
        flat = flatAlgorithmSequences( self.top )
        #print "here", flat.keys()
        expected = [ "top", "nest2" ]
        self.assertEqual( set( flat.keys() ), set( expected ), "To many or to few sequences in flat structure, present: %s expected: %s "% ( " ".join( flat.keys() ), " ".join( expected ) ) )

        expected = [ "top", "nest1", "nest2", "deep_nest1", "deep_nest2" ]
        flat = flatSequencers( self.top )
        self.assertEqual( set( flat.keys() ), set( expected ), "To many or to few sequences in flat structure, present: %s expected: %s "% ( " ".join( flat.keys() ), " ".join( expected ) ) )

    def test_findAlgorithms( self ):
        a1 = findAlgorithm( self.top, "SomeAlg0" )
        self.assertIsNotNone( a1, "Can't find algorithm present in sequence" )

        a1 = findAlgorithm( self.top, "SomeAlg1" )
        self.assertIsNotNone( a1, "Can't find nested algorithm " )

        nest2 = findSubSequence( self.top, "nest2" )

        a1 = findAlgorithm( nest2, "SomeAlg0" )
        self.assertIsNone( a1, "Finding algorithm that is in the upper sequence" )
    
        a1 = findAlgorithm( nest2, "NonexistentAlg" )
        self.assertIsNone( a1, "Finding algorithm that is does not exist" )
             
        a1 = findAlgorithm( self.top, "SomeAlg0", 1)
        self.assertIsNotNone( a1, "Could not find algorithm within the required nesting depth == 1" )

        a1 = findAlgorithm( self.top, "SomeAlg1", 1)
        self.assertIsNone( a1, "Could find algorithm even if it is deep in sequences structure" )

        a1 = findAlgorithm( self.top, "SomeAlg1", 2)
        self.assertIsNotNone( a1, "Could not find algorithm within the required nesting depth == 2" )

        a1 = findAlgorithm( self.top, "SomeAlg3", 2)
        self.assertIsNotNone( a1 is None, "Could find algorithm even if it is deep in sequences structure" )
