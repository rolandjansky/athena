include.block("TrigBphysHypo/TriggerConfig_BmumuX.py")

def TriggerConfig_BmumuX(TE, sliceID):

    l2chain = HLTChain( chain_name=sliceID+"_L2", chain_counter="1608", lower_chain_name="L1_MU06", level="L2", prescale="1", pass_through="0")
    l2chain.addTriggerTypeBit('9')
    l2chain.addStreamTag('bphysics', prescale='1')
    triggerPythonConfig.addHLTChain(l2chain)

#    l2chain.addHLTSignature( ['T2muFastMuon_mu6l', 'T2muFastMuon_mu6l'] )
    l2chain.addHLTSignature( ['L2_mu6l', 'L2_mu6l'] )
    l2chain.addHLTSignature( [TE, TE] )
    
    sequence = []
    
#    include ("TrigBphysHypo/TriggerConfig_TrigL2BMuMuHypo.py")
#    trigL2Bmumuhypo=TriggerConfig_TrigL2BMuMuHypo("L2","B0") 
    
    from TrigBphysHypo.TrigL2BMuMuXFexConfig import L2BMuMuXFex_1
    t2BMuMuXFex = L2BMuMuXFex_1()
    sequence += [ t2BMuMuXFex ]

    from TrigBphysHypo.TrigL2BMuMuXHypoConfig import L2BMuMuXHypo_1
    t2BMuMuXHypo = L2BMuMuXHypo_1()
    sequence += [ t2BMuMuXHypo ]
    
#    sequence +=  [trigL2Bmumuhypo.classAndInstanceName()[0]]
    if len(sequence) is not 0:    
        TE = triggerPythonConfig.addSequence( [TE, TE], [ t2BMuMuXFex, t2BMuMuXHypo ], sliceID)
#        TE = triggerPythonConfig.addSequence( [TE, TE], appendLevel(sequence, "L2"), sliceID)
        l2chain.addHLTSignature(TE)
        
        
