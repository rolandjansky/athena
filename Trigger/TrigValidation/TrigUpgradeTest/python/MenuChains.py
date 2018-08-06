## chain name, HypoTool confname (for combined chains there is more than one)

ChainDictionary ={  'HLT_e8'     : ['HLT_e8'],
                    'HLT_e20'    : ['HLT_e20'],
                    'HLT_mu6'    : ['HLT_mu6'],
                    'HLT_mu6Comb' : ['HLT_mu6'],
                    'HLT_mu6fast' : ['HLT_mu6'],
                    'HLT_mu8'    : ['HLT_mu8'],
                    'HLT_mu81step': ['HLT_mu8'],
                    'HLT_mu20'   : ['HLT_mu20'],
                    'HLT_2mu6'   : ['HLT_2mu6'], # review double object
                    'HLT_2mu6Comb'   : ['HLT_2mu6'], # review double object
                    'HLT_mu8_e8' : ['HLT_mu8','HLT_e8'],                    
                    'HLT_e3_etcut': ['HLT_e3_etcut'],
                    'HLT_e3_etcut1step': ['HLT_e3_etcut'],
                    'HLT_e5_etcut': ['HLT_e5_etcut'],
                    'HLT_e7_etcut': ['HLT_e7_etcut'],
                    'HLT_mu6_e3_etcut': ['HLT_mu6', 'HLT_e3_etcut'],
                    'HLT_g5_etcut' : ['HLT_g5_etcut']
                 }



def getConfFromChainName(name):  
    if name in ChainDictionary:
        print "called chain "+name+" and hypoTool conf "+ str(ChainDictionary[name])
        return ChainDictionary[name]
    log.error("Wrong chain name given: found %s",name)
    sys.exit("ERROR, in chain configuration") 


    
