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

    return eta_min, eta_max

def eta_string_to_ints(eta_range):

    eta_min, eta_max = _extract(eta_range)
    return int(eta_min), eta_max

def eta_string_to_strings(eta_range):

    return _extract(eta_range)

