include.block("TrigMuonHypo/TriggerConfig_singleMuon.py")


#                                  muFast        muComb       TrigMoore
#                                  Th cut        Th cut        Th cut
dirMuonThresholds = { 'mu0'   : [  '2GeV' ,      '2GeV' ,      '2GeV' ] ,
                      'mu5'   : [  '5GeV' ,      '5GeV' ,      '5GeV' ] ,
                      'mu6l'  : [  '2GeV' ,      '2GeV' ,      '2GeV' ] ,
                      'mu6'   : [  '6GeV' ,      '6GeV' ,      '6GeV' ] ,
                      'mu20i' : [  '20GeV',      '20GeV',      '20GeV'] }


dirMuonChainCounter = {'mu0'    : '832',
                       'mu5'    : '833',
                       'mu6l'   : '834',
                       'mu6'    : '835',
                       'mu20i'  : '836',
		       'stau'   : '837',
                       'tileROD': '838',
                       'tileMU' : '839'}


# all import statament are moved at the module level

from TrigMuonHypo.TrigMuonHypoConfig import *
from TrigmuFast.TrigmuFastConfig import *
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Muon
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Muon
from TrigmuComb.TrigmuCombConfig import *
from TrigmuIso.TrigmuIsoConfig import *
from InDetTrigRecExample.EFInDetConfig import *
from TrigMoore.TrigMooreConfig import *
from TrigTileMuId.TrigTileRODMuAlgConfig import TrigTileRODMuAlg_L2
from TrigTileMuId.TrigTileLookForMuAlgConfig import TrigTileLookForMuAlg_L2


def TriggerConfig_singleMuon(TE , sliceId , trigId, lvl2Conf ):

    try:
        counterId = dirMuonChainCounter[sliceId]
    except LookupError:
        print 'slice Id = ', sliceId
        raise RuntimeError, 'ERROR :  no chain number defined!'

    try:
        thList = dirMuonThresholds[sliceId]
    except LookupError:
        print 'slice Id = ', sliceId
        raise RuntimeError, 'ERROR :  no threshold value defined!'



    # defining the L2 chain for signatures
    l2chain = HLTChain( chain_name=sliceId+"_L2", 
                        chain_counter=counterId, 
                        lower_chain_name=trigId,
                        level="L2", 
                        prescale='1',
                        pass_through='0')
    l2chain.addTriggerTypeBit(4)
    l2chain.addStreamTag('muons', prescale='1')


    # defining the EF chain for signatures
    efchain = HLTChain( chain_name=sliceId+"_EF", 
                        chain_counter=counterId,
                        lower_chain_name=sliceId+"_L2",
                        level="EF", 
                        prescale='1', 
                        pass_through='0')
    efchain.addTriggerTypeBit(4)
    efchain.addStreamTag('muons', prescale='1')



    ##########################################
    # Starting configuring the L2 signatures #
    ##########################################

    # muFast
    ##########
    sequence = []
    if TriggerFlags.MuonSlice.doL2Muon():

        sequence += [ TrigmuFastConfig(lvl2Conf) ]
        sequence += [ MufastHypoConfig(lvl2Conf,thList[0]) ]
	
        if TriggerFlags.MuonSlice.doL2ID() or TriggerFlags.MuonSlice.doL2Calo():
            TE = triggerPythonConfig.addSequence(TE, sequence , 'T2muFastMuon_'+sliceId )
        else:
            TE = triggerPythonConfig.addSequence(TE, sequence,"L2_"+sliceId)

        l2chain.addHLTSignature(TE)

    # muComb
    ##########
    sequence = []
    if TriggerFlags.MuonSlice.doL2ID():
        sequence += [ TrigIDSCAN_Muon() ]

        if TriggerFlags.MuonSlice.testFlag("doSITRACK"):
            sequence += [ TrigSiTrack_Muon() ]


    if TriggerFlags.MuonSlice.doL2Muon() and TriggerFlags.MuonSlice.doL2ID():
        sequence += [ TrigmuCombConfig(lvl2Conf) ]
        sequence += [ MucombHypoConfig(lvl2Conf,thList[1]) ]

    if len(sequence) is not 0 and not ( TriggerFlags.MuonSlice.doL2Calo() and sliceId=='mu20i' ) :    
        TE = triggerPythonConfig.addSequence(TE,sequence,"L2_"+sliceId)
        l2chain.addHLTSignature(TE)
    elif len(sequence) is not 0:
        TE = triggerPythonConfig.addSequence(TE,sequence,"T2muFastComb_"+sliceId)
        l2chain.addHLTSignature(TE)

    # muIso
    ##########
    sequence = []
    if TriggerFlags.MuonSlice.doL2Calo() and sliceId == 'mu20i':        
        sequence += [ muIsoConfig() ]
        sequence += [ MuisoHypoConfig() ]

    if len(sequence) is not 0:    
        TE = triggerPythonConfig.addSequence(TE,sequence,"L2_"+sliceId)
        l2chain.addHLTSignature(TE)

    


    ##########################################
    # Starting configuring the EF signatures #
    ##########################################


    # ID reconstruction
    #####################

    sequence = []  
    if TriggerFlags.MuonSlice.doEFID():

        efInDetMuon = TrigEFIDInsideOut_Muon()

        TE = triggerPythonConfig.addSequence(TE,
                                             efInDetMuon.getSequence(),
                                             "EFID_"+sliceId)
        efchain.addHLTSignature(TE)

    if TriggerFlags.MuonSlice.doEFMuon():

        #include( "TrigMoore/jobOfragment_TrigMoore.py" ) 

        sequence += [ TrigMooreConfig_MS(lvl2Conf) ]
        if TriggerFlags.MuonSlice.doEFID():
            sequence += [ TrigMooreConfig_SA(lvl2Conf) ]
            sequence += [ TrigMooreConfig_CB(lvl2Conf) ]
        sequence += [ TrigMooreHypoConfig(lvl2Conf,thList[2]) ]

        
        
    if len(sequence) is not 0:    
        TE = triggerPythonConfig.addSequence(TE,sequence,"EF_"+sliceId)
        efchain.addHLTSignature(TE)    
    

    triggerPythonConfig.addHLTChain(l2chain)
    triggerPythonConfig.addHLTChain(efchain)





