#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# More detailed syntax checking for han files

from __future__ import print_function

import lark, sys
from AthenaCommon.Utils.unixtools import find_datafile

class T(lark.Transformer):
    def __default_token__(self,tok):
        print(tok)
        return tok
    def referenceblock(self, tok):
        locations = [_ for _ in tok if isinstance(_, lark.Tree) and _.data == 'location']
        if len(locations) > 1:
            raise ValueError(f'More than one location given for reference {tok[0].children[0]}')
        if len(locations) > 0:
            locs = locations[0].children[0].split(',')
        else:
            locs = [_.children[0] for _ in tok if isinstance(_, lark.Tree) and _.data == 'file']
        afspaths = [_ for _ in locs if _.startswith('/afs')]
        if any(afspaths):
            raise ValueError('A reference location for a production han configuration is given with an AFS path\n'
            f'Offending path is {",".join(afspaths)} of reference {tok[0].children[0]}')
        eospaths = [_ for _ in locs if _.startswith('/eos')]
        xrootdpaths = [_ for _ in locs if _.startswith('root://')]
        if len(eospaths) !=  len(xrootdpaths):
            raise ValueError(f'Backup xrootd locations must be given for references with EOS paths for reference {tok[0].children[0]}\n'
            f'Given EOS paths are {",".join(eospaths)}\n'
            f'Given xrootd paths are {",".join(xrootdpaths)}')
        return tok
    pass

grammarfile = find_datafile('DataQualityInterfaces/han_def.lark')
if grammarfile is None:
    raise OSError(f'Unable to find file parser configuration file')
grammar = open(grammarfile).read()
parser = lark.Lark(grammar, parser='lalr', lexer='contextual', transformer=T())

infile = open(sys.argv[1]).read()
try:
    tree = parser.parse(infile)
    print('Tree done')
    print(tree.pretty())
except Exception as e:
    print('ERROR:')
    print(e)
    raise e
    sys.exit(1)
