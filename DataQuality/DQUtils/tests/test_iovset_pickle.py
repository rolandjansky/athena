# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs, process_iovs
from cPickle import loads, dumps

def test_serializer():
    """
    Try serializing a fairly complicated iovset object
    """
    iovs = fetch_iovs("SHIFTOFL", runs=152166)
    
    channels, iovsets = iovs.chans_iovsets
    
    processed = [(since, until, states)
                 for since, until, states in process_iovs(*iovsets)]
    
    serialized = dumps(processed).encode("bz2").encode("base64")
    deserialized = loads(serialized.decode("base64").decode("bz2"))
    
    assert deserialized == processed
    
