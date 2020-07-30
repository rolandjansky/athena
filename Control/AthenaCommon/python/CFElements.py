# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Configurable import Configurable
AthSequencer=CompFactory.AthSequencer
from AthenaCommon.AlgSequence import AthSequencer as LegacyAthSequencer
import collections


def _append( seq, sub ):
    if Configurable.configurableRun3Behavior:
        seq.Members.append( sub )
    else:
        seq += sub

def parOR(name, subs=[]):
    """ parallel OR sequencer """
    seq = AthSequencer( name ) if Configurable.configurableRun3Behavior else LegacyAthSequencer( name )
    seq.ModeOR = True
    seq.Sequential = False
    seq.StopOverride = True
    for s in subs:
        _append(seq, s)
    return seq

def seqAND(name, subs=[]):
    """ sequential AND sequencer """
    seq = AthSequencer( name ) if Configurable.configurableRun3Behavior else LegacyAthSequencer( name )
    seq.ModeOR = False
    seq.Sequential = True
#    seq.StopOverride = True
    seq.StopOverride = False
    for s in subs:
        _append( seq, s )
    return seq

def seqOR(name, subs=[]):
    """ sequential OR sequencer, used when a barrier needs to be set by all subs reached irrespective of the decision """
    seq = AthSequencer( name ) if Configurable.configurableRun3Behavior else LegacyAthSequencer( name )
    seq.ModeOR = True
    seq.Sequential = True
    seq.StopOverride = True
    for s in subs:
        _append( seq, s )
    return seq

def hasProp(comp, propname):
    if hasattr(comp, "properties"): # old/legacy component
        return propname in comp.properties()
    else:
        return hasattr(comp, propname)
    
def getProp(comp, propname):
    if hasattr(comp, "getProperties"): # old/legacy component
        return comp.getValuedProperties()[propname] if propname in  comp.getValuedProperties() else comp.getDefaultProperty(propname)
    else:
        return getattr(comp, propname)



def compName(comp):
    return  comp.name() if callable(comp.name) else comp.name

def getSequenceChildren(comp):
    return  comp.getChildren() if hasattr(comp, "getChildren") and callable(comp.getChildren) else comp.Members


def getAllSequenceNames(seq, depth=0):
    """ Generate a list of sequence names and depths in the graph, e.g.
    [('AthAlgSeq', 0), ('seq1', 1), ('seq2', 1), ('seq1', 2)]
    represents
    \\__ AthAlgSeq (seq: PAR AND)
        \\__ seq1 (seq: SEQ AND)
           \\__ seq2 (seq: SEQ AND)
    """

    seqNameList = [(compName(seq), depth)]
    for c in getSequenceChildren(seq):
      if isSequence(c):
        seqNameList +=  getAllSequenceNames(c, depth+1)

    return seqNameList

def checkSequenceConsistency( seq ):
    """ Enforce rules for sequence graph - identical items can not be added to itself (even indirectly) """

    def __noSubSequenceOfName( s, n, seen = set() ):
        seen = seen.copy()
        seen.add (s)
        for c in getSequenceChildren( s ):
            if c in seen:
                raise RuntimeError("Sequence {} contains itself".format(compName(c)) )
            if isSequence( c ):
                if compName(c) == n:
                    raise RuntimeError("Sequence {} contains sub-sequence of the same name".format(n) )
                __noSubSequenceOfName( c, compName(c), seen ) # check each sequence for repetition as well
                __noSubSequenceOfName( c, n, seen )

    __noSubSequenceOfName( seq, compName(seq) )



def stepSeq(name, filterAlg, rest):
    """ elementary HLT step sequencer, filterAlg is gating, rest is anything that needs to happen within the step """
    stepReco = parOR(name+"_reco", rest)
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd

def isSequence( obj ):
    if hasattr(obj, "__cpp_type__"):
        return obj.__cpp_type__ == 'AthSequencer'
    return 'AthSequence' in type( obj ).__name__

def findSubSequence( start, nameToLookFor ):
    """ Traverse sequences tree to find a sequence of a given name. The first one is returned. """
    if compName(start) == nameToLookFor:
        return start
    for c in getSequenceChildren(start):
        if isSequence( c ):
            if  compName(c) == nameToLookFor:
                return c
            found = findSubSequence( c, nameToLookFor )
            if found:
                return found
    return None

def findOwningSequence( start, nameToLookFor ):
    """ find sequence that owns the sequence nameTooLookFor"""
    for c in getSequenceChildren(start):
        if  compName(c) == nameToLookFor:
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
    for c in getSequenceChildren(startSequence):
        if not isSequence( c ):
            if  compName(c) == nameToLookFor:
                return c
        else:
            if depth > 1:
                found = findAlgorithm( c, nameToLookFor, depth-1 )
                if found:
                    return found

    return None

def findAllAlgorithms(sequence, nameToLookFor=None):
    """
    Returns flat listof of all algorithm instances in this, and in sub-sequences
    """
    algorithms = []
    for child in getSequenceChildren(sequence):
        if isSequence(child):
            algorithms += findAllAlgorithms(child, nameToLookFor)
        else:
            if nameToLookFor is None or compName(child) == nameToLookFor:
                algorithms.append(child)
    return algorithms


