import re

# substrings that cannot occur in any chainPartName for simple chains.
reject_substr = (
    'gsc',
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


    # print cd
    # assert False
    chain_parts = [c for c in cd['chainParts'] if c['signature'] == 'Jet']
    chain_name = cd['chainName']
                       
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
            if smcstr:
                condition_str += ',%s)'
            else:
                condition_str += ')'
            label += condition_str
    label += '])'
    return label


def select_vbenf_chains(cd):
    """Select chains for which to make a vbenf chain label.
    Chains selected by reuiring that the signature os 'Jet'. Chains are
    vetoed if specific substrings occur in any of the chainPartNames"""


    # print cd
    # assert False
    chain_parts = [c for c in cd['chainParts'] if c['signature'] == 'Jet']
    return any(['vbenf' in cp['chainPartName'] for cp in chain_parts])

def make_vbenf_label(chain_dict):
    """Marshal information from the selected chainParts to create a
    vbenf label.
    """

    # toy label for developement: run simple and dijet independently.
    # simple makes Et cuts on two jets. Independently (sharing possible)
    # of jets choosean by simple,  the dijet
    # scenario requires a dijet of mass > 900, and opening angle in phi > 2.6
               
    return 'and([] simple([(50et)(70et)])dijet([(900mass, 26dphi)]))'

if __name__ == '__main__':
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
