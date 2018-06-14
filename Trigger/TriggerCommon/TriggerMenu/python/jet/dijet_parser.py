""" 2/2/2018 this module is currently not used due to
    objection to chain name conventions by th memu group,
    hopefully tmporarily. For now use dijet_parser2.

    Decode the incoming string with dijet hypo parameters. The string
    will have variable names and optional low and max limits. If a limit
    is missing, use a default value. If the default value is +infinity,
    set to a non-physical value that can be converted to a float.
    For some variables, -1 fits the needs. This will be converted
    to the max C++ double in the C++ Algorithm.

    Convert the values to floats, taking into account the units (eg
    deta = 120 -> 1.2)
    """


import re, sys


dijet_re = re.compile(r'^(?P<min>\d{0,3})(?P<var>[a-z]+)(?P<max>\d{0,3}$)')

def _dj_parse(s):
    """Parse the incoming string to find the variable names and the optional
    low and max limits. """
    
    error =  False
    tokens = s.split('Z')
    result = {}
    for t in tokens:
        m = dijet_re.match(t)
        if m is None:
            error = True
            break
        d = m.groupdict()
        result[d.pop('var')] = d
        
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
                    'deta': 0.01,
                    'dphi': 0.01}
    
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

    assert s.startswith('dj')
    s = s[2:]
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
        'dj80aetZ120betZ100m150Z50detaZdphi40ZZ90aet120Z120bet240Z100m150Z50detaZdphi40',
        'dj10aet20',
        'djaeta20',
        'dj10bet20',
        'djbeta20',
        'djaet100Z320aeta450Zdeta10Z150m'
    )

    bad = ('dj10aet2000', # too many digits
           'dj10Xet20', # bad variable
    )

    n = 0
    errs = n
    for i in good:
        n += 1
        args = {}
        err = dijet_parser(i, args)
        if err:
            print 'error, expext ok', i
            errs += 1
        else:
            print 'ok, expect ok   ', i

    print 'tests : %d expected errors  %d, obtained errors %d' % (n,
                                                                 errs,
                                                                 0)

    n = 0
    errs = 0
    exp_errs = 0 
    for i in bad:
        n += 1
        args = {}
        err = dijet_parser(i, args)
        if err:
            errs += 1
            print 'error expect error ', i
        else:
            print 'ok, expect error   ', i


    print 'tests : %d expected errors  %d, obtained errors %d' % (n,
                                                                 errs,
                                                                 n)
    

    s = 'dj30aetZ50betZ900mZdphi260'
    args = {}
    err =  dijet_parser(s, args)
    if err:
        print 'Error parsing', s
        sys.exit(0)

    wid = max(len(k) for k in args.keys())
    for k in sorted(args.keys()):
        print (k.ljust(wid), args[k])
            # else:
        #    print i.split('?')
        #    for k in sorted(args.keys()):
        #        print k
        #        print ' ', args[k]
#        tokens = i.split('?')
#        for t in tokens:
#            print t
#            error, result = dj_decode(t)
#            if error:
#                print '   ERROR'
#            else:
#                for j, k in result.items():
#                    print j, k
