# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags                               import TriggerFlags
from TriggerJobOpts.MuonSliceFlags           import MuonSliceFlags
from TriggerJobOpts.EgammaSliceFlags       import EgammaSliceFlags
from TriggerJobOpts.JetSliceFlags             import JetSliceFlags
from TriggerJobOpts.CombinedSliceFlags   import CombinedSliceFlags

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataFlow( True )

from AthenaCommon.CFElements import parOR, seqAND, stepSeq
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import *
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import *
from TriggerMenuMT.HLTMenuConfig.Menu.HLTSignatureConfig import *

import os, traceback, operator, commands, time

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT' )

_func_to_modify_the_menu = None
_func_to_modify_signatures = None

class GenerateMenuMT:   
    
    def __init__(self):
        self.triggerMTConfig = None
        self.chains = []
        self.chainDefs = []
        self.listOfErrorChainDefs = []
        self.signaturesOverwritten = False
        # flags
        self.doEgammaChains      = True
        self.doJetChains         = True
        self.doMuonChains        = True
        self.doCombinedChains    = True



    def getChainDef(self,chainDicts):
    # get the Chain object from each signature of the form Chain(chainname, L1seed, ChainSteps
        if self.doMuonChains:
            try:
                import TriggerMenu.muon.generateMuonChainDefs 
            except:
                log.error('Problems when importing MuonDef.py, disabling muon chains.')
                log.info(traceback.print_exc())
                self.doMuonChains = False
                        

        allowedSignatures = ["jet","egamma","muon", "electron", "photon","met","tau", 
                             "minbias", "heavyion", "cosmic", "calibration", "streaming", "monitoring", "ht", 'bjet','eb']        
    
        listOfChainDefs = []
        chainDicts = TriggerMenuMT.HLTMenuConfig.Menu.MenuUtils.splitInterSignatureChainDict(chainDicts)        
        log.debug("\n chainDicts2 %s", chainDicts)

        for chainDict in chainDicts:
            chainDef = None
            print 'checking chainDict for chain %s %s %r' %(chainDict['chainName'],chainDict["signature"], self.doEnhancedBiasChains)
            
            if chainDict["signature"] == "Muon" and self.doMuonChains:
                try:
                    chainDef = TriggerMenuMT.HLTMenuConfig.Muon.generateMuonChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue
            
            else:
                log.error('Chain %s ignored - either because the trigger signature ("slice") has been turned off or because the corresponding chain dictionary cannot be read.' %(chainDict['chainName']))
                log.debug('Chain dictionary of failed chain is %s.', chainDict)
            
            log.debug(' ChainDef  %s ' % chainDef)


        if len(listOfChainDefs) == 0:  
            return False
        else:
            theChainDef = listOfChainDefs[0]
            
        return theChainDef



    def writeLvl1EmuFiles(self):
        data = {'noreco': [';', ';', ';']}  # in the lists there are the events
        data['emclusters'] = ['eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:35000;',
                              'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;',
                              'eta:-0.6,phi:1.7,et:9000;']
        data['msmu']  = [';',
                         'eta:-1.2,phi:0.7,pt:6500; eta:-1.1,phi:0.6,pt:8500;',
                         'eta:-1.7,phi:-0.2,pt:9500;']
        
        data['ctp'] = [ 'HLT_g100',  'HLT_2g50 HLT_e20', 'HLT_mu20 HLT_mu8 HLT_2mu8 HLT_mu8_e8' ]
        
        data['l1emroi'] = ['1,1,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.2,0,EM3,EM7',
                           '-0.6,0.2,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM50',
                           '-0.6,1.5,0,EM3,EM7,EM7']
        
        data['l1muroi'] = ['0,0,0,MU0;',
                           '-1,0.5,0,MU6,MU8; -1,0.5,0,MU6,MU8,MU10',
                           '-1.5,-0.1,0,MU6,MU8']
        
        data['tracks'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                          'eta:0.5,phi:0,pt:130000; eta:1,phi:-1.2,pt:60000;eta:-1.2,phi:0.7,pt:6700; eta:-1.1,phi:0.6,pt:8600;',
                          'eta:-0.6,phi:1.7,et:9000;'] # no MU track for MS candidate 'eta:-1.7,phi:-0.2,pt:9500;'
        
        data['mucomb'] = [';',
                          'eta:-1.2,phi:0.7,pt:6600; eta:-1.1,phi:0.6,pt:8600;',
                          ';']
        
        data['electrons'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                             ';',
                             ';']
        data['photons'] = ['eta:1,phi:1,pt:130000;',
                           ';',
                           ';']    
        from TrigUpgradeTest.TestUtils import writeEmulationFiles
        writeEmulationFiles(data)
        

    def Lvl1Decoder(self):
        #_L1UnpackingSeq = parOR("L1UnpackingSeq")
        from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
        l1Decoder = L1Decoder( OutputLevel=DEBUG, RoIBResult="" )
        l1Decoder.prescaler.EventInfo=""
        
        ctpUnpacker = CTPUnpackingEmulationTool( OutputLevel =  DEBUG, ForceEnableAllChains=False , InputFilename="ctp.dat" )
        #ctpUnpacker.CTPToChainMapping = [ "0:HLT_g100",  "1:HLT_e20", "2:HLT_mu20", "3:HLT_2mu8", "3:HLT_mu8", "33:HLT_2mu8", "15:HLT_mu8_e8" ]
        l1Decoder.ctpUnpacker = ctpUnpacker
        
        emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1emroi.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EM" )
        emUnpacker.ThresholdToChainMapping = ["EM7 : HLT_mu8_e8", "EM20 : HLT_e20", "EM50 : HLT_2g50",   "EM100 : HLT_g100" ]
        
        muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1muroi.dat",  OutputTrigRoIs="L1MURoIs", Decisions="L1MU" )
        muUnpacker.ThresholdToChainMapping = ["MU6 : HLT_mu6", "MU8 : HLT_mu8", "MU8 : HLT_2mu8",  "MU8 : HLT_mu8_e8",  "MU10 : HLT_mu20",   "EM100 : HLT_g100" ]
        
        l1Decoder.roiUnpackers = [emUnpacker, muUnpacker]

        #print l1Decoder
        #_L1UnpackingSeq += l1Decoder
        print l1Decoder

        return l1Decoder


    def allChains(self):
        # muon chains
        muStep1 = muStep1Sequence()
        muStep2 = muStep2Sequence()
        MuChains  = [
            Chain(name='HLT_mu20', Seed="L1_MU10",   ChainSteps=[ChainStep("Step1_mu20", [SequenceHypoTool(muStep1,step1mu20())]),
                                                                 ChainStep("Step2_mu20", [SequenceHypoTool(muStep2,step2mu20())]) ] ),            
            Chain(name='HLT_mu8',  Seed="L1_MU6",    ChainSteps=[ChainStep("Step1_mu8",  [SequenceHypoTool(muStep1,step1mu8())] ),
                                                                 ChainStep("Step2_mu8",  [SequenceHypoTool(muStep2,step2mu8())]) ] )
            ]
        #electron chains
        elStep1 = elStep1Sequence()
        elStep2 = elStep2Sequence()
        ElChains  = [
            Chain(name='HLT_e20' , Seed="L1_EM10", ChainSteps=[ ChainStep("Step1_e20",  [SequenceHypoTool(elStep1,step1e20())]),
                                                                ChainStep("Step2_e20",  [SequenceHypoTool(elStep2,step2e20())]) ] )
            ]
        # combined chain
        muelStep1 = combStep1Sequence()
        muelStep2 = combStep2Sequence()
        CombChains =[
            Chain(name='HLT_mu8_e8' , Seed="L1_EM6_MU6", ChainSteps=[ ChainStep("Step1_mu8_e8",  [SequenceHypoTool(muelStep1, step1mu8_e8())]),
                                                                      ChainStep("Step2_mu8_e8",  [SequenceHypoTool(muelStep2, step2mu8_e8())]) ] )
            ]

        _allChains = MuChains + ElChains + CombChains
        return _allChains



    #----------------------#
    # Menu generation #
    #----------------------#
    def generateMT(self):
        log.info('GenerateMenuMT.py:generateMT ')

        topSequence = AlgSequence()
        TopHLTRootSeq = seqAND("TopHLTRootSeq") 
        topSequence += TopHLTRootSeq
        
        #----------------------#
        # L1 menu generation #
        #----------------------#
        self.writeLvl1EmuFiles()
        Lvl1UnpackingSeq = parOR("L1UnpackingSeq")
        Lvl1UnpackingSeq += Lvl1Decoder()
        log.info('Lvl1UnpackingSeq: %s', Lvl1UnpackingSeq  )
        TopHLTRootSeq += L1UnpackingSeq

        #----------------------#
        # HLT menu generation #
        #----------------------#
        nsetps = 2 

        HLTAllStepsSeq = seqAND("HLTAllStepsSeq")
        TopHLTRootSeq += HLTAllStepsSeq
        
        group_of_chains = allChains()
        print 'all Chains ', allChains()
        decisionTree_From_Chains(HLTAllStepsSeq, group_of_chains, NSTEPS=nsteps)

 

 
