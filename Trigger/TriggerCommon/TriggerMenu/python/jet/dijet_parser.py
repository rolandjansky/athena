import re


cut_re = re.compile(r'^(?P<min>\d*)(?P<var>[a-z]+)(?P<max>\d*$)')

def _dj_parse(s):
    """Parse the incoming string to find the variable names and the optional
    low and max limits. """
    
    error =  False
    tokens = s.split('!')
    result = {}
    for t in tokens:
        m = cut_re.match(t)
        if m is None:
            error = True
            break
        d = m.groupdict()
        result[d.pop('var')] = d
        
    return (error, result)

def _dj_decode_one(s):
    """Decode the incoming string with dijet hypo parameters. The string
    will have variable anmes an optional low and max limits. If a limit
    is missing, use a default value. If the default value is +infinity,
    set to a non-physical value that can be converted to a float.
    For some variables, -1 fits the needs.

    Convert the values to floats, taking into account the units (eg
    deta = 120 -> 1.2)
    """
    
    result = {
        'aet': {'min': '0', 'max': '-1'},
        'bet': {'min': '0', 'max': '-1'},
        'aeta': {'min': '0', 'max': '-1'},
        'beta': {'min': '0', 'max': '-1'},
        'm': {'min': '0', 'max': '-1'},
        'deta': {'min': '0', 'max': '-1'},
        'dphi': {'min': '0', 'max': '-1'},
    }

    error, d = _dj_parse(s)
    if error: return (error, {})

    for dd in d.values():
        for k, v in dd.items():
            if not v: dd.pop(k)
    
    for k in d: result[k].update(d[k])

    unit_factors = {'aet': 1.,
                    'bet': 1.,
                    'aeta': 0.01,
                    'beta': 0.01,
                    'm': 1.,
                    'deta': 0.01,
                    'dphi': 0.01}
    
    for k, v in result.items():
        uf = unit_factors[k]
        for kk in v:
            v[kk] = float(v[kk])
            if v[kk] > 0: v[kk] = uf * v[kk]

    return (False, result)

def dijet_parser(s, args):
    """The incoming string contains cut data for 1 or more dijets.
    Split and process each dijet. Repack results by variable.
    """

    assert s.startswith('dj')
    s = s[2:]
    dijet_params = [_dj_decode_one(t) for t in s.split('?')]
    errors = [e[0] for e in dijet_params]
    if any(errors): return True
    
    dijet_params = [e[1] for e in dijet_params]

    args['aet_mins'] = [d['aet']['min'] for d in dijet_params]
    args['aet_maxs'] = [d['aet']['max'] for d in dijet_params]

    args['aeta_mins'] = [d['aeta']['min'] for d in dijet_params]
    args['aeta_maxs'] = [d['aeta']['max'] for d in dijet_params]

    args['bet_mins'] = [d['bet']['min'] for d in dijet_params]
    args['bet_maxs'] = [d['bet']['max'] for d in dijet_params]

    args['beta_mins'] = [d['beta']['min'] for d in dijet_params]
    args['beta_maxs'] = [d['beta']['max'] for d in dijet_params]

    args['m_mins'] = [d['m']['min'] for d in dijet_params]
    args['m_maxs'] = [d['m']['max'] for d in dijet_params]
    

    args['deta_mins'] = [d['deta']['min'] for d in dijet_params]
    args['deta_maxs'] = [d['deta']['max'] for d in dijet_params]


    args['dphi_mins'] = [d['dphi']['min'] for d in dijet_params]
    args['dphi_maxs'] = [d['dphi']['max'] for d in dijet_params]

if __name__ == '__main__':
    good = (
        '80aet!120bet!100m150!50deta!dphi40?90aet120!120bet240!100m150!50deta!dphi40',
    )

    
    for i in good:
        args = {}
        err = dijet_parser(i, args)
        if err:
            print err
        else:
            print i.split('?')
            for k in sorted(args.keys()):
                print k
                print ' ', args[k]
#        tokens = i.split('?')
#        for t in tokens:
#            print t
#            error, result = dj_decode(t)
#            if error:
#                print '   ERROR'
#            else:
#                for j, k in result.items():
#                    print j, k
