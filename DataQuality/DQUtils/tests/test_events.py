#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs, process_iovs
from DQUtils.tests import invariant

@invariant("359918-DetStatusDEFECTS-pro22-04")
def test_processing():
    """
    Test that processing works
    """
    iovs = fetch_iovs("DEFECTS", tag="DetStatusDEFECTS-pro22-04", runs=359918)    
    channels, iovsets = iovs.chans_iovsets
    processed = [(since, until, states[:])
                 for since, until, states in process_iovs(*iovsets)]
                 
    return processed

if __name__ == '__main__':
    test_processing()