# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from __future__ import absolute_import
import re

# substrings that cannot occur in any chainPartName for simple chains.
reject_substr = (
    #    'gsc',
    'ion',
    'dphi',
    'deta',
    'invm',
    '0i1',
    '1i2',
    'dphi',
    r'agg\d',)

reject_substr_res = re.compile(r'%s' % '|'.join(reject_substr))


def _select_simple_chainparts(chain_parts):
    """ Reject unsuported chain parts """

    for cp in chain_parts:
        if  reject_substr_res.search(cp['chainPartName']):
            return False

    return True


def _make_simple_label(chain_parts, leg_label):
    """Marshal information deom the selected chainParts to create a
    'simple' label. NOTE: THIS IS A SPECIAL CASE - IT DOES NOT DEPEND
    SOLELY ON THE HYPO SCENARIO.
    Argument leg_label is not used - rather the leg label is fouNd
    from the chain parts.
    """
    
    if not _select_simple_chainparts(chain_parts):
        msg = 'Jet Configuration error: '\
              'chain fails substring selection: not "simple" '

        raise NotImplementedError(msg)

    chainpartind = 0
    label = 'simple(['
    for cp in chain_parts:
        smcstr =  str(cp['smc'])
        jvtstr =  str(cp['jvt'])
        momstr =  str(cp['momCuts'])
        if smcstr == 'nosmc':
            smcstr = ''
        for i in range(int(cp['multiplicity'])):
            # condition_str = '(%set,%s,%s)' % (str(cp['threshold']),
            #                                  str(cp['etaRange']),
            #                                  smcstr,)
            condition_str = '(%set,%s' % (str(cp['threshold']),
                                              str(cp['etaRange']),)
            if smcstr: # Run 2 chains have "INF" in the SMC substring
                condition_str += ',%s' % smcstr.replace('INF','')
            if jvtstr:
                condition_str += ',%s' % jvtstr
            if momstr:
                if 'SEP' in momstr:
                    print('_cuts_from_momCuts(momstr):')
                    print(_cuts_from_momCuts(momstr))
                    for cut in _cuts_from_momCuts(momstr):
                        condition_str += ',%s' % cut
                else:
                    condition_str += ',%s' % momstr
            condition_str += ', leg{:0>3}'.format(chainpartind)
            if not condition_str.endswith(')'):
                condition_str += ')'
            label += condition_str
        chainpartind += 1

    label += '])'
    return label


def _args_from_scenario(scenario):
    separator = 'SEP'
    
    args = scenario.split(separator)
    if len(args) > 1:
        return args[1:]
    return ''


def _cuts_from_momCuts(momCuts):
    separator = 'SEP'
    args      = momCuts.split(separator)
    if len(args) > 1:
        return args
    return ''


def _make_vbenf_label(chain_parts, leg_label):
    """Marshal information from the selected chainParts to create a
    vbenf label. Use a Reducer for elimination of unusable jets
    """

    # toy label for development: run simple and dijet independently.
    # simple makes Et cuts on two jets. Independently (sharing possible)
    # of jets choosean by simple,  the dijet
    # scenario requires a dijet of mass > 900, and opening angle in phi > 2.6

    assert len(chain_parts) == 1
    
    scenario = chain_parts[0]['hypoScenario']
    assert scenario.startswith('vbenf')
    args = _args_from_scenario(scenario)
    if not args:
        return 'all([]simple([(50et)(70et)])dijet([(900djmass, 26djdphi)] all[], all[])))'        
    arg_res = [
        re.compile(r'(?P<lo>\d*)(?P<key>fbet)(?P<hi>\d*)'),
        re.compile(r'(?P<lo>\d*)(?P<key>mass)(?P<hi>\d*)'),
        re.compile(r'(?P<lo>\d*)(?P<key>et)(?P<hi>\d*)'),
    ]

    defaults = {
        'et': ('101', 'inf'),
        'mass': ('800', 'inf'),
        'fbet': ('501', 'inf'),
    }

    argvals = {}
    while args:
        assert len(args) == len(arg_res)
        arg = args.pop()
        for r in arg_res:
            m = r.match(arg)
            if m is not None:
                arg_res.remove(r)
                gd = m.groupdict()
                key = gd['key']
                try:
                    lo = float(gd['lo'])
                except ValueError:
                    lo = defaults[key][0]
                argvals[key+'lo'] = lo 
                try:
                    hi = float(gd['hi'])
                except ValueError:
                    hi = defaults[key][1]
                argvals[key+'hi'] =  hi

    assert len(args) == len(arg_res)
    assert len(args) == 0

    argvals['leg_label'] = leg_label
    return """
    all
    (
      []
      simple
      (
        [(%(fbetlo).0fet, 500neta, leg000)(%(fbetlo).0fet, peta500, %(leg_label)s)]
      )
      dijet
      (
        [(%(masslo).0fdjmass, 26djdphi)]
        simple
        (
          [(10et, 0eta320, leg000)(20et, 0eta320, %(leg_label)s)]
        )
      )
    )""" % argvals


def  _make_fbdjshared_label(chain_parts, leg_label):
    """example label for a 2-tree forest.
    The fbdjshared contains a dijet and forward backward jets, in separate 
    trees, to allow the fb jets to particoate in the dijet."""

    
    return """
    simple
    (
    [(50et, 500neta, leg000)(50et, peta500, leg000)]
    )
    dijet
    (
    [(34djmass, 26djdphi)]
        simple
        ([(10et, 0eta320, leg000)(20et, 0eta320, leg000)])
    )"""

    
