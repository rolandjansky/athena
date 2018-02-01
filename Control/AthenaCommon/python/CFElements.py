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

    
def findSubSeqeunce( start, nameToLookFor ):
    for c in start.getChildren():
        #print type(c), c.name()
        if 'AthSequencer' in type(c).__name__:
            if  c.name() == nameToLookFor:
                return c
            found = findSubSeqeunce( c, nameToLookFor )
            if found:
                return found
    return None

def flatAlgorithmSequences( start, collector={} ):
    collector[start.name()] = []
    for c in start.getChildren():
        if 'AthSequencer' not in type(c).__name__:            
            collector[start.name()].append( c )
        else:
            flatAlgorithmSequences( c, collector )
    return collector
    pass



if __name__ == "__main__":
    from AthenaCommon.Configurable import ConfigurablePyAlgorithm
    top = parOR("top")
    top += parOR("nest1")
    top += seqAND("nest2")
    top += ConfigurablePyAlgorithm("SomeAlg0")
    # a one level deep search
    nest2 = findSubSeqeunce( top, "nest2" )
    assert nest2, "Can not find sub sequence" 
    assert nest2.name() == "nest2", "Sub sequence incorrect"

    nest2 += parOR("deep_nest1")
    nest2 += parOR("deep_nest2")
    
    nest2 += ConfigurablePyAlgorithm("SomeAlg1")
    nest2 += ConfigurablePyAlgorithm("SomeAlg2")

    # deeper search
    d = findSubSeqeunce(top, "deep_nest2")
    assert d, "Deep searching for sub seqeunce fails"
    assert d.name() == "deep_nest2", "Wrong sub sequence in deep search"

    d += ConfigurablePyAlgorithm("SomeAlg3")

    # algorithm is not a sequence
    d = findSubSeqeunce(top, "SomeAlg1") 
    assert d == None, "Algorithm confused as a sequence"
    
    # no on demand creation
    inexistent = findSubSeqeunce(top, "not_there")
    assert inexistent == None, "ERROR, found sub sequence that does not relay exist %s" % inexistent.name()

    flat = flatAlgorithmSequences( top )
    expected = [ "top", "nest1", "nest2", "deep_nest1", "deep_nest2" ]
    assert set( flat.keys() ) == set( expected ), "To many or to few sequences in flat structure, expected %s present %s "% ( " ".join( flat.keys() ), " ".join( expected ) )
    assert len(flat["top"]) == 1, "To many, to few algorithms under the top seqeunce"
    assert flat["top"][0].getName() == "SomeAlg0", "Wrong algorithm under top sequence %s" % flat["top"][0].getName()
    assert flat["nest2"][0].getName() == "SomeAlg1"
    assert flat["nest2"][1].getName() == "SomeAlg2"
    assert flat["deep_nest2"][0].getName() == "SomeAlg3"

    print ("All OK")
