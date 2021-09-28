#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 
def efficiency(inputs):
    assert len(inputs) == 1 # one match at a time 
    assert len(inputs[0][1]) == 2 # pair of truth and matched-to-truth hists
    for _ in inputs:
        transform = _[1][1].Clone() # cloning retains binning and titles for each variable
        transform.Divide(_[1][1],_[1][0],1,1,"B") # divide by the total truth hist
    return [transform]