#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs
from DQUtils.tests import invariant

@invariant("DetStatusSHIFTOFL-pass1-analysis-2010B")
def test_read():
    """
    Check if retrieving the Code and Comment field from DetStatusSHIFTOFL-pass1-analysis-2010B changed
    
    Reads some data from the database which is expected not to change.
    A problem with this could either indicate a broken API, or a changed database
    """
    tag = "DetStatusSHIFTOFL-pass1-analysis-2010B"
    result = fetch_iovs("SHIFTOFL", what=["Code", "Comment"], tag=tag)
    return result
        
def test_fetch():
    
    fetch_iovs("DCSOFL", runs=152166)
    fetch_iovs("COOLOFL_DCS::/LHC/DCS/FILLSTATE", runs=152166)
    fetch_iovs("COOLOFL_DCS/COMP200::/LHC/DCS/FILLSTATE", runs=152166)

