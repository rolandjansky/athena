# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""Utility to test whether a string is a legal jet chain label"""
from __future__ import print_function


from ChainLabelParser import ChainLabelParser
from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander
from  TrigHLTJetHypo.ConditionsToolSetterTree import ConditionsToolSetterTree
# from  TrigHLTJetHypo.ToolSetter import ToolSetter


def compile(label, setter=None, expand=False, do_dump=False, do_print=False):
    print ('compile:',  label)

    parser = ChainLabelParser(label, debug=False)
    tree = parser.parse()

    tree.set_ids(node_id=0, parent_id=0)
    
    
    if expand:
        visitor = TreeParameterExpander()
        tree.accept(visitor)

    print ('compile: tree.scenario', tree.scenario)
    if setter is not None:
        tree.accept(setter)
        
    if do_print:
        print ('\nnode dumping top node only:\n')
        print (tree)

    if do_dump:
        print ('\nnode dump tree:\n')
        print (tree.dump())
        
    return tree

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
        print('expected int in [1,%d] ]on comand line, got %s' % (
            len(test_strings), c))
        sys.exit()

    if(index < 0 or index > len(test_strings) - 1):
        print('index %d not in range [0, %d]' % (index, len(test_strings) -1))
        sys.exit()
                                                 
            
    print('index', index)
    label = test_strings[index]

    # tree = compile(label, dump=True)
    
    setter = ConditionsToolSetterTree('toolSetter')
    
    tree = compile(label, setter=setter,  expand=True, do_dump=True)

    sys.exit(0)

    label = 'simple([(80et)(81et)(82et)(83et)(maxshare)])'
        
    label = """
    agree([]
    simple([(80et)(81et)(82et)(83et)])

    partgen(
    []
    simple([(80et)(8z1et)])
    simple([(80et)(81et)]))
    )"""
    
    label = """
    combgen(
    [(20et, 0eta320)]
    
    simple([(40et, 0eta320) (50et, 0eta320)])
    simple([(35et, 0eta240) (55et, 0et240)])
    )"""


    label = """
        and
    (
      []
      simple
      (
        [(30et)(30et)]
      )
      combgen
      (
        [(10et)]

        dijet
        (
          [(34mass, 26dphi)]
        ) 
        simple
        (
          [(10et)(20et)]
        )
      )
    )"""

    label =  """agree([]
                         flownetwork([(130mass)(131mass)(10et)(11et)(12et)(13et)(0001122tree)])
                         partgen([] 
                            and([]
                                dijet([(130mass)]) simple([(10et)(11et)])
                            )
                           
                            and([]
                                dijet([(131mass)]) simple([(12et)(13et)])
                            )
                         )
        )"""

    label = """agree([]
               flownetwork([(130mass)(131mass)(10et)(11et)(0011par)])
               partgen([] 
               and([]
                   dijet([(130mass)]) simple([(10et)])
                  )
               )
        )"""

    label =  """simple
        (
          [(10et)(20et)]
        )"""

    label =  """partgen([] 
                       simple([(10et)(20et)])
                       simple([(12et)(22et)])
                )"""
    label = """and([]
                   combgen([]
                                simple([(10et)(20et)])
                                simple([(12et)(22et)])
                           )
                   and([]
                          simple([(32et)(42et)])
                          simple([(52et)(62et)])
                       )
                  )"""

#   label = """and([]
#                   combgen([]
#                           simple([(10et)(20et)])
#                           simple([(12et)(22et)])
#                          )
#                   partgen([]
#                       simple([(32et)(42et)])
#                       simple([(52et)(62et)])
#                       )
#                   )"""

    label = """noshare([] a([]) b([]))"""
    label = """ simple([(40et, 0eta320) (50et, 0eta320)])"""

    label = """z([] simple([(10et)])
                    simple([(20et)]))"""

    label = """simple([(10et, 0eta320)(20et)])"""

    # tree = compile(label, dump=True)
    
    setter = ConditionsToolSetterTree('toolSetter')

    tree = compile(label, setter=setter,  expand=True, dump=True)
    print ('tree scenario:', tree.scenario)
    # setter = ConditionsToolSetter('conditionsToolSetter')
    # setter.mod(tree)
    # print setter
