#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs, process_iovs
from DQUtils.tests import invariant

@invariant("DetStatusSHIFTOFL-pass1-analysis-2010B")
def test_processing():
    """
    Test that processing works
    """
    iovs = fetch_iovs("SHIFTOFL", tag="DetStatusSHIFTOFL-pass1-analysis-2010B")    
    channels, iovsets = iovs.chans_iovsets
    processed = [(since, until, states[:])
                 for since, until, states in process_iovs(*iovsets)]
                 
    return processed