def TriggerConfig_stauMuon(TE , trigId, lvl2Conf ):

    sliceId = 'stau'
    
    try:
        counterId = dirMuonChainCounter[sliceId]
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
    l2chain.addTriggerTypeBit(4)
    l2chain.addStreamTag('muons', prescale='1')


    # defining the EF chain for signatures
    efchain = HLTChain( chain_name=sliceId+"_EF", 
                        chain_counter=counterId,
                        lower_chain_name=sliceId+"_L2",
                        level="EF", 
                        prescale='1', 
                        pass_through='0')
    efchain.addTriggerTypeBit(4)
    efchain.addStreamTag('muons', prescale='1')



    ##########################################
    # Starting configuring the L2 signatures #
    ##########################################

    # muFast
    ##########
    sequence = []
    if TriggerFlags.MuonSlice.doL2Muon():
        sequence += [ TrigmuFastConfig(lvl2Conf) ]
        sequence += [ StauHypoConfig(lvl2Conf) ]
	
        TE = triggerPythonConfig.addSequence(TE, sequence , 'L2_'+sliceId )
        l2chain.addHLTSignature(TE)


    triggerPythonConfig.addHLTChain(l2chain)
    triggerPythonConfig.addHLTChain(efchain)



def TriggerConfig_tileROD(TE , trigId, lvl2Conf ):

    sliceId = 'tileROD'
    
    try:
        counterId = dirMuonChainCounter[sliceId]
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
    l2chain.addTriggerTypeBit(4)
    l2chain.addStreamTag('muons', prescale='1')


    # defining the EF chain for signatures
    efchain = HLTChain( chain_name=sliceId+"_EF", 
                        chain_counter=counterId,
                        lower_chain_name=sliceId+"_L2",
                        level="EF", 
                        prescale='1', 
                        pass_through='0')
    efchain.addTriggerTypeBit(4)
    efchain.addStreamTag('muons', prescale='1')



    ##########################################
    # Starting configuring the L2 signatures #
    ##########################################

    # tile
    ##########
    sequence = []
    if TriggerFlags.MuonSlice.doL2Muon():
        sequence += [ TrigTileRODMuAlg_L2() ]
        sequence += [ TileMuHypoConfig(lvl2Conf) ]
	
        TE = triggerPythonConfig.addSequence(TE, sequence , 'L2_'+sliceId )
        l2chain.addHLTSignature(TE)


    triggerPythonConfig.addHLTChain(l2chain)
    triggerPythonConfig.addHLTChain(efchain)


def TriggerConfig_tileMU(TE , trigId, lvl2Conf ):

    sliceId = 'tileMU'
    
    try:
        counterId = dirMuonChainCounter[sliceId]
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
    l2chain.addTriggerTypeBit(4)
    l2chain.addStreamTag('muons', prescale='1')


    # defining the EF chain for signatures
    efchain = HLTChain( chain_name=sliceId+"_EF", 
                        chain_counter=counterId,
                        lower_chain_name=sliceId+"_L2",
                        level="EF", 
                        prescale='1', 
                        pass_through='0')
    efchain.addTriggerTypeBit(4)
    efchain.addStreamTag('muons', prescale='1')



    ##########################################
    # Starting configuring the L2 signatures #
    ##########################################

    # tile
    ##########
    sequence = []
    if TriggerFlags.MuonSlice.doL2Muon():

        sequence += [ TrigTileLookForMuAlg_L2() ]
        sequence += [ TileMuHypoConfig(lvl2Conf) ]
	
        TE = triggerPythonConfig.addSequence(TE, sequence , 'L2_'+sliceId )
        l2chain.addHLTSignature(TE)


    triggerPythonConfig.addHLTChain(l2chain)
    triggerPythonConfig.addHLTChain(efchain)
