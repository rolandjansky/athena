# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Accept requests from central trigger menu. Check incoming information for
validity and repackage before forwarding ot to the ChainDef generating
code."""
import os
import re
import copy
import sys
import getopt
import shelve

from TriggerMenu.menu.L1Seeds import getInputTEfromL1Item

from JetSequencesBuilder import JetSequencesBuilder
from TriggerMenu.menu.ChainDef import (ChainDef,
                                       ErrorChainDef)
from exc2string import exc2string2
from InstantiatorFactory import instantiatorFactory
from SequenceTree import SequenceLinear
from ChainConfigMaker import ChainConfigMaker
from AlgFactory import AlgFactory

from JetCleanMonitoring import JetChainsToKeepMonitoring
from TriggerMenu.menu.CleanMonitoring import KeepMonitoring
from TriggerMenu.menu.CleanMonitoring import DisableMonitoringButValAndTime

try:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger("TriggerMenu.jet.generateJetChainDefs")
except:
    logger = None

def _check_input(in_data):
    """Sanity checks on the data passed in from the central menu code."""

    err_hdr = '_check_input: '

    # input data check
    must_have = ('chainName', 'chainParts')
    for k in must_have:
        missing = [k for k in must_have if k not in in_data]
        if missing:
            msg = '%s missing chainPart keys: %s not in %s' % (
                err_hdr,
                ', '.join(missing),
                in_data.keys())
            raise RuntimeError(msg)

    chain_parts = in_data['chainParts']

    # if there are more than one chain part, the differences should
    # only refer to the jet hypo. If this is not the case, we do
    # not know what is going on
    _check_chainpart_consistency(chain_parts)
    _check_values(chain_parts)


def _check_values(chain_parts):

    err_hdr = '_check_values: '

    # jet chains and "HT" chains are processed
    bad = [p['signature'] for p in chain_parts if p['signature'] not in  ('Jet',
                                                                          'HT')]
    if bad:
        msg = '%s unknown chain part(s): %s' % (err_hdr, ' '.join(bad))
        raise RuntimeError(msg)
        

    # input data check
    must_have = ('etaRange', 'threshold', 'recoAlg', 'dataType', 'calib',
                 'addInfo', 'jetCalib', 'cleaning')

    for cp in chain_parts:
        missing = [k for k in must_have if k not in cp]
        if missing:
            msg = '%s missing chainPart keys: %s' % (err_hdr,
                                                     ', '.join(missing))
            raise RuntimeError(msg)

    dataTypes = [p['dataType'] for p in chain_parts]
    bad = [r for r in dataTypes if r not in ('TT', 'tc', 'ion')]

    if bad:
        msg = '%s unknown dataType(s): %s' % (err_hdr, ' '.join(bad))
        raise RuntimeError(msg)



def _check_chainpart_consistency(chain_parts):
    check_chain_parts = [copy.deepcopy(c) for c in chain_parts]
    
    def remove_hypodata(d):
        """Remove those from checks those entries allowed to vary across
        the different chainParts"""

        to_remove = ['multiplicity', 'etaRange', 'threshold', 'chainPartName',
                     'addInfo', 'bTag', 'bTracking', 'bConfig', 'topo', 'bMatching']
        for tr in to_remove: 
            try:
                del d[tr]
            except KeyError:
                pass

    [remove_hypodata(c) for c in check_chain_parts]
    c0 = check_chain_parts[0]
    for c in check_chain_parts[1:]:
        if c != c0:
            msg = '_check_chainpart_consistency: chain parts differ: '\
                '%s %s' % (str(c0), str(c))
            raise RuntimeError(msg)


def _make_sequences(alg_lists, start_te, chain_name):
    """Use a SequenceLinear to set up a sequence of sequences"""

    st = SequenceLinear(start_te, alg_lists, chain_name)
    return st.sequences

def _make_chaindef(from_central, instantiator):

    # print '----> _make_chaindef: dumping from central\n'
    # print from_central
    # print
    # assert False

    _check_input(from_central)

    # rearrange the input data to produce chain_config
    ccm = ChainConfigMaker(from_central)
    chain_config = ccm()

    chain_name = chain_config.chain_name

    alg_factory = AlgFactory(chain_config)
    seq_builder = JetSequencesBuilder(alg_factory, chain_config)

    # get the alg_lists (which will combine with trigger element names
    # to formsequences) for the chain
    alg_lists = seq_builder.make_alglists()

    # ... but chain names start with HLT_
    #header = 'HLT_'
    #if not chain_name.startswith(header):
    #    chain_name = header + chain_name
    #    final_chain_name= header + from_central['chainName']

    
    final_chain_name= from_central['chainName']
        
    # combine the alg_lists and the start sequence trigger element name
    # to produce a list if sequences

    start_te = _make_start_te(chain_config=chain_config)

    sequences = _make_sequences(alg_lists,
                                start_te,
                                chain_name=chain_config.chain_name)

    # convert the algorithms according to the instantiator type
    [s.instantiateAlgs(instantiator) for s in sequences]

    # create an empty ChainDef
    chain_def = ChainDef(chain_name=final_chain_name,
                         #level='HLT',
                         level='EF',
                         lower_chain_name=chain_config.seed)

    # add sequence and signature (check point) information to it

    disableMon = not KeepMonitoring(final_chain_name,JetChainsToKeepMonitoring, strictComparison = True) #reduce number of online histograms according to a whitelist, strictComparison is needed as e.g. j25 is found as a substring in other chain names

    sig_ind = 0
    for s in sequences:
        
        if disableMon: #if block used to remove online histograms of a hypo's AthenaMonTools
            if "hypo" in s.alias:
                for thisalg in s.alg_list:
                    if hasattr(thisalg,"AthenaMonTools"):
                        thisalg.AthenaMonTools = DisableMonitoringButValAndTime(thisalg.AthenaMonTools)
    
        sig_ind += 1
        chain_def.addSequence(listOfAlgorithmInstances=s.alg_list,
                              te_in=s.te_in,
                              te_out=s.te_out)

        chain_def.addSignature(signature_counter=sig_ind,
                               listOfTriggerElements=[s.te_out])



    chain_def.chain_name = "HLT_"+final_chain_name
    chain_def.level = "HLT"
    
    return chain_def


def _is_full_scan(chain_config):
    return chain_config.menu_data.scan_type == 'FS'


# def _make_start_te(chain_config):
# 
#     if _is_full_scan(chain_config):
#         return ''  # the te_in name for a full scan is ''
# 
#     pat = r'^L1_\d?'
#     seed  = chain_config.seed
#     start_te = re.split(pat, seed)
#     if len(start_te) != 2:
#         msg = '_make_chain_def, L1 seed %s does not match %s' % (seed, pat)
#         raise RuntimeError(msg)
# 
#     #chandle special case: for L1_TAU the start te should be HA
#     result = start_te[1]
#     result = result.replace('TAU', 'HA')
#     return result
# 
# 
def _make_start_te(chain_config):

    if _is_full_scan(chain_config):
        return ''  # the te_in name for a full scan is ''

    # not sure about the following imported function...
    # its return type can vary....
    try:
        te = getInputTEfromL1Item(chain_config.seed)
    except:
        raise RuntimeError(
            'JetDef._make_start_te: Unable to obtain te name for L1')

    if isinstance(te, str):
        return te

    if isinstance(te, list):
        return te[0]

    raise RuntimeError('JetDef._make_start_te: unknown te type %s' % str(te))


def generateHLTChainDef(caller_data):
    """Entrance point to the jet slice configuration code.
    Arguments:
    caller_data [type - dictionary]: data from the caller and returns
                either a ChainDef or an ErrorChainDef object.

    Debug and testing actions are controlled by environment variables.
    See commnets in usage()."""

    # maintain a copy of the incoming dictionary - to be used
    # for debugging, will not be overwritten.

    caller_data_copy = copy.deepcopy(caller_data)
    
    caller_data_copy['run_rtt_diags'] = 'JETDEF_TEST' in os.environ
    debug = 'JETDEF_DEBUG' in os.environ
    no_instantiation_flag = 'JETDEF_NO_INSTANTIATION' in os.environ
    use_atlas_config = not no_instantiation_flag

    try:
        # instantiator instantiation can fail if there are
        # ATLAS import errors
        instantiator = instantiatorFactory(use_atlas_config)
    except Exception, e:
        tb = exc2string2()
        msg = 'JetDef Instantiator error: error: %s\n%s' % (str(e), tb)
        chain_name = caller_data_copy['chainName']

        cd = ErrorChainDef(msg, chain_name)
        if debug:
            # for debugging, output the original incoming dictionary
            dump_chaindef(caller_data, cd, no_instantiation_flag)

        return cd
            
    try:
        cd = _make_chaindef(caller_data_copy, instantiator)
    except Exception, e:
        tb = exc2string2()
        chain_name = caller_data_copy['chainName']
        msg = 'JetDef error: error: %s\n%s' % (str(e), tb)
        cd = ErrorChainDef(msg, chain_name)
        if logger:
            logger.warning(str(cd))

    if debug:
        # for debugging, output the original incoming dictionary
        dump_chaindef(caller_data, cd, no_instantiation_flag)
    return cd

def dump_chaindef(caller_data, cd, no_instantiation_flag):
    """Dump incoming dictionaly and outfgoing(Error)ChainDef to a file."""

    chain_name = caller_data['chainName']
    ddir = os.path.join('/tmp/', os.environ['USER'])
    if not os.path.exists(ddir):
        os.mkdir(ddir)
    fn = os.path.join(ddir, chain_name)
    fn = fn.replace(' ', '_')
    txt = 'Input dictionary:\n%s\nChainDef:\n%s' % (str(caller_data), str(cd))
    with open(fn, 'w') as off:
        off.write(txt)

    # If instantiation is off, ChainDef instances
    # hold only string data, and
    # can be stored for later investigation.

    if no_instantiation_flag:
        db =  shelve.open(os.path.join(ddir,
                                       'chain_defs.db'))
        db[chain_name] = cd
        db.close()

    print 'Debug output written to ', fn


def usage():
    print """\
    python JetDef.py

    Run the jet slice configuration code in stand alone node.
    Input dictionaries have been hardwired into the test code.
    These are used as the input to JetDef. ChainDef objects are
    returned.

    Debug output is controlled by the following environment variables
    $JETDEF_DEBUG 1 -            ChainDef and ErrorChainDef objects are
                                 written to /tmp/<username>

    $JETDEF_NO_INSTANTIATION 1 - No instantiation of the ATLAS
                                 configuration objects is done.
                                 A string representation of the Algorithms
                                 is presented. The arguments to the
                                 Algoruthms are visble.
        
    $JETDEF_TEST           - Diagnosis Algorithms are added to the
                             production chains.

    Performing
    export JETDEF_DEBUG=1;export JETDEF_NO_INSTANTIATION=1

    will produce useful debug information in /tmp/<usename>.
 """

if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    verbose = False
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        else:
            assert False, "unhandled option"

    # chain_defs = run_test()
    from test_functions import run_strawman_test
    chain_defs = run_strawman_test()
    for c in chain_defs:
        print '\n-----------------------\n'
        print c

    print 'done'
    
