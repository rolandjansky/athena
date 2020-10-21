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


def _make_simple_label(chain_parts):
    """Marshal information deom the selected chainParts to create a
    'simple' label. NOTE: THIS IS A SPECIAL CASE - IT DOES NOT DEPEND
    SOLELY ON THE HYPO SCENARIO.
    """
    
    if not _select_simple_chainparts(chain_parts):
        msg = 'Jet Configuration error: '\
              'chain fails substring selection: not "simple" '

        raise NotImplementedError(msg)
    
    label = 'simple(['
    for cp in chain_parts:
        smcstr =  str(cp['smc'])
        jvtstr =  str(cp['jvt'])
        if smcstr == 'nosmc':
            smcstr = ''
        for i in range(int(cp['multiplicity'])):
            # condition_str = '(%set,%s,%s)' % (str(cp['threshold']),
            #                                  str(cp['etaRange']),
            #                                  smcstr,)
            condition_str = '(%set,%s' % (str(cp['threshold']),
                                              str(cp['etaRange']),)
            if smcstr: # Run 2 chains have "INF" in the SMC substring
                condition_str += ',%s)' % smcstr.replace('INF','')
            elif jvtstr:
                condition_str += ',%s)' % jvtstr
            else:
                condition_str += ')'
            label += condition_str
    label += '])'
    return label


def _make_simple_partition_label(chain_dict):
    """Marshal information deom the selected chainParts to create a
    'simple_partition' label.
    """

    cps = chain_dict['chainParts']
    if not (_select_simple_chainparts(cps)):
        raise NotImplementedError(
            'chain fails substring selection: not "simple": %s' % (
                chain_dict['chainName']))
    
    label = 'simplepartition(['
    for cp in cps:
        smcstr =  str(cp['smc'])
        if smcstr == 'nosmc':
            smcstr = ''
        for i in range(int(cp['multiplicity'])):
            # condition_str = '(%set,%s,%s)' % (str(cp['threshold']),
            #                                  str(cp['etaRange']),
            #                                  smcstr,)
            condition_str = '(%set,%s' % (str(cp['threshold']),
                                              str(cp['etaRange']),)
            if smcstr:
                condition_str += ',%s)'
            else:
                condition_str += ')'
            label += condition_str
    label += '])'
    return label


def _make_simple_comb_label(chain_dict):
    """Marshal information deom the selected chainParts to create a
    'simple' label NOTE: DO NOT USE this method.
    THIS CHAINLABEL IS FOR TIMING STUDIES ONLY.
    It has n^2 behaviour rather than n obtained using _make_simple_label.
    """

    cps = chain_dict['chainParts']
    if not (_select_simple_chainparts(cps)):
        raise NotImplementedError(
            'chain fails substring selection: not "simple": %s' % (
                chain_dict['chainName']))
    
    simple_strs = []

    for cp in cps:
        print(cp)
        simple_strs.append(_make_simple_label([cp]))

        label = 'combgen([(%d)]' % len(cps)
        for s in simple_strs:
            label += ' %s ' % s
        label += ')'
    return label


def _args_from_scenario(scenario):
    separator = 'SEP'
    
    args = scenario.split(separator)
    if len(args) > 1:
        return args[1:]
    return ''


def _make_vbenf_label(chain_parts):
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
        return 'and([]simple([(50et)(70et)])combgen([(2)] dijet([(900djmass, 26djdphi)])))'        
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

    return """
    and
    (
      []
      simple
      (
        [(%(etlo).0fet, 500neta)(%(etlo).0fet, peta500)]
      )
      combgen
      (
        [(10et, 0eta320)]
        dijet
        (
          [(%(masslo).0fdjmass, 26djdphi)]
        ) 
        simple
        (
          [(10et, 0eta320)(20et, 0eta320)]
        )
      )
    )""" % argvals


def _make_dijet_label(chain_parts):
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

    return """
    combgen(
            [(2)(%(j1etlo).0fet, %(j1etalo).0feta%(j1etahi).0f)
                (%(j1etlo).0fet, %(j1etalo).0feta%(j1etahi).0f)
               ]
    
            dijet(
                  [(%(djmasslo).0fdjmass)])
            simple([(%(j1etlo).0fet, %(j1etalo).0feta%(j1etahi).0f)
                    (%(j2etlo).0fet, %(j2etalo).0feta%(j2etahi).0f)])
            )""" % argvals


def _make_agg_label(chain_parts):
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

    print (argvals)
    assert len(argvals) == 2*nargs, 'no of args: %d, expected %d' % (len(argvals), 2*nargs)

    print ('sent 100')
    result =  """
    ht([(%(htlo).0fht)
        (%(etlo).0fet)
        (%(etalo).0feta%(etahi).0f)
    ])"""  % argvals
    print (result)
    return result
    


def _make_combinationsTest_label(chain_parts):
    """make test label for  combinations helper with two simple children."""

    assert len(chain_parts) == 1
    scenario = chain_parts[0]['hypoScenario']
    
    assert scenario == 'combinationsTest'

   

    return """
    combgen(
            [(2)(20et, 0eta320)]
    
            simple([(40et, 0eta320) (50et, 0eta320)])
            simple([(35et, 0eta240) (55et, 0eta240)])
            )"""


def _make_partitionsTest_label(chain_parts):
    """make test label for  combinations helper with two simple children."""

    assert len(chain_parts) == 1
    scenario = chain_parts[0]['hypoScenario']
    
    assert scenario == 'partitionsTest'

   

    return """
    partgen(
            [(20et, 0eta320)]
    
            simple([(40et, 0eta320) (50et, 0eta320)])
            simple([(35et, 0eta240) (55et, 0eta240)])
            )"""


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
        'combinationsTest': _make_combinationsTest_label,
        'partitionsTest': _make_partitionsTest_label,
    }

    # chain_part - scenario association
    cp_sorter = {}
    for k in router: cp_sorter[k] = []

    for cp in chain_dict['chainParts']:
        if cp['signature'] != 'Jet' and cp['signature'] != 'Bjet': 
            continue
        for k in cp_sorter:
            if cp['hypoScenario'].startswith(k):
                cp_sorter[k].append(cp)
                break

    # obtain labels by scenario.
    labels = []
    for k, chain_parts in cp_sorter.items():
        if chain_parts: labels.append(router[k](chain_parts))

    assert labels
    nlabels = len(labels)
    if nlabels == 1: return labels[0]
    if nlabels == 2:
        alabel = """\
and([]
    %s
    %s)""" % (tuple(labels))
        return alabel

    # more than 2 labels is not expected
    assert False

# module tests now in testChainDictMaker.py
