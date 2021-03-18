# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""Utility to test whether a string is a legal jet chain label"""
from __future__ import print_function


from ChainLabelParser import ChainLabelParser

def compile(label, do_dump=False, do_print=False):
    print ('compile:',  label) # noqa: ATL901

    parser = ChainLabelParser(label, debug=False)
    forest = parser.parse()

    print ('forest has %d' % len(forest),' tree(s)\n') # noqa: ATL901
    
    for i, tree in enumerate(forest):

        print ('tree ', i, '\n') # noqa: ATL901

        tree.set_ids(node_id=0, parent_id=0)
    
    
        print ('compile: tree.scenario', tree.scenario) # noqa: ATL901

        
        
        if do_print:
            print ('\nnode dumping top node only:\n') # noqa: ATL901
            print (tree) # noqa: ATL901

        if do_dump:
            print ('\nnode dump tree:\n') # noqa: ATL901
            print (tree.dump()) # noqa: ATL901
        
    return forest

def compile_(label, setter=None, expand=True, do_dump=False, do_print=False):
    compile(label, setter, expand, do_dump)

            
if __name__ == '__main__':


    import sys
    
    from TrigHLTJetHypo.test_cases import test_strings
    c = sys.argv[1]
    index = -1
    try:
        index = int(c)
    except Exception:
        print('expected int in [1,%d] ]on comand line, got %s' % (  # noqa: ATL901
            len(test_strings), c))
        sys.exit()

    if(index < 0 or index > len(test_strings) - 1):
        print('index %d not in range [0, %d]' % (index, len(test_strings) -1))  # noqa: ATL901
        sys.exit()
                                                 
            
    print('index', index)  # noqa: ATL901
    label = test_strings[index]

    tree = compile(label, do_dump=True)

