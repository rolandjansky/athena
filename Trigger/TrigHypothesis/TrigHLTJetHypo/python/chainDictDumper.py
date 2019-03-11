from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
from TriggerMenuMT.HLTMenuConfig.Menu.SignatureDicts import JetChainParts
import sys

from pprint import PrettyPrinter

pp = PrettyPrinter(indent=4)
pp.pprint(JetChainParts)

decodeChainName = DictFromChainName.DictFromChainName()

def do_it():
    print '\n------------------\n'
    chain_dict = decodeChainName.getChainDict(chain_name)
    pp.pprint(chain_dict)

if __name__ == '__main__':
    chain_name = sys.argv[1]
    do_it()
    # chain_names = ('HLT_j85', 'j85', 'HLT_j55_0eta320_2j20_0eta320',)
    # for chain_name in chain_names:
    #      do_it()
