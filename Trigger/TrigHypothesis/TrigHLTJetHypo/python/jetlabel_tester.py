# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""Utility to test whether a string is a legal jet chain label"""


from ChainLabelParser import ChainLabelParser
from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander

def compile(label, expand=False, dump=False):
    parser = ChainLabelParser(label, debug=False)
    tree = parser.parse()

    if expand:
        visitor = TreeParameterExpander()
        tree.accept(visitor)
        print visitor.report()
        
    if dump:
        print tree.dump()

def compile_(label, expand=True, dump=True):
    compile(label, expand, dump)
            
if __name__ == '__main__':

    label = 'simple([(80et)(81et)(82et)(83et)(maxshare)])'
        
    label = """
    agree([]
    simple([(80et)(81et)(82et)(83et)])

    partgen(
    []
    simple([(80et)(81et)])
    simple([(80et)(81et)]))
    )"""
    
    label = """
    combgen(
    [(2)(20et, 0eta320)]
    
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
        [(2)(10et)]
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
    compile(label, expand=True)
