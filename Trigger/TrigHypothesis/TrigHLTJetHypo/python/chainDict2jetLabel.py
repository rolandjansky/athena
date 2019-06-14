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


def select_simple_chains(cd):
    """Select chains for which to make an simple chain label.

    Chains selected by requiring that the signature os 'Jet'. Chains are
    vetoed if specific substrings occur in any of the chainPartNames"""

    chain_parts = [c for c in cd['chainParts'] if c['signature'] == 'Jet']

    for cp in chain_parts:
        if  reject_substr_res.search(cp['chainPartName']):
            return []

    return chain_parts


def make_simple_label(chain_dict):
    """Marshal information deom the selected chainParts to create a
    'simple' label.
    """
    
    cps = select_simple_chains(chain_dict)
    if not cps:
        msg = 'Jet Configuration error: '\
              'chain fails substring selection: not "simple" '
        msg +=  chain_dict['chainName']

        raise NotImplementedError(msg)
    
    label = 'simple(['
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


def select_vbenf_chains(scenario):

    """Select chains for which to make a vbenf chain label.
    Chains selected by reuiring that the signature os 'Jet'. Chains are
    vetoed if specific substrings occur in any of the chainPartNames"""


    if scenario.startswith('vbenf'):
        return scenario

    return ''


def args_from_scenario(scenario):
    separator = 'SEP'
    
    args = scenario.split(separator)
    if len(args) > 1:
        return args[1:]
    return ''


def make_vbenf_label(scenario):
    """Marshal information from the selected chainParts to create a
    vbenf label. Use a Reducer for elimination of unusable jets
    """

    # toy label for development: run simple and dijet independently.
    # simple makes Et cuts on two jets. Independently (sharing possible)
    # of jets choosean by simple,  the dijet
    # scenario requires a dijet of mass > 900, and opening angle in phi > 2.6

    assert scenario.startswith('vbenf')
    args = args_from_scenario(scenario)
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


def make_multijetInvmLegacy_label(scenario):
    assert scenario.startswith('multijetInvmLegacy')

    arg_res = [
        re.compile(r'^(?P<key>mult)(?P<val>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>mass)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>eta)(?P<hi>\d*)$'),
        re.compile(r'^(?P<lo>\d*)(?P<key>et)(?P<hi>\d*)$'),
    ]

    defaults = {
        'et': ('100', 'inf'),
        'mass': ('1000', 'inf'),
        'eta': ('0', '320'),
        'mult': '4',
    }


    args = args_from_scenario(scenario)
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

                if key == 'mult':
                    try:
                        val = int(gd['val'])
                    except ValueError:
                        val = int(defaults[key])
                    break
                
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


    simple_args = 4*'(%(etlo).0fet,%(etalo).0feta%(etahi).0f)' % argvals
    argvals['simple_args'] = simple_args

    return """
    and
    (
      []
      simple
      (
        [%(simple_args)s]
      )
      combgen
      (
        [(2)(%(etlo).0fet)]
        dijet
        (
          [(%(masslo).0fmass)]
        ) 
      )
    )""" % argvals


def _test0():
    """Read chainDicts from files, cread simple label if possible"""
    from chainDictSource import chainDictSource

    for cd in chainDictSource():
        f = cd['chainName']
        print '\n---------'
        print f
        try:
            label = make_simple_label(cd)
        except Exception, e:
            print e
            continue
        
        print 'chain label', label
        print '-----------\n'


def _test1():
    scenario = 'vbenfSEP81etSEP34mass35SEP503fbet'
    print 'scenario: ', scenario
    print 'label: ', make_vbenf_label(scenario)
    print
    scenario = 'vbenf'
    print 'scenario: ',scenario, ' - note: no arguments'
    print  'label: ', make_vbenf_label(scenario)

def _test2():
    scenario = 'multijetInvmLegacySEPmult4SEP35etSEP0eta490SEP1000mass'
    print 'scenario: ', scenario

    print 'label: ',  make_multijetInvmLegacy_label(scenario)

if __name__ == '__main__':
    print '_test1'
    _test1()
    print '\n_test2'
    _test2()
