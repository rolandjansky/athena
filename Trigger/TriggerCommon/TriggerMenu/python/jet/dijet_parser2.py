""" 2/2/2018 this module currently replaces di_jetparser, due to
    objection to chain name conventions by th memu group,
    hopefully tmporarily.

    Decode the incoming string with dijet hypo parameters. The string
    will have variable names and optional low and max limits. If a limit
    is missing, use a default value. If the default value is +infinity,
    set to a non-physical value that can be converted to a float.
    For some variables, -1 fits the needs. This will be converted
    to the max C++ double in the C++ Algorithm.

    Convert the values to floats, taking into account the units (eg
    deta = 120 -> 1.2)
    """


import re



dijet_re = re.compile(r'^invm(?P<massmin>\d{0,3})j(?P<etmin>\d{0,3})(dPhi(?P<dphimax>\d{0,2}))?(dEta(?P<detamin>\d{0,2}))?$$')

def _dj_parse(s):
    """Parse the incoming string to find the variable names and the optional
    low and max limits. The code is this function is artificial - it intentionally
    remains close to th code in dijet_parser.py"""
    
    error =  False
    tokens = s.split('Z')
    assert len(tokens) == 1 # dijet_parser2 only
    result = {}
    for t in tokens: # dijet_parser2: loop over 1 entry
        m = dijet_re.match(t)
        error = True
        if m is None: return(error, {})

        d = m.groupdict()

        key_map = {
            'm': ('massmin', 'min'),
            'aet': ('etmin', 'min'),
            'bet': ('etmin', 'min'),
            'dphi': ('dphimax','max'),
            'deta': ('detamin', 'min'),
        }

        for k, v in key_map.items():
            if d[v[0]] is not None: result[k] = {v[1]: d[v[0]]}

    error = False
    return (error, result)

def _dj_decode_one(s):
    
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
                    'deta': 0.1,
                    'dphi': 0.1}
    
    assert unit_factors.keys() == result.keys()
    
    for k, v in result.items():
        uf = unit_factors[k]  # result and unit_factors must have the same jk
        if uf is None:
            m='dijet_parser: unknown variable: %s not in %s' (
                str(k),
                str(unit_factors.keys()
                )
            )
            raise RuntimeError(m)
        for kk in v:
            v[kk] = float(v[kk])
            if v[kk] > 0: v[kk] = uf * v[kk]

    return (False, result)

def dijet_parser(s, args):
    """The incoming string contains cut data for 1 or more dijets.
    Split and process each dijet. Repack results by variable.
    """

    assert s.startswith('invm')

    dijet_params = [_dj_decode_one(t) for t in s.split('ZZ')]
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

    return False
if __name__ == '__main__':
    good = (
        'invm900j50dPhi24',
        'invm900j50dPhi24dEta100',
        'invm900j0dPhi24dEta100',
    )

    bad = (
        'invm900',  # run1 hypo spec
        'invm9000j50dPhi240', # too many digits
        'invm900j50dEta240dPhi240', # wrong order
    )

    print
    n = 0
    errs = n
    for i in good:
        n += 1
        args = {}
        err = dijet_parser(i, args)
        for k in sorted(args.keys()): print k, args[k]
        if err:
            print 'error, but expected ok', i
            errs += 1
        else:
            print 'ok, and expect ok   ', i

    print 'no of tests: %d. Expected errors  %d. Obtained errors %d' % (n,
                                                                        0,
                                                                        errs)



    print
    n = 0
    errs = 0
    exp_errs = 0 
    for i in bad:
        n += 1
        args = {}
        err = dijet_parser(i, args)
        if err:
            errs += 1
            print 'error, and expected error ', i
        else:
            print 'ok, but expected  error   ', i


    print 'no of tests: %d. Expected errors  %d. Obtained errors %d' % (n,
                                                                        n,
                                                                        errs)

    

