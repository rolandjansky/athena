
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Excerciser functions for JetDef.py"""

from exc2string import exc2string2

def _generate(d, silent):

    #if silent:
    #    controller = StdOutController()
    #    controller.off()
        
    cd  = generateChainDefs(d)
    sys.settrace(None)

    # if silent: controller.on()

    return cd

def show_names():

    from test_dicts import dicts
    for name in sorted([d['chainName'] for d in dicts]): print name

    
def run_all_dicts(silent,
                  use_atlas_config=True,
                  debug=True,
                  printChains=True,
                  chains='',
                  dicts=[]):

    from test_dicts import dicts
    m =  'have [%d] dicts, set $JETDEF_DEBUG, '\
         '$JETDEF_DEBUG_NO_INSTANTIATION'\
         ' to write to disk' % (len(dicts))
    print m

    names = chains.split(':') if chains else []
    if names:
        print 'searching for  %d chains' % len(names)
        dicts = [d for d in dicts if d['chainName'] in names]
        print 'found %d chains' % len(dicts)
    else:
        print 'processsing all %d dicts' % len(dicts)
    
    # devnull = open(os.devnull, 'w')
    # old_out = sys.stdout
    # old_err = sys.stderr
    
    result = []
    ndicts = 0
    # toSkip = (
    #     'j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY',
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
            print exc2string2()
            assert False
        ndicts += 1
        # sys.stdout = old_out
        # sys.stderr = old_err

    print 'have [%d] chainDefs' % len(result)
    return result

def run_test_dicts(silent, dicts=[]):
    """Function to run jetdef from a text file containing a single dict"""

    chainDefs = []
    for d in dicts:
        print d
        print d['chainName']
        try:
            cd = _generate(d, silent)
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

def get_dicts(module_name='test_dicts'):
    module = __import__(module_name, globals(), locals(), ['dicts'])
    return getattr(module, 'dicts') 


def usage():
    print 'test_functions: Create ChainDefs instances from dictionaries'
    print '   -a run all the dicts gathered from building TriggerMenuXML'
    print '   -l run on colon separated chain names'
    print '   -m <module_name> run from dicts in module <module_name>'
    print '   -t run topo chains from . TriggerMEnuXML dicts'
    print '   -s silent (but stops pdb prompt...)'
    print '   -n show the available chain names'
    print '   -e print only ErrorChainDef objects'

if __name__ == '__main__':
    import getopt, sys, os
    # from exc2string import exc2string2 

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'htasm:nl:e', [])
    except getopt.GetoptError as err:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    silent = False
    do_all = False
    do_testdicts = False
    do_topo = False
    chains = ''
    module_name = 'test_dicts'
    print_error_only = False
    for o, a in opts:
        if o == '-n':
            show_names()
            sys.exit(0)
        elif o == '-s':
            silent = True
        elif o == '-a':
            do_all = True
        elif o == '-l':
            do_all = True
            chains = a
        elif o == '-m':
            do_testdicts = True
            module_name = a
        elif o == '-t':
            do_topo = True
        elif o == '-h':
            usage()
            sys.exit(0)
        elif o == '-e':
            print_error_only = True
        
        else:
            print 'unknown option', o, do_all
            usage()
            sys.exit(0)

    print 'importing modules (slow!)'
    from AthenaCommon.Include import include

    # start - junk code to stop unit tests complaining about unused imports
    include('include_dummy.py')
    from AthenaCommon.OldStyleConfig import  Service
    try:
        Service('')
    except:
        pass
    # end - junk code to stop unit tests complaining about unused imports

    
    # from StdOutController import StdOutController
    # import importlib

    from generateJetChainDefs import  generateChainDefs

    dicts = get_dicts(module_name)
    chainConfigs = []
    if do_all:
        chainConfigs = run_all_dicts(silent, printChains=True, chains=chains,
                                     dicts=dicts)
    if do_testdicts:
        chainConfigs = run_test_dicts(silent, dicts=dicts)
    if do_topo:
        chainConfigs = run_from_topo(silent, dicts)

    to_print = chainConfigs
    if print_error_only:
        to_print = [c for c in chainConfigs if
                    c.__class__.__name__ != 'ChainDef']

    for c in to_print: print '\n' + str(c)
         
    n_ChainDef = 0
    n_ErrorChainDef = 0
    for c in chainConfigs:
        if c.__class__.__name__ == 'ChainDef':
            n_ChainDef += 1
        else:
            n_ErrorChainDef += 1

    print 'Retrieved dicts                %d' % len(dicts)
    print 'name selector                  %s' % chains
    print 'No of ChainConfig objects      %d' % n_ChainDef
    print 'No of ErrorChainConfig objects %d' % n_ErrorChainDef
    print 'done'

