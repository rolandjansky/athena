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

    Chains selected by reuiring that the signature os 'Jet'. Chains are
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
        raise NotImplementedError(
            'chain fails substring selection: not "simple": %s' % (
                chain_dict['chainName']))
    
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
            if smcstr: # Run 2 chains have "INF" in the SMC substring
                condition_str += ',%s)' % smcstr.replace('INF','')
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


def make_vbenf_label(scenario):
    """Marshal information from the selected chainParts to create a
    vbenf label. USe a Reducer for elimination of unusable jets
    """

    # toy label for developement: run simple and dijet independently.
    # simple makes Et cuts on two jets. Independently (sharing possible)
    # of jets choosean by simple,  the dijet
    # scenario requires a dijet of mass > 900, and opening angle in phi > 2.6

    assert scenario.startswith('vbenf')
    separator = 'SEP'
    def get_args(scenario):
        args = scenario.split(separator)
        if len(args) > 1:
            return args[1:]
        return ''

    args = get_args(scenario)
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

    # return """
    # and
    # (
    #   []
    #   simple
    #   (
    #     [(%(etlo).0fet)(%(etlo).0fet)]
    #   )
    #   combgen
    #   (
    #     [(2)]
    #     dijet
    #     (
    #       [(%(masslo).0fmass, 26dphi)]
    #     ) 
    #     simple
    #     (
    #       [(10et)(20et)]
    #     )
    #   )
    # )""" % argvals





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
    print scenario
    print make_vbenf_label(scenario)
    print
    scenario = 'vbenf'
    print scenario, ' - note: no arguments'
    print make_vbenf_label(scenario)


if __name__ == '__main__':
    _test1()
