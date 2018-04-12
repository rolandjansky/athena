# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Excerciser functions for JetDef.py"""

import getopt, sys, os
from StdOutController import StdOutController
import importlib

from generateJetChainDefs import  generateChainDefs
from exc2string import exc2string2 

def _generate(d, silent):

    if silent:
        controller = StdOutController()
        controller.off()
        
    cd  = generateChainDefs(d)
    sys.settrace(None)

    if silent: controller.on()

    return cd


def run_triggerMenuXML_dicts(silent,
                             dicts,
                             use_atlas_config=True,
                             debug=True,
                             printChains=True):
    if not dicts:
        from triggerMenuXML_dicts import triggerMenuXML_dicts as dicts
        m =  'have [%d] dicts, set $JETDEF_DEBUG, '\
             '$JETDEF_DEBUG_NO_INSTANTIATION'\
             ' to write to disk' % (len(dicts))
    print m

    #devnull = open(os.devnull, 'w')
    #old_out = sys.stdout
    #old_err = sys.stderr
    
    result = []
    ndicts = 0
    #toSkip = (
    #    'j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY',
    #    'j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO',
    #    )
    for d in dicts:
        # if d['chainName'] in toSkip or '_PS_' in d['chainName']:
        #    print d['chainName']
        #    continue
        
        if printChains: print ndicts, d['chainName']
        # sys.stdout = devnull
        # sys.stderr = devnull
        try:
            result.append(_generate(d, silent))
        except Exception, e:
            print d['chainName']
            print e
            assert False
        ndicts += 1
        # sys.stdout = old_out
        # sys.stderr = old_err

    print 'have [%d] chainDefs' % len(result)
    return result

def run_test_dicts(silent, dicts):
    """Function to run jetdef from a text file containing a single dict"""

    if not dicts:
        from test_dicts import triggerMenuXML_dicts as dicts

    print 'run_test_dicts: found %d dicts for chains:' % len(dicts)
    for d in dicts:
        print d['chainName']


    chainDefs = []
    for d in dicts:
        print d['chainName']
        try:
            cd = _generate(d, silent)
            print cd
            chainDefs.append(cd)
        except Exception, e:
            print '(Error)ChainDef creation failed: ', str(e)
            print exc2string2()

    return chainDefs


def run_from_topo(silent, dicts):

    assert ('JETDEF_DEBUG2' in os.environ)

    def has_topo(d):
        for c in d['chainParts']:
            if c['topo']: return True
        return False

    if not dicts:
        from triggerMenuXML_dicts import triggerMenuXML_dicts as dicts

    dicts = [d for d in dicts if has_topo(d)]
    print 'have [%d] chaindefs, use env variables to write to disk' % (
        len(dicts))
    return [_generate(d, silent) for d in dicts]
        
def usage():
    print 'test_functions: Create ChainDefs instances from dictionaries'
    print '   -m run all the dicts gathered from building TriggerMenuXML'
    print '   -f run from test_dicts.'
    print '   -t run topo chains from . TriggerMEnuXML dicts'
    print '   -s silent (but stops pdb prompt...)'
    print '   -n name of file (default depends on option)'

if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'htmsfn:', [])
    except getopt.GetoptError as err:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    if len(opts) > 2:
        print 'Wrong number of arguments ', str(opts)
        usage()
        sys.exit(1)

    silent = False
    do_all = False
    do_testdicts = False
    do_topo = False
    fn = ''
    for o, a in opts:
        print o, a
        if o == '-s':
            silent = True
        if o == '-m':
            do_all = True
        elif o == '-f':
            do_testdicts = True
        elif o == '-t':
            do_topo = True
        elif o == '-n':
            fn = a
        else:
            usage()

    dicts = []
    if fn:
        try:
            dicts = importlib.import_module(fn).dicts
        except:
            print 'Error, could not find a dicts name in ', fn
            sys.exit(0)

    if do_all: run_triggerMenuXML_dicts(silent, dicts, printChains=True)
    if do_testdicts: run_test_dicts(silent, dicts)
    if do_topo: run_from_topo(silent, dicts)
        
            

