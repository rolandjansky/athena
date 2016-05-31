# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Excerciser functions for JetDef.py"""

import getopt, sys, os
from StdOutController import StdOutController

from generateJetChainDefs import  generateChainDefs


def _generate(d):
    controller = StdOutController()

    controller.off()
    cd  = generateChainDefs(d)
    sys.settrace(None)
    controller.on()
    return cd


def run_triggerMenuXML_dicts(use_atlas_config=True,
                             debug=True,
                             printChains=True):
    from triggerMenuXML_dicts import triggerMenuXML_dicts as dicts
    m =  'have [%d] dicts, set $JETDEF_DEBUG, $JETDEF_DEBUG_NO_INSTANTIATION'\
         ' to write to disk' % (
        len(dicts))
    print m

    devnull = open(os.devnull, 'w')
    old_out = sys.stdout
    old_err = sys.stderr
    
    result = []
    ndicts = 0
    for d in dicts:
        if d['chainName'] == 'j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO':
            print 'Excluding j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO'
            continue
        
        if printChains: print ndicts, d['chainName']
        # sys.stdout = devnull
        # sys.stderr = devnull
        try:
            result.append(_generate(d))
        except Exception, e:
            assert False
        ndicts += 1
        # sys.stdout = old_out
        # sys.stderr = old_err

    print 'have [%d] chainDefs' % len(result)
    return result

def run_test_dicts():
    """Function to run jetdef from a text file containing a single dict"""
    from test_dicts import triggerMenuXML_dicts as dicts
    print 'run_test_dicts: found %d dicts for chains:' % len(dicts)
    for d in dicts:
        print d['chainName']


    chainDefs = []
    for d in dicts:
        print d['chainName']
        try:
            cd = _generate(d)
        except Exception, e:
            print '(Error)ChainDef creation failed: ', str(e)
        print cd
        chainDefs.append(cd)

    return chainDefs


def run_from_topo():

    assert ('JETDEF_DEBUG2' in os.environ)

    def has_topo(d):
        for c in d['chainParts']:
            if c['topo']: return True
        return False

    from triggerMenuXML_dicts import triggerMenuXML_dicts as dicts
    dicts = [d for d in dicts if has_topo(d)]
    print 'have [%d] chaindefs, use env variables to write to disk' % (
        len(dicts))
    return [_generate(d) for d in dicts]
        
def usage():
    print 'test_functions: Create ChainDefs instances from dictionaries'
    print '   -m run all the dicts gathered from building TriggerMenuXML'
    print '   -f run from test_dicts.'
    print '   -t run topo chains from . TriggerMEnuXML dicts'

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
            run_triggerMenuXML_dicts(printChains=True)
        elif o == '-f':
            run_test_dicts()
        elif o == '-t':
            run_from_topo()
        else:
            usage()
                
            

