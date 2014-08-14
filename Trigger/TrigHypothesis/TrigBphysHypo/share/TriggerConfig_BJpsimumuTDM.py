include.block("TrigMuonHypo/TriggerConfig_BJpsimumuTDM.py")


dirJpsimumuChainCounter = {'BJpsiMU6mu'   : '1605',
                           'BJpsimu6mu'   : '1606' }

def TriggerConfig_BJpsimumuTDM(TE, sliceId, trigId):

    try:
        counterId = dirJpsimumuChainCounter[sliceId]
    except LookupError:
        print 'slice Id = ', sliceId
        raise RuntimeError, 'ERROR :  no chain number defined!'
    
    # defining the L2 chain for signatures
    l2chain = HLTChain( chain_name=sliceId+"_L2",
                        chain_counter=counterId,
                        lower_chain_name=trigId,
                        level="L2",
                        prescale='1',
                        pass_through='0')

    l2chain.addTriggerTypeBit(counterId)
    l2chain.addStreamTag('bphysics', prescale='1')	
        
    #################################
    # LVL1(6GeV)-->IdScan_Bphysics-->TrigDiMuon-->JpsiHypo
    #################################

    if counterId is '1605':
       sequence = []
       if TriggerFlags.BphysicsSlice.doL2ID() and TriggerFlags.BphysicsSlice.doL2Muon():
           from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Bphysics
           sequence += [ TrigIDSCAN_Bphysics() ]
           from TrigDiMuon.TrigDiMuonConfig import *
           from TrigBphysHypo.JpsiHypoConfig import JpsiHypo_1
           sequence += [ TrigDiMuonConfig("test") ]
           sequence += [ JpsiHypo_1() ]

           if len(sequence) is not 0:
               TE = triggerPythonConfig.addSequence(TE, sequence , "L2_"+ sliceId )
               l2chain.addHLTSignature(TE)
    else:
    
        #################################
        # LVL1(6GeV)--> muFast--> muFastHypo(5GeV)-->IdScan_Muon-->MuComb-->MuCombFast(5GeV)-->IdScan_Bphysics-->TrigDiMuon-->JpsiHypo
        #################################
    
        from TrigMuonHypo.TrigMuonHypoConfig import *

        sequence = []
    
        if TriggerFlags.BphysicsSlice.doL2Muon():
            from TrigmuFast.TrigmuFastConfig import *
            sequence += [ TrigmuFastConfig("Muon") ]
            sequence += [ MufastHypoConfig("Muon","5GeV") ]
            
            TE = triggerPythonConfig.addSequence(TE, sequence , 'T2muFastMuon_'+sliceId )
            l2chain.addHLTSignature(TE)
    
        sequence = []
    
        if TriggerFlags.BphysicsSlice.doL2ID():
            from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Muon
            sequence += [ TrigIDSCAN_Muon() ]
            from TrigmuComb.TrigmuCombConfig import *
            sequence += [ TrigmuCombConfig("Muon") ]
            sequence += [ MucombHypoConfig("Muon","5GeV") ]

            TE = triggerPythonConfig.addSequence(TE,sequence,"T2muFastComb_"+sliceId)
            l2chain.addHLTSignature(TE)
        
        sequence = []
    
        if TriggerFlags.BphysicsSlice.doL2Muon() and TriggerFlags.BphysicsSlice.doL2ID():
            from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Bphysics
            sequence += [ TrigIDSCAN_Bphysics() ]
            from TrigDiMuon.TrigDiMuonConfig import *
            from TrigBphysHypo.JpsiHypoConfig import JpsiHypo_1
            sequence += [ TrigDiMuonConfig("test") ]
            sequence += [ JpsiHypo_1() ]

            if len(sequence) is not 0:
                TE = triggerPythonConfig.addSequence(TE, sequence , "L2_"+ sliceId )
                l2chain.addHLTSignature(TE) 
    
    triggerPythonConfig.addHLTChain(l2chain)
    #triggerPythonConfig.addHLTChain(efchain)
