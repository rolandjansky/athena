# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re

# substrings that cannot occur in any chainPartName for simple chains.
reject_substr = ( # noqa: W605
    #    'gsc',
    'ion',
    'dphi',
    'deta',
    'invm',
    '0i1',
    '1i2',
    'dphi',
    'ht\d',)

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
            else:
                condition_str += ')'
            label += condition_str
    label += '])'
    return label


def _make_simple_partition_label(chain_dict):
    """Marshal information deom the selected chainParts to create a
    'simple' label.
    """
    
    cps = _select_simple_chains(chain_dict)
    if not cps:
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
    
    cps = _select_simple_chains(chain_dict)
    if not cps:
        raise NotImplementedError(
            'chain fails substring selection: not "simple": %s' % (
                chain_dict['chainName']))

    simple_strs = []

    for cp in cps:
        chain_dict['chainParts'] = [cp]
        simple_strs.append(_make_simple_label(chain_dict))

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
        return 'and([]simple([(50et)(70et)])combgen([(2)] dijet([(900mass, 26dphi)])))'        
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
        [(%(etlo).0fet)(%(etlo).0fet)]
      )
      combgen
      (
        [(2)(10et)]
        dijet
        (
          [(%(masslo).0fmass, 26dphi)]
        ) 
        simple
        (
          [(10et)(20et)]
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
                  [(%(djmasslo).0fmass)])
            simple([(%(j1etlo).0fet, %(j1etalo).0feta%(j1etahi).0f)
                    (%(j2etlo).0fet, %(j2etalo).0feta%(j2etahi).0f)])
            )""" % argvals


def chainDict2jetLabel(chain_dict):
    """Examine chain_parts in chain_dict. jet chain parts are used to 
    calculate chain_labels according to the hypo scenario. There may be
    more than one chain part used for a single label (only if 
    hypoScanario is 'simple') and there mabe more than one label per chain
    = ;'j80..._j0_dijet... will give rise to  'simple' and 'dijet' labels.
    """

    # suported scenarios 
    router = {
        'simple': _make_simple_label,
        'vbenf': _make_vbenf_label,
        'dijet': _make_dijet_label,
    }

    # chain_part - scenario association
    cp_sorter = {}
    for k in router: cp_sorter[k] = []


    for cp in chain_dict['chainParts']:
        if cp['signature'] != 'Jet': continue
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

def _tests():

    print '\n--------- _tests() starts _______'

    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

    from ChainLabelParser import ChainLabelParser

    chain_names = (
        'HLT_j85',
        'HLT_j80_0eta240_2j60_320eta490',
        'HLT_j85_j70',
        'HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet',
        'HLT_j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass',
        
        
    )
    
    decodeChainName = DictFromChainName.DictFromChainName()

    for cn in chain_names:
        chain_dict = decodeChainName.getChainDict(cn)
        label = chainDict2jetLabel(chain_dict)
        print '\n'
        print cn
        print '  ', label
        print '\n'

        parser = ChainLabelParser(label, debug=False)
        parser.parse()

    print '\n--------- _tests() ends _______'


if __name__ == '__main__':
    _tests()
