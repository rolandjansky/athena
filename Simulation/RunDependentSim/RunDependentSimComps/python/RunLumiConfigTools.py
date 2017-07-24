# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def condenseRunLumiInfoFragment(frag,filename="./RunDMCTriggerRunsInfo.py"):
    """ Condense the run lumi configuration into a simple list describing the number of events per run, for trigger reco trf.
    """
    condensed = []
    for xDict in frag:
        r = xDict['run']
        e = xDict['evts']
        if ((not condensed) or (condensed[-1]['run'] != r)): #new entry
            condensed.append({'run':r,'evts':e})
        else:
            condensed[-1]['evts'] += e
            pass
        pass    
    with open( filename, 'w' ) as f:
        f.write( "#Configuration describing nEvents for each run number in digi file.\nlistOfRunsEvents = %s" % condensed )
        pass    
    return condensed