def _make_dijet_label(chain_parts, leg_label):
    """dijet label. supports dijet cuts, and cuts on particpating jets
    Currently supported cuts:
    - dijet mass
    - jet1 et, eta
    - jet2 et, eta

    - default values are used for unspecified cuts.
    The cut set can be extended according to the pattern
    """

    assert len(chain_parts) == 1
    scenario = chain_parts[0]['hypoScenario']
    
    assert scenario.startswith('dijet')

    arg_res = [
        re.compile(r'^(?P<lo>\d*)(?P<key>djmass)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>j1et)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>j1eta)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>j2et)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>j2eta)(?P<hi>\d*)$'),
    ]

    defaults = {
        'j1et': ('100', 'inf'),
        'j2et': ('100', 'inf'),
        'j1eta': ('0', '320'),
        'j2eta': ('0', '320'),
        'djmass': ('1000', 'inf'),
    }


    args = _args_from_scenario(scenario)
    argvals = {}
    while args:
        assert len(args) == len(arg_res)
        arg = args.pop()
        for r in arg_res:
            m = r.match(arg)
            if m is not None:
                arg_res.remove(r)
                gd = m.groupdict()
                key = gd['key']

                try:
                    lo = float(gd['lo'])
                except ValueError:
                    lo = defaults[key][0]
                argvals[key+'lo'] = lo 
                try:
                    hi = float(gd['hi'])
                except ValueError:
                    hi = defaults[key][1]
                argvals[key+'hi'] =  hi

    assert len(args) == len(arg_res)
    assert len(args) == 0

    argvals['leg_label'] = leg_label
    
    return """
    dijet(
    [(%(djmasslo).0fdjmass)]
    simple([(%(j1etlo).0fet, %(j1etalo).0feta%(j1etahi).0f, %(leg_label)s)
    (%(j2etlo).0fet, %(j2etalo).0feta%(j2etahi).0f, %(leg_label)s)]))""" % argvals


def _make_agg_label(chain_parts, leg_label):
    """agg label. cuts on aggregate quantities, and cuts on particpating jets
    Only partway migrated from pure ht to more general agg
    Currently supported cuts:
    - all jets: ht
    - all jets: et
    - all jets: eta

    - default values are used for unspecified cuts.
    The cut set can be extended according to the pattern
    """

    assert len(chain_parts) == 1, '_make_agg_label, no. of chain parts != 1'
    scenario = chain_parts[0]['hypoScenario']
    
    assert scenario.startswith('agg'), '_make_agg_label(): scenario does not start with agg'

    arg_res = [
        re.compile(r'^(?P<lo>\d*)(?P<key>ht)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>et)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>eta)(?P<hi>\d*)$'),
    ]

    defaults = {
        'ht': ('0', 'inf'),
        'et': ('0', 'inf'),
        'eta': ('0', 'inf'),
     }


    args = _args_from_scenario(scenario)
    argvals = {}
    nargs = len(args)
    assert len(args) <= len(arg_res), 'bad num of args %d, expected < %d' % (len(args),
                                                                             len(arg_res))

    # obtain argument values frrom scenario
    while args:
        arg = args.pop()
        for r in arg_res:
            m = r.match(arg)
            if m is not None:
                arg_res.remove(r)
                gd = m.groupdict()
                key = gd['key']

                try:
                    lo = float(gd['lo'])
                except ValueError:
                    lo = float(defaults[key][0])
                argvals[key+'lo'] = lo 
                try:
                    hi = float(gd['hi'])
                except ValueError:
                    hi = float(defaults[key][1])
                argvals[key+'hi'] =  hi

    assert len(argvals) == 2*nargs, 'no of args: %d, expected %d' % (len(argvals), 2*nargs)

    argvals['leg_label'] = leg_label
    result =  """
    agg([(%(htlo).0fht, %(leg_label)s)
        (%(etlo).0fet)
    (%(etalo).0feta%(etahi).0f)
    ])"""  % argvals
    print (result)
    return result
    

def chainDict2jetLabel(chain_dict):
    """Entry point to this Module. Return a chain label according to the
    value of cp['hypoScenario'], where cp is an element of list/
    chainDict['chainPart']

    Due to historical reasons, the ;logic is as sollows:
    hypoScenatio   Action
    "simple"       examine all chain parts, to greate an EtaEt label.
    other          if len(chainParts) == 1 create correponding  chain label
                   if len(chainParts) > 1 create and of simple and other.
    """

    # suported scenarios 
    router = {
        'simple': _make_simple_label,
        'agg':   _make_agg_label,
        'vbenf': _make_vbenf_label,
        'dijet': _make_dijet_label,
        'fbdjshared': _make_fbdjshared_label,
    }

    # chain_part - scenario association
    cp_sorter = {}
    for k in router: cp_sorter[k] = []

    chain_parts = chain_dict['chainParts']
    for cp in chain_parts:
        if cp['signature'] != 'Jet' and cp['signature'] != 'Bjet': 
            continue
        for k in cp_sorter:
            if cp['hypoScenario'].startswith(k):
                cp_sorter[k].append(cp)
                break

    # obtain labels by scenario.
    labels = []

    leg_label = 'leg%03d' % (len(chain_parts) - 1)
    for k, chain_parts in cp_sorter.items():
        if chain_parts:
            labels.append(router[k](chain_parts, leg_label))

    assert labels
    nlabels = len(labels)
    return ''.join(labels)
    if nlabels == 1: return labels[0]
    if nlabels == 2:
        # two labels occur when combining simple and a non-simple scenario
        alabel = """\
all([]
    %s
    %s)""" % (tuple(labels))
        return alabel

    # more than 2 labels is not expected
    assert False

# module tests now in testChainDictMaker.py
