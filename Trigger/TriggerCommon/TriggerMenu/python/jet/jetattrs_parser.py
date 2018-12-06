""" 7 May 2018 parse string defining the jet attributes hypo cuts,
and return lists of strings suitable for initializing the hypo.
If low or high values mising from menu string, attempt to fill in
default values from dictionary. Raise an exception if this fails. 
"""


import re
import sys


def parse(s, vals):
    tpat = re.compile(r'^(?P<low>\d*)(?P<var>[a-zA-Z]+)(?P<high>\d*)$')

    sep0 = 'II'
    sep1 = 'I'

    jets_tokens = s.split(sep0)
    j_dicts = []

    known_defaults = {
    'ktdr' : {'low' : '0', 'high' : '20'}, #scaled by 0.1 in interpreter
    'width' : {'low' : '0', 'high' : '10'}, #scaled by 0.1 in interpreter
    }


    for j in jets_tokens:
        d = {}
        assert j.startswith('subj')
        j_toks = j.split(sep1)
        assert len(j) > 1
        d['E'] = int(j_toks[0][len('subj'):])
        for t in j_toks[1:]:
            m = tpat.match(t)
            dd = m.groupdict()

            if not dd['low']:
                dd['low'] = known_defaults.get(dd['var'], {}).get('low',{})
            if not dd['high']: 
                dd['high'] = known_defaults.get(dd['var'], {}).get('high',{})
            d[dd.pop('var')] = dd
        j_dicts.append(d)


    all_moments = set()
    for d in j_dicts:
        for k in d.keys():all_moments.add(k)
        
    all_moments.remove('E')
    v_E = [d['E'] for d in j_dicts]
    v_var = list(all_moments)

    v_has = []
    v_low = []
    v_high = []
    for d in j_dicts:
        for v in v_var:
            if v in d:
                v_has.append('yes')
                v_low.append(d[v]['low'])
                v_high.append(d[v]['high'])
            else:
                v_has.append('no')
                v_low.append('-')
                v_high.append('-')

    vals.update({'momentstr': s,
                 'E':v_E,
                 'jetVars':v_var,
                 'has':v_has,
                 'limit_mins': v_low,
                 'limit_maxs':v_high})

if __name__ == '__main__':

    for s in ('subj360Iktdr2I3width6IIsubj300Iwidth',
              'subj360Iktdr3IIsubj300Iwidth2',
              'subj360Iktdr',
              'subj360I0width5',
              'subj360IXXX', 'XXX'):
        args = {}
        try:
            parse(s, args)
            print '\n-------------------\n'
            print 's          ', args['momentstr']
            print 'E          ', args['E']
            print 'moments:   ', args['jetVars']
            print 'has:       ', args['has']
            print 'low vals:  ', args['limit_mins']
            print 'high vals: ', args['limit_maxs']
        except:
            print '\n-------------------\n'
            print 's          ', s
            print 'parse error'           
    
