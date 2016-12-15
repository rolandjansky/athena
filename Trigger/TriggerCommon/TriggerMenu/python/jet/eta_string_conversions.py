# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""convert standard string such as 0eta320 to floats eta_min, eta_max"""


import re
eta_range_re = re.compile(r'(?P<eta_min>\d{1,3})eta(?P<eta_max>\d{1,3})')

def _extract(eta_range):
    match = eta_range_re.search(eta_range)
    if not match:
        msg = '%s.process_part() unknown eta range: %s does not match %s'
        
        msg = msg % ('eta_string_conversions._extract()',
                     str(eta_range),
                     eta_range_re.pattern)
        raise RuntimeError(msg)

    return (match.group('eta_min'), match.group('eta_max'))


def signflip(a, b):
    ta = min(-a, -b)
    tb = max(-a, -b)
    return ta, tb

def signflipstr(a, b):
    fa = '-'+ b
    fb = '-'+ a
    
    return fa, fb

def eta_string_to_floats(eta_range):

    eta_min, eta_max = _extract(eta_range)
    eta_min = float(eta_min)/100.
    eta_max = float(eta_max)/100.
        
    if eta_min > eta_max:
        msg = '%s.process_part()  eta range inverted: [%s,%s]' % (
            'eta_string_conversions.eta_string_to_floatsx()',
            str(eta_min),
            str(eta_max)
        )
        raise RuntimeError(msg)

    if eta_range.startswith('n'):
        eta_min, eta_max = signflip(eta_min, eta_max)

    return eta_min, eta_max

def eta_string_to_ints(eta_range):

    eta_min, eta_max = _extract(eta_range)
    eta_min = int(eta_min)
    eta_max = int(eta_max)

    if eta_range.startswith('n'):
        eta_min, eta_max = signflip(eta_min, eta_max)

    return eta_min, eta_max

def eta_string_to_strings(eta_range):

    eta_min, eta_max = _extract(eta_range)
    if eta_range.startswith('n'):
        eta_min, eta_max =  signflipstr(eta_min, eta_max)
        
    return eta_min, eta_max
