# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###########################################################################
# Configure L2 and EF monitoring chains
###########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( 'TriggerMenu.calibcosmon.MonitorDef' )

from TriggerMenu.menu.HltConfig import L2EFChainDef


###########################################################################
###########################################################################

class L2EFChain_Monitoring(L2EFChainDef):
    def __init__(self, chainDict):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainName = chainDict['chainName']
        self.chainPart = chainDict['chainParts']  
        self.chainL1Item = chainDict['L1item']       
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.monType = self.chainPart['monType']

        self.L2InputTE = self.chainL1Item 

        if ('timeburner' in self.monType):
            self.setupTimeBurnerChain()
        elif ('idmon' in self.monType):
            self.setupIdmonTrkFS()
        elif ('costmonitor' in self.monType):
            self.setupCostMonChain()
        elif ('cscmon' in self.monType):
            self.setupCSCMonChain()
        elif ('l1calooverflow' in self.monType):
            self.setupL1CaloOverflow()
        elif ('l1topodebug' in self.monType):
            self.setupL1TopoTransmission()
        else:
            log.error("No suitable configuration for chain %s found!" % self.chainName)

        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                              self.L2InputTE, self.EFName, self.chainCounter, self.L2InputTE)

            
    def defineSequences(self):   
        for sequence in self.L2sequenceList:
            self.addL2Sequence(*sequence)            
        for sequence in self.EFsequenceList:
            self.addEFSequence(*sequence)
          
    def defineSignatures(self):
        for signature in self.L2signatureList:
            self.addL2Signature(*signature)
        for signature in self.EFsignatureList:
            self.addEFSignature(*signature)          
           
    def defineTErenaming(self):
        self.TErenamingMap=self.TErenamingDict


    ####################################
    ####################################
    def setupCSCMonChain(self):
        from TrigDetCalib.TrigDetCalibConfig import CSCSubDetListWriter        
        CSC_PEB = CSCSubDetListWriter('CSCSubDetListWriter_' + self.chainName)

        outputTE='L2_CSCSubDetListWriter_'+self.chainName

        self.L2sequenceList +=  [ [ '', [CSC_PEB], outputTE ] ]        

        self.L2signatureList += [ [[outputTE]] ]
        

    ####################################
    ####################################
    def setupTimeBurnerChain(self):
        from TrigGenericAlgs.TrigGenericAlgsConf import TimeBurner
        TimeBurner= TimeBurner("DummyTimeBurner")
        TimeBurner.TimeDelay = 200
        self.L2sequenceList += [[ self.L2InputTE,     
                                  [TimeBurner], 
                                  'L2_DummyTimeBurner']]
        self.L2signatureList += [ [['L2_DummyTimeBurner']] ]
        

    ####################################
    ####################################
    def setupCostMonChain(self):
        #from TrigDetCalib.TrigDetCalibConfig import TDAQ_L2SubDetListWriter
        #l2_seq_peb = 'HLT_l2_'+self.chainName+'_peb'
        #self.L2sequenceList += [[  '',[TDAQ_L2SubDetListWriter('TDAQ_L2SubDetListWriter')],l2_seq_peb]]
        #self.L2signatureList += [ [[l2_seq_peb]] ]

        from TrigDetCalib.TrigDetCalibConfig import TDAQ_HLTSubDetListWriter        
        ef_seq_peb = 'HLT_'+self.chainName+'_peb'
        self.L2sequenceList += [['',[TDAQ_HLTSubDetListWriter('TDAQ_HLTSubDetListWriter')],ef_seq_peb]]                               
        self.L2signatureList += [ [[ef_seq_peb]] ]

    ####################################
    ####################################
    def setupL1CaloOverflow(self):
        from TrigDetCalib.TrigDetCalibConf import TrigL1CaloOverflow
        CaloOverflowMonitor = TrigL1CaloOverflow("TrigL1CaloOverflow")
        self.L2sequenceList += [[ '' , [CaloOverflowMonitor],  'L2_l1calooverflow']]
        self.L2signatureList += [ [['L2_l1calooverflow']] ]
    ####################################
    ####################################
    def setupL1TopoTransmission(self):
        from TrigGenericAlgs.TrigGenericAlgsConf import AcceptL1TopoMonitor
        l1topodebug = AcceptL1TopoMonitor('L1TopoAcceptDebug')
        self.L2sequenceList += [[ '' , [l1topodebug],  'L2_l1topodebug']]
        self.L2signatureList += [ [['L2_l1topodebug']] ]

    ####################################
    ####################################
