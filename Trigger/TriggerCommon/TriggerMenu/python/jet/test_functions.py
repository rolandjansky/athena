# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Excerciser functions for JetDef.py"""

import getopt, sys, os


from generateJetChainDefs import  generateChainDefs


def run_mc_pp_v5(use_atlas_config=True, debug=True):
    from MC_pp_V5_dicts import MC_pp_V5_dicts
    dicts = MC_pp_V5_dicts
    print 'have [%d] chaindefs, use env variables to write to disk' % (
        len(dicts))
    return [generateChainDefs(d) for d in dicts]


def run_test_dicts():
    """Function to run jetdef from a text file containing a single dict"""
    from test_dicts import test_dicts
    chainDefs = [generateChainDefs(d) for d in test_dicts] 
    for c in chainDefs:
        print c
    return chainDefs

def run_from_topo():

    assert ('JETDEF_DEBUG2' in os.environ)
    from MC_pp_V5_dicts import MC_pp_V5_dicts


    def has_topo(d):
        for c in d['chainParts']:
            if c['topo']: return True
        return False

    dicts = [d for d in MC_pp_V5_dicts if has_topo(d)]
    print 'have [%d] chaindefs, use env variables to write to disk' % (
        len(dicts))
    return [generateChainDefs(d) for d in MC_pp_V5_dicts]
        
def usage():
    print 'test_functions: Create ChainDefs instances from dictionaries'
    print '   -m run all the MC_pp_V5 dicts'
    print '   -f run from test_dicts.'
    print '   -t run topo chains from . MC_pp_V5 dicts'

if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'htmf', [])
    except getopt.GetoptError as err:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    if len(opts) != 1:
        print 'Wrong number of arguments ', str(opts)
        usage()
        sys.exit(1)
    
    for o, a in opts:
        if o == '-m':
            run_mc_pp_v5()
        elif o == '-f':
            run_test_dicts()
        elif o == '-t':
            run_from_topo()
        else:
            usage()
                
            

