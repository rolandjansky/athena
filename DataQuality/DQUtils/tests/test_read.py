#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs
from DQUtils.tests import invariant

@invariant("359918-DetStatusDEFECTS-pro22-04")
def test_read():
    """
    Check if retrieving the Code and Comment field from DetStatusSHIFTOFL-pass1-analysis-2010B changed
    
    Reads some data from the database which is expected not to change.
    A problem with this could either indicate a broken API, or a changed database
    """
    tag = "DetStatusDEFECTS-pro22-04"
    result = fetch_iovs("DEFECTS", what=["present", "recoverable", "user"], tag=tag, runs=359918)
    return list(result)
        
@invariant("359918-blobs")
def test_fetch():
    fetch_iovs("DEFECTS", runs=359918)
    fetch_iovs("COOLOFL_DCS::/LHC/DCS/FILLSTATE", runs=359918)
    fetch_iovs("COOLOFL_DCS/CONDBR2::/LHC/DCS/FILLSTATE", runs=359918)
    iovs=fetch_iovs("COOLONL_TRIGGER::/TRIGGER/LUMI/OnlPrefLumi", runs=359918)
    assert len(iovs) == 729, 'correct number of iovs retrieved'
    return iovs[:10]

if __name__ == '__main__':
    import sys
    test_read()
    test_fetch()