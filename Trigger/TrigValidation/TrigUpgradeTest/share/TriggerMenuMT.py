#-------------------------
# Imports
#-------------------------
# logging 
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger('TriggerMenuMT')

include("TrigUpgradeTest/DictFromChainName.py")

#-------------------------
# List of chains to run
#-------------------------
def triggerMenuList():
    # --- Input formate for now:
    # --- ['chainName',  'L1itemforchain']

    electrons =[
        ['HLT_e20', 'L1_EM15'],
        ['HLT_e30', 'L1_EM24VHI'],
        ['HLT_e30_ringer', 'L1_EM24VHI'],
        ]

    photons = [ 
        ['HLT_g100', 'L1_EM24VHI'],
        ['HLT_2g50', 'L1_2EM15VH' ],
        ]

    muons =[
        ['HLT_mu20', 'L1_MU20' ],
        ['HLT_mu8', 'L1_MU6'],
        ['HLT_2mu8', 'L1_2MU4'],
        ]
    
    allchains = electrons + photons + muons
    log.info('List of all chains to run: %s', allchains)
    return allchains


#-------------------------
# GenerateMenu class:
# assembels it all
#-------------------------
class GenerateMenu:

    def generate(self):
        log.info('GenerateMenu.generate')
        
        # get names of chains to be generated
        chainsInMenu = triggerMenuList()
        log.info('chainsInMenu %s', chainsInMenu)
        
        # get chain dict for each chain
        myChainDict = DictFromChainName()
    
        for chain in chainsInMenu:
            log.info('Processing chain:  %s', chain)
            chainDicts = myChainDict.getChainDict(chain)
        
        import pprint
        pp = pprint.PrettyPrinter(indent=4, depth=8)
        log.info('FINAL dictionary: %s', pp.pformat(chainDicts))
                
        # use info from dictionary to send it to the right signature
                    
        # in the relevant signature, use the info from dict to assemble the correct 




    






    
