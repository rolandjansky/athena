# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Converts a tree dictionary with structure:
{...
parent: [child0,, child1...]
...
}

to 
{...
parent: [descendant0, descendant1...]
...
}
"""


def combine(to, fro):
    lto = len(to)
    # print 'comnine0:', to, fro
    [to.append(ffro) for ffro in fro if ffro not in to]
    # print 'comnine1:', to
    return len(to)>lto
    
def link(dict):
    dead = []
    dlen = len(dict)
    npass = 0
    while dlen>len(dead)+1:
        for key in dict.keys():
            if key in dead: continue
            for ent in dict[key]:
                if not combine(dict[key],dict[ent]):
                    if ent not in dead: dead.append(ent)
    
        # print 'pass: %d ' % npass
        # print 'dead: %s ' % dead
        # print 'dict: %s'  % d
        # npass += 1

    

if __name__ == '__main__':
    d={
        'a':['b', 'c'],
        'b':['d', 'e'],
        'c':['f', 'g'],
        'e':['j', 'k'],
        'f':['l', 'm'],
        'g':['n', 'o'],
        'h':[],
        'i':[],
        'j':[],
        'k':[],
        'l':[],
        'm':[],
        'n':[],
        'o':[],
        'd':['h', 'i']
        }

    link(d)
    [d[ent].sort() for ent in d.keys()]
    print d
    assert d['a'] == ['b','c','d','e','f','g','h','i','j','k','l','m','n','o']
    assert d['b'] == ['d','e','h','i','j','k']
    assert d['c'] == ['f','g','l','m','n','o']
    assert d['d'] == ['h','i']
    assert d['e'] == ['j','k']
    assert d['f'] == ['l','m']
    assert d['g'] == ['n','o']
    assert d['h'] == []
    assert d['i'] == []
    assert d['j'] == []
    assert d['k'] == []
    assert d['l'] == []
    assert d['m'] == []
    assert d['n'] == []
    assert d['o'] == []
    

