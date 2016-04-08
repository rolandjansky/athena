# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def cullJobDescriptors(descs, isNightly, limitedNightlyVeto, rttPilotJobs):
    if isNightly:
        descs = [d for d in descs if not d.__dict__['numberedOnly']]
    if limitedNightlyVeto:
        descs = [d for d in descs if not d.__dict__['limitedNightlies']]
    if not rttPilotJobs:
        descs = [d for d in descs if not d.__dict__['rttPilotJob']]
    return descs
    
