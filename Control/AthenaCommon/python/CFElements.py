from AthenaCommon.AlgSequence import AthSequencer


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
    seq.StopOverride = False
    for s in subs:
        seq += s
    return seq

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
    

def flatAlgorithmSequences( start, collector={} ):
    """ Converts tree like structure of sequences into dictionary keyed by sequence name containing lists of of algorithms."""
    collector[start.name()] = []
    for c in start.getChildren():        
        if not isSequence( c ):            
            collector[start.name()].append( c )
        else:
            flatAlgorithmSequences( c, collector )
    return collector



def flatSequencers( start, collector={} ):
    collector[start.name()] = []
    for c in start.getChildren():        
        collector[start.name()].append( c )
        if isSequence( c ):            
            flatAlgorithmSequences( c, collector )
    return collector

    

# self test
if __name__ == "__main__":
    from AthenaCommon.Configurable import ConfigurablePyAlgorithm
    top = parOR("top")
    top += parOR("nest1")
    top += seqAND("nest2")
    top += ConfigurablePyAlgorithm("SomeAlg0")

    f = findSubSequence(top, "top")
    assert f, "Can not find sequence at start"
    assert f.name() == "top", "Wrong sequence"
    # a one level deep search
    nest2 = findSubSequence( top, "nest2" )
    assert nest2, "Can not find sub sequence" 
    assert nest2.name() == "nest2", "Sub sequence incorrect"

    nest2 += parOR("deep_nest1")
    nest2 += parOR("deep_nest2")
    
    nest2 += ConfigurablePyAlgorithm("SomeAlg1")
    nest2 += ConfigurablePyAlgorithm("SomeAlg2")

    # deeper search
    d = findSubSequence(top, "deep_nest2")
    assert d, "Deep searching for sub seqeunce fails"
    assert d.name() == "deep_nest2", "Wrong sub sequence in deep search"

    d += ConfigurablePyAlgorithm("SomeAlg3")

    # algorithm is not a sequence
    d = findSubSequence(top, "SomeAlg1") 
    assert d == None, "Algorithm confused as a sequence"
    
    # no on demand creation
    inexistent = findSubSequence(top, "not_there")
    assert inexistent == None, "ERROR, found sub sequence that does not relay exist %s" % inexistent.name()

    flat = flatAlgorithmSequences( top )
    expected = [ "top", "nest1", "nest2", "deep_nest1", "deep_nest2" ]
    assert set( flat.keys() ) == set( expected ), "To many or to few sequences in flat structure, expected %s present %s "% ( " ".join( flat.keys() ), " ".join( expected ) )
    assert len(flat["top"]) == 1, "To many, to few algorithms under the top sequence"
    assert flat["top"][0].getName() == "SomeAlg0", "Wrong algorithm under top sequence %s" % flat["top"][0].getName()
    assert flat["nest2"][0].getName() == "SomeAlg1"
    assert flat["nest2"][1].getName() == "SomeAlg2"
    assert flat["deep_nest2"][0].getName() == "SomeAlg3"



    flat = flatSequencers(top)
    assert set( flat.keys() ) == set( expected ), "To many or to few sequences in flat structure, expected %s present %s "% ( " ".join( flat.keys() ), " ".join( expected ) )
    assert len(flat["top"]) == 3, "To many, to few algorithms under the top sequence"

    a1 = findAlgorithm( top, "SomeAlg0" )
    assert a1, "Can't find algorithm present in sequence"
    a1 = findAlgorithm( top, "SomeAlg1" )
    assert a1, "Can't find nested algorithm "

    a1 = findAlgorithm( nest2, "SomeAlg0" )
    assert a1 == None, "Finding algorithm that is in the upper sequence"
    
    a1 = findAlgorithm( nest2, "NonexistentAlg" )
    assert a1 == None, "Finding algorithm that is does not exist"
             
    a1 = findAlgorithm( top, "SomeAlg0", 1)
    assert a1, "Could not find algorithm within the required nesting depth == 1"

    a1 = findAlgorithm( top, "SomeAlg1", 1)
    assert a1 == None, "Could find algorithm even if it is deep in sequences structure"

    a1 = findAlgorithm( top, "SomeAlg1", 2)
    assert a1, "Could not find algorithm within the required nesting depth == 2"

    a1 = findAlgorithm( top, "SomeAlg3", 2)
    assert a1 == None, "Could find algorithm evn if it is deep in sequences structure"    


    print ("All OK")
