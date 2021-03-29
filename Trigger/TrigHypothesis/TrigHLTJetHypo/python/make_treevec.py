# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def make_treevec(repcondargs):
    """find the tree vector from a list of RepeatedCondition configuration 
    AlgTools using the indices assigned to each AlgTool"""

    treevec = [None for i in range(len(repcondargs)+1)]
    for rc in repcondargs:
        treevec[rc.tree_id] =  rc.tree_pid
        
    assert treevec[0] is None

    treevec[0] = 0  # root
    assert None not in treevec

    return treevec
