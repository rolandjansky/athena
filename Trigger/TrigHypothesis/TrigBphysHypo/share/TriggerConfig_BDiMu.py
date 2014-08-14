include.block("TrigBphysHypo/TriggerConfig_BDiMu.py")


dirBDiMuChainCounter = {'BUpsiMU6mu'   : '1611' }

def TriggerConfig_BDiMu(TE, sliceId, trigId):

    try:
        counterId = dirBDiMuChainCounter[sliceId]
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
    # LVL1(6GeV)-->IdScan_FullScan-->TrigDiMuon-->DiMuHypo
    #################################

    if counterId is '1611':
       sequence = []
       if TriggerFlags.BphysicsSlice.doL2ID() and TriggerFlags.BphysicsSlice.doL2Muon():
           from TrigSteeringTest.TrigSteeringTestConf import  PESA__newDummyAlgoScan
           from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_FullScan, TrigIDSCAN_Bphysics
#           DummyAlgo = PESA__newDummyAlgoScan("DummyAlgo")
#           sequence += [DummyAlgo]
           sequence += [ TrigIDSCAN_Bphysics() ]
           from TrigDiMuon.TrigDiMuonConfig import *
           from TrigBphysHypo.TrigL2DiMuHypoConfig import  TrigL2DiMuHypo_Upsi
           sequence += [ TrigDiMuonConfig("test") ]
           sequence += [ (TrigL2DiMuHypo_Upsi) ]

           if len(sequence) is not 0:
               TE = triggerPythonConfig.addSequence(TE, sequence , "L2_"+ sliceId )
               l2chain.addHLTSignature(TE)
    
    triggerPythonConfig.addHLTChain(l2chain)
    #triggerPythonConfig.addHLTChain(efchain)
