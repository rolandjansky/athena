# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
from TriggerMenuMT.HLTMenuConfig.Menu.SignatureDicts import JetChainParts
import sys

from pprint import PrettyPrinter

pp = PrettyPrinter(indent=4)
pp.pprint(JetChainParts)

def do_it():
    print('\n------------------\n') # noqa: ATL901
    chain_dict = dictFromChainName(chain_name)
    pp.pprint(chain_dict)

if __name__ == '__main__':
    chain_name = sys.argv[1]
    do_it()
    # chain_names = ('HLT_j85', 'j85', 'HLT_j55_0eta320_2j20_0eta320',)
    # for chain_name in chain_names:
    #      do_it()
