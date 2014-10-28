# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Accept requests from central trigger menu. Check incoming information for
validity and repackage before forwarding ot to the ChainDef generating
code."""
import os
import re
import copy

from JetSequenceRouter import JetSequenceRouter
from TriggerMenu.menu.ChainDef import (ChainDef,
                                       ErrorChainDef)
from exc2string import exc2string2
from InstantiatorFactory import instantiatorFactory
from SequenceTree import SequenceTree
from ChainConfigMaker import ChainConfigMaker


def _check_input(in_data):
    """Sanity checks on the data passed in from the central menu code."""

    err_hdr = '_check_input: '

    # input data check
    must_have = ('chainName', 'chainParts')
    for k in must_have:
        missing = [k for k in must_have if k not in in_data]
        if missing:
            msg = '%s missing chainPart keys: %s' % (err_hdr,
                                                     ', '.join(missing))
            raise RuntimeError(msg)

    # expect two chain parts for bjets: these sepecify different
    # multiplicities and thresholds for the jet hypo.
    # all remaining dictionaries should be the same.

    chain_parts = in_data['chainParts']
    if len(chain_parts) > 2:
        msg = '%s Unexpected input data: no of chain' \
            ' parts %s: max: 2' % (err_hdr, len(chain_parts))
        raise RuntimeError(msg)

    # if there are more than one chain part, the differences should
    # only refer to the jet hypo. If this is not the case, we do
    # not know what is going on
    _check_chainpart_consistency(chain_parts)
    
    bad = [p['signature'] for p in chain_parts if p['signature'] != 'Jet']
    if bad:
        msg = '%s unknown chain part(s): %s' % (err_hdr, ' '.join(bad))
        raise RuntimeError(msg)
        

    # input data check
    must_have = ('etaRange', 'threshold', 'recoAlg', 'dataType', 'calib')
    for cp in chain_parts:
        missing = [k for k in must_have if k not in cp]
        if missing:
            msg = '%s missing chainPart keys: %s' % (err_hdr,
                                                     ', '.join(missing))
            raise RuntimeError(msg)

    dataTypes = [p['dataType'] for p in chain_parts]
    bad = [r for r in dataTypes if r not in ('TT', 'tc')]

    if bad:
        msg = '%s unknown dataType(s): %s' % (err_hdr, ' '.join(bad))
        raise RuntimeError(msg)

    if 'tc' not in dataTypes:
        msg = '%s "tc" not in dataTypes: %s' % (err_hdr, ' '.join(dataTypes))
        raise RuntimeError(msg)


def _check_chainpart_consistency(chain_parts):
    check_chain_parts = [copy.deepcopy(c) for c in chain_parts]
    def remove_hypodata(d):
        to_remove = ['multiplicity', 'etaRange', 'threshold', 'chainPartName']
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
    """Use a SequenceTree to set up a DAG of sequences"""

    st = SequenceTree(start_te, alg_lists, chain_name)
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

    chain_name = chain_config.name
    
    router = JetSequenceRouter()

    # get the alg_lists (which will combine with trigger element names
    # to formsequences) for the chain
    alg_lists = router.make_alglists(chain_config)

    # ... but chain names start with HLT_
    #header = 'HLT_'
    #if not chain_name.startswith(header):
    #    chain_name = header + chain_name
    #    final_chain_name= header + from_central['chainName']

    
    final_chain_name= from_central['chainName']
        
    # combine the alg_lists and the start sequence trigger element name
    # to produce a list if sequences

    # the start te will either by  '' for a full scan, or dereived
    # from the L1 seed chain name.
    start_te = _make_start_te(chain_name=chain_name,
                              chain_config=chain_config)

    sequences = _make_sequences(alg_lists, start_te, chain_name)

    # convert the algorithms according to the instantiator type
    [s.instantiateAlgs(instantiator) for s in sequences]

    # create an empty ChainDef
    chain_def = ChainDef(chain_name=final_chain_name,
                         #level='HLT',
                         level='EF',
                         lower_chain_name=chain_config.seed)

    # add sequence and signature (check point) information to it
    sig_ind = 0
    for s in sequences:

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
    menu_data = chain_config.jr_menudata
    if menu_data:
        if menu_data.scan_type == 'FS':
            return True
    return False


def _make_start_te(chain_name, chain_config):

    if _is_full_scan(chain_config):
        return ''  # the te_in name for a full scan is ''

    pat = r'^L1_\d?'
    seed  = chain_config.seed
    start_te = re.split(pat, seed)
    if len(start_te) != 2:
        msg = '_make_chain_def, L1 seed %s does not match %s' % (seed, pat)
        raise RuntimeError(msg)
    return start_te[1]

def generateHLTChainDef(caller_data, use_atlas_config=True, debug=False):
    """Entrance point to the jet slice configuration code.
    Arguments:
    caller_data [type - dictionary]: data from the caller and returns
                either a ChainDef or an ErrorChainDef object.

    use_atlas_config [type - boolean]: True - instantiate ATLAS configuration
                     objects, which requires the succesful imports of the
                     python classes.
                     False - Use JetDef proxies of the ATLAS configuration
                     objects. These have natural types as attributes,
                     and are suitable for development work."""

    caller_data['test'] = ''
    # debug = True
    # caller_data['test'] = 'test0'
    # use_atlas_config = False

    try:
        # instantiator instantiation can fail if there are
        # ATLAS impoirt errors
        instantiator = instantiatorFactory(use_atlas_config)
    except Exception, e:
        tb = exc2string2()
        msg = 'JetDef Instantiator error: error: %s\n%s' % (str(e), tb)
        chain_name = caller_data['chainName']

        cd = ErrorChainDef(msg, chain_name)
        if debug:
            _debug(caller_data, cd)

        return cd
            

    try:
        cd = _make_chaindef(caller_data, instantiator)
    except Exception, e:
        tb = exc2string2()
        chain_name = caller_data['chainName']
        msg = 'JetDef error: error: %s\n%s' % (str(e), tb)
        cd = ErrorChainDef(msg, chain_name)

    if debug:
        _debug(caller_data, cd)
    return cd

def _debug(caller_data, cd):
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

    print 'Debug output written to ', fn


def inclusive_jetdata_good_test():
    inclusive_jet_data ={'EBstep': 1,
                         'signatures': '',
                         'stream': 'Jet',
                         'chainParts': [{'dataType': 'tc',
                                         'trigType': 'j',
                                         'extra': '',
                                         'multiplicity': '1',
                                         'FSinfo': '',
                                         'etaRange': ['0eta320'],
                                         'topo': [],
                                         'calib': 'had',
                                         'L1item': '',
                                         'threshold': '30',
                                         'addInfo': [],
                                         'signature': 'Jet',
                                         'chainPartName': 'j30_had',
                                         'recoAlg': 'a4'}],
                         'topo': '',
                         'chainCounter': 890,
                         'signature': 'Jet',
                         'L1item': 'L1_J20',
                         'chainName': 'j30_had'}

    return inclusive_jet_data


def inclusive_jetdata_set(chain_part_update, jetdata_update={}):
    incl_jet_data = inclusive_jetdata_good_test()
    incl_jet_data['chainParts'][0].update(chain_part_update)
    incl_jet_data.update(jetdata_update)
    return incl_jet_data


def inclusive_jetdata_setthreshold(threshold):
    return inclusive_jetdata_set({'threshold': str(threshold)})


def inclusive_jetdata_bad_test():
    return inclusive_jetdata_setthreshold(-999)
    
        
def run_test():

    from_central = [inclusive_jetdata_good_test(),
                    inclusive_jetdata_bad_test()]
                    
    return [generateHLTChainDef(fc, debug=True) for fc in from_central]

def run_strawman_test(use_atlas_config=True, debug=True):
    """run JetDef from dictionaries produced by menu code 16/5/2014 ~19:00"""

    from_central = [
        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '3', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '175', 'addInfo': [], 'chainPartName': '3j175', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 110, 'groups': ['RATE:MultiJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': '3j175'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '4', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '100', 'addInfo': [], 'chainPartName': '4j100', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 111, 'groups': ['RATE:MultiJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_3J50', 'chainName': '4j100'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '5', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '85', 'addInfo': [], 'chainPartName': '5j85', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 112, 'groups': ['RATE:MultiJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_4J20', 'chainName': '5j85'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '110', 'addInfo': [], 'chainPartName': 'j110', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 105, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J50', 'chainName': 'j110'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '150', 'addInfo': [], 'chainPartName': 'j150', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 106, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J75', 'chainName': 'j150'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '200', 'addInfo': [], 'chainPartName': 'j200', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 107, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': 'j200'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '260', 'addInfo': [], 'chainPartName': 'j260', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 108, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': 'j260'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '330', 'addInfo': [], 'chainPartName': 'j330', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 109, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': 'j330'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '400', 'addInfo': [], 'chainPartName': 'j400', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 100, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': 'j400'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '460', 'addInfo': [], 'chainPartName': 'j460_a10_L1J100', 'recoAlg': 'a10'}], 'topo': [], 'chainCounter': 101, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': 'j460_a10_L1J100'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '60', 'addInfo': [], 'chainPartName': 'j60', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 103, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J20', 'chainName': 'j60'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '320eta490', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '200', 'addInfo': [], 'chainPartName': 'j200_320eta490', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 102, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_FJ100', 'chainName': 'j200_320eta490'},
        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'trigType': 'j', 'scan': 'PS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '110', 'addInfo': [], 'chainPartName': 'j110', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 105, 'groups': ['RATE:SingleJet', 'BW:Jets'], 'signature': 'Jet', 'L1item': 'L1_J50', 'chainName': 'j110'},

        {'EBstep': 1, 'signatures': '', 'stream': ['Jet'], 'chainParts': [{'bTag': 'bmedium', 'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '175', 'addInfo': [], 'chainPartName': 'j175_bmedium', 'recoAlg': 'a4'}, {'bTag': 'bmedium', 'trigType': 'j', 'scan': 'FS', 'dataType': 'tc', 'multiplicity': '1', 'extra': '', 'etaRange': '0eta320', 'topo': [], 'calib': 'had', 'L1item': '', 'signature': 'Jet', 'threshold': '60', 'addInfo': [], 'chainPartName': 'j60_bmedium', 'recoAlg': 'a4'}], 'topo': [], 'chainCounter': 14, 'groups': ['RATE:MultiJet', 'BW:Jets'], 'signature': 'Jet', 'test': '', 'L1item': 'L1_J100', 'chainName': 'j175_bmedium_j60_bmedium'}
        ]
    return [generateHLTChainDef(fc,
                                use_atlas_config=use_atlas_config,
                                debug=debug) for fc in from_central]


def run_from_dict_file(fn):
    """Function to run jetdef from a text file containing a single dict"""
    
    d = eval(open(fn).read())
    print d
    return generateHLTChainDef(d, debug=True)


if __name__ == '__main__':
    # chain_defs = run_test()
    chain_defs = run_strawman_test()
    for c in chain_defs:
        print '\n-----------------------\n'
        print c

    print 'done'