def findAllAlgorithmsByName(sequence, namesToLookFor=None):
    """
    Finds all algorithms in sequence and groups them by name
    
    Resulting dict has a following structure
    {"Alg1Name":(Alg1Instance, parentSequence, indexInThisSequence),
     "Alg2Name":(Alg2Instance, parentSequence, indexInThisSequence),
     ....}
    """
    algorithms = collections.defaultdict(list)
    for idx, child in enumerate(getSequenceChildren(sequence)):
        if (compName(child) == compName(sequence)):
            raise RuntimeError("Recursively-nested sequence: {} contains itself".format(compName(child)))
        if isSequence(child):
            childAlgs = findAllAlgorithmsByName(child, namesToLookFor)
            for algName in childAlgs:
                algorithms[algName] += childAlgs[algName]
        else:
            if namesToLookFor is None or compName(child) in namesToLookFor:
                algorithms[compName(child)].append( (child, sequence, idx) )
    return algorithms


def flatAlgorithmSequences( start ):
    """ Converts tree like structure of sequences into dictionary
    keyed by top/start sequence name containing lists of of algorithms & sequences."""

    def __inner( seq, collector ):
        for c in getSequenceChildren(seq):
            collector[compName(seq)].append( c )
            if isSequence( c ):
                __inner( c, collector )

    from collections import defaultdict
    c = defaultdict(list)
    __inner(start, c)
    return c

def flatSequencers( start, algsCollection=None ):
    """ Returns dict of sequences keyed by name and containing list of it's members """

    def __inner( seq, collector ):
        if compName(seq) not in collector:
            collector[compName(seq)] = []
        for c in getSequenceChildren(seq):
            isSeq = isSequence(c)
            if not isSeq and algsCollection is not None and compName(c) in algsCollection:
                collector[compName(seq)].append( algsCollection[compName(c)] )
                continue
            collector[compName(seq)].append( c )
            if isSeq and compName(c) not in collector:
                __inner( c, collector )


    from collections import defaultdict
    c = defaultdict(list)
    __inner(start, c)
    return c

# self test
import unittest
class TestCF(object):
    def test_findTop( self ):
        f = findSubSequence( self.top, "top")
        self.assertIsNotNone( f, "Can not find sequence at start" )
        self.assertEqual( compName(f), "top", "Wrong sequence" )
        # a one level deep search
        nest2 = findSubSequence( self.top, "nest2" )
        self.assertIsNotNone( nest2, "Can not find sub sequence" )
        self.assertEqual( compName(nest2), "nest2", "Sub sequence incorrect" )

    def test_findDeep( self ):
        # deeper search
        d = findSubSequence( self.top, "deep_nest2")
        self.assertIsNotNone( d, "Deep searching for sub seqeunce fails" )
        self.assertEqual( compName(d), "deep_nest2", "Wrong sub sequence in deep search" )

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
        self.assertEqual( compName(owner), "nest2", "Wrong owner %s" % compName(owner) )

        owner = findOwningSequence( self.top, "deep_nest2")
        self.assertEqual( compName(owner), "nest2", "Wrong owner %s" % compName(owner) )

        owner = findOwningSequence( self.top, "SomeAlg1")
        self.assertEqual( compName(owner), "nest2", "Wrong owner %s" % compName(owner) )

        owner = findOwningSequence( self.top, "SomeAlg0")
        self.assertEqual( compName(owner) , "top", "Wrong owner %s" % compName(owner) )

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


class TestLegacyCF( unittest.TestCase, TestCF ):
    def setUp( self ):
        from AthenaCommon.Configurable import ConfigurablePyAlgorithm
        Configurable.configurableRun3Behavior=0
        top = parOR("top")

        # Skip initialization if it's already been done... otherwise, we'll
        # get errors about duplicates.
        if not top.getChildren():
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



class TestConf2CF( unittest.TestCase,  TestCF ):
    def setUp( self ):
        Configurable.configurableRun3Behavior=1
        def __mkAlg(name):
            alg = ConfigurablePyAlgorithm(name)
            setattr(alg, "__component_type__", "Algorithm")
            return alg

        from AthenaCommon.Configurable import ConfigurablePyAlgorithm
        top = parOR("top")
        top.Members += [parOR("nest1")]
        nest2 = seqAND("nest2")
        top.Members += [nest2]
        top.Members += [__mkAlg("SomeAlg0")]
        nest2.Members += [parOR("deep_nest1")]
        nest2.Members += [parOR("deep_nest2")]

        nest2.Members += [__mkAlg("SomeAlg1")]
        nest2.Members += [__mkAlg("SomeAlg2")]
        nest2.Members += [__mkAlg("SomeAlg3")]
        self.top = top

class TestNest( unittest.TestCase ):
    def test( self ):
        Configurable.configurableRun3Behavior=1
        top = parOR("top")
        nest1 = parOR("nest1")
        nest2 = seqAND("nest2")
        top.Members += [nest1, nest2]

        deep_nest1 = seqAND("deep_nest1")
        nest1.Members += [deep_nest1]

        nest2.Members += [nest1] # that one is ok
        checkSequenceConsistency( top )
        deep_nest1.Members += [nest1] # introducing an issue
        self.assertRaises( RuntimeError, checkSequenceConsistency, top )

