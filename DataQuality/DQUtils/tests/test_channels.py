# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs

def test_fetch_single_detstatus_channel():
    
    result = fetch_iovs("SHIFTOFL", channels=["SCTEA"])
    print "Result length:", len(result)
    
def test_fetch_single_non_detstatus_channel():
    
    result = fetch_iovs("VIRTUALFLAGS", channels=["CP_TRACKING"])
    print "Result length:", len(result)
    

