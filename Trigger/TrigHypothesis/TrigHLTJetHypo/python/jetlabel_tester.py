# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""Utility to test whether a string is a legal jet chain label"""


from ChainLabelParser import ChainLabelParser

def compile(label, dump=False):
    parser = ChainLabelParser(label, debug=False)
    tree = parser.parse()

    if dump: tree.dump()
    

if __name__ == '__main__':
    label = """
    agree([]
    simple([(80et)(81et)(82et)(83et)])

    partgen(
    []
    simple([(80et)(81et)])
    simple([(80et)(81et)]))
    )"""
    compile(label)
    
    label = """
    combgen(
    [(2)(20et, 0eta320)]
    
    simple([(40et, 0eta320) (50et, 0eta320)])
    simple([(35et, 0eta240) (55et, 0et240)])
    )"""

    #compile(label)
