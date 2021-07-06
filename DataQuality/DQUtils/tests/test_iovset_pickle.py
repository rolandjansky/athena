#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs, process_iovs
from pickle import loads, dumps

def test_serializer():
    """
    Try serializing a fairly complicated iovset object
    """
    iovs = fetch_iovs("DEFECTS", runs=359918)
    
    channels, iovsets = iovs.chans_iovsets
    
    processed = [(since, until, states)
                 for since, until, states in process_iovs(*iovsets)]
    
    serialized = dumps(processed)
    deserialized = loads(serialized)
    
    assert deserialized == processed
    
if __name__ == '__main__':
    test_serializer()