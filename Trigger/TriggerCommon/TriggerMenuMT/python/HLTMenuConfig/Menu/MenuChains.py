#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

## chain name, HypoTool confname (for combined chains there is more than one)

ChainDictionary ={  'HLT_e8'           : ['HLT_e8'],
                    'HLT_e5'           : ['HLT_e5'],
                    'HLT_e20'          : ['HLT_e20'],
                    'HLT_g5'           : ['HLT_g5'],
                    'HLT_mu6'          : ['HLT_mu6'],
                    'HLT_mu6fast'      : ['HLT_mu6'],
                    'HLT_mu6Comb'      : ['HLT_mu6'],
                    'HLT_mu6msonly'    : ['HLT_mu6'],
                    'HLT_mu8'          : ['HLT_mu8'],
                    'HLT_mu81step'     : ['HLT_mu8'],
                    'HLT_mu20'         : ['HLT_mu20'],
                    'HLT_mu20_ivar'    : ['HLT_mu20'],
                    'HLT_2mu6'         : ['HLT_2mu6'], # review double object
                    'HLT_2mu6Comb'     : ['HLT_2mu6'], # review double object
                    'HLT_mu8_e8'       : ['HLT_mu8','HLT_e8'],
                    'HLT_e5_e8'        : ['HLT_e5', 'HLT_e8'],
                    'HLT_e3_etcut'     : ['HLT_e3_etcut'],
                    'HLT_e3_etcut1step': ['HLT_e3_etcut'],
                    'HLT_e5_etcut'     : ['HLT_e5_etcut'],
                    'HLT_e7_etcut'     : ['HLT_e7_etcut'],
                    'HLT_mu6_e3_etcut' : ['HLT_mu6', 'HLT_e3_etcut'],
                    'HLT_e3_etcut_mu6' : ['HLT_e3_etcut', 'HLT_mu6'],
                    'HLT_g5_etcut'     : ['HLT_g5_etcut'],
                    'HLT_e3_g5_etcut'  : ['HLT_e3_etcut', 'HLT_g5_etcut'],
                    'HLT_j85'          : ['HLT_j85'],
                    'HLT_j100'         : ['HLT_j100'],
                    'HLT_j35_gsc45_boffperf_split' : ['HLT_j35_gsc45_boffperf_split'],
                    'HLT_j35_gsc45_bmv2c1070_split' : ['HLT_j35_gsc45_bmv2c1070_split'],
                    'HLT_j35_gsc45_bmv2c1070' : ['HLT_j35_gsc45_bmv2c1070'],
                    'HLT_xe65_L1XE50': ['HLT_xe65_L1XE50'],
                    'HLT_xe30_L1XE10': ['HLT_xe30_L1XE10']
                 }



def getConfFromChainName(name):  
    if name in ChainDictionary:
        print "MenuChains.getConfFromChainName: Called chain "+name+" and hypoTool conf "+ str(ChainDictionary[name])
        return ChainDictionary[name]
    else:
        return [name]
    log.error("MenuChains.getConfFromChainName: Wrong chain name given: found %s",name)
    sys.exit("ERROR, in getConfFromChainName: Wrong chain configuration") 


    
