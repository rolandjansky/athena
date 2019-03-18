# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags                       import TriggerFlags
from TriggerMenu.muon.MuonSliceFlags                   import MuonSliceFlags
from TriggerMenu.bphysics.BphysicsSliceFlags           import BphysicsSliceFlags
from TriggerMenu.egamma.EgammaSliceFlags               import EgammaSliceFlags
from TriggerMenu.jet.JetSliceFlags                     import JetSliceFlags
from TriggerMenu.bjet.BjetSliceFlags                   import BjetSliceFlags
from TriggerMenu.met.METSliceFlags                     import METSliceFlags
from TriggerMenu.tau.TauSliceFlags                     import TauSliceFlags
from TriggerMenu.afp.AFPSliceFlags                     import AFPSliceFlags
from TriggerMenu.minbias.MinBiasSliceFlags             import MinBiasSliceFlags
from TriggerMenu.heavyion.HeavyIonSliceFlags           import HeavyIonSliceFlags
from TriggerMenu.combined.CombinedSliceFlags           import CombinedSliceFlags
from TriggerMenu.calibcosmicmon.CosmicSliceFlags       import CosmicSliceFlags
from TriggerMenu.calibcosmicmon.CalibSliceFlags        import CalibSliceFlags
from TriggerMenu.calibcosmicmon.StreamingSliceFlags    import StreamingSliceFlags
from TriggerMenu.calibcosmicmon.MonitorSliceFlags      import MonitorSliceFlags
from TriggerMenu.calibcosmicmon.BeamspotSliceFlags     import BeamspotSliceFlags
from TriggerMenu.calibcosmicmon.EnhancedBiasSliceFlags import EnhancedBiasSliceFlags
from TriggerMenu.test.TestSliceFlags                   import TestSliceFlags

from TriggerMenu.menu.TriggerPythonConfig  import TriggerPythonConfig
from TriggerMenu.menu.CPS                  import addCPS
from TriggerMenu.menu.Lumi                 import lumi, applyPrescales
from TriggerMenu.menu.MenuUtil             import checkStreamConsistency, getStreamTagForRerunChains,checkGroups
from TriggerMenu.menu.HLTObjects           import HLTChain, HLTSequence
from TriggerMenu.menu                      import StreamInfo, DictFromChainName
from TriggerMenu.menu.MenuUtils            import splitInterSignatureChainDict,mergeChainDefs

import os, traceback, operator, commands

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.GenerateMenu' )


_func_to_modify_the_menu = None
_func_to_modify_signatures = None


class GenerateMenu:   
    
    def overwriteSignaturesWith(f):
        log.info('In overwriteSignaturesWith ')
        global _func_to_modify_signatures
        if _func_to_modify_signatures != None:
            log.warning('Updating the function to modify signatures from %s to %s'\
                  % (_func_to_modify_signatures.__name__, f.__name__))
        _func_to_modify_signatures = f

    def overwriteMenuWith(f):
        log.info('In overwriteSignaturesWith ')
        global _func_to_modify_the_menu
        if _func_to_modify_the_menu != None:
            log.warning('Updating the function to modify the menu from %s to %s'\
                  % (_func_to_modify_the_menu.__name__, f.__name__))
        _func_to_modify_the_menu = f
    overwriteSignaturesWith = staticmethod(overwriteSignaturesWith)
    overwriteMenuWith = staticmethod(overwriteMenuWith)
    

    def __init__(self):

        self.triggerPythonConfig = None
        self.chains = []
        self.chainDefs = []
        self.listOfErrorChainDefs = []
        self.signaturesOverwritten = False
        self.L1Prescales = None
        self.HLTPrescales = None
        
        # flags
        self.doEgammaChains      = True
        self.doJetChains         = True
        self.doBjetChains        = True
        self.doMuonChains        = True
        self.doBphysicsChains    = True
        self.doMETChains         = True
        self.doTauChains         = True
        self.doAFPChains         = True
        self.doMinBiasChains     = True
        self.doHeavyIonChains     = True
        self.doCosmicChains      = True
        self.doCalibrationChains = True
        self.doStreamingChains   = True
        self.doMonitorChains   = True
        self.doBeamspotChains   = True
        self.doEnhancedBiasChains   = True
        self.doTestChains   = True
        self.doCombinedChains   = True

        # Or like this:
        #self.enabledSignatures = ['Egamma','Jet','Muon','Bphysics','MET',
        #'Tau','MinBias','Cosmic','Calibration','Streaming']
    

    def deactivateChains(self,signatureGroupsToDeactivate):
        for signatureGroupToDeactivate in signatureGroupsToDeactivate:
            try:
                eval("self.do"+signatureGroupToDeactivate+"Chains = False")
            except:
                log.error('GenerateMenu: Could not deactivate trigger signature:',signatureGroupToDeactivate)


    def getChainsFromMenu(self):
        """
        == returns the list of chain names that are in the menu 
        """
        
        log.debug('Creating list of all available slice signatures flags')
        # make one big list of enabled signatures        
        chains = []

        if (CombinedSliceFlags.signatures() or EgammaSliceFlags.signatures()) and self.doEgammaChains:
            chains += EgammaSliceFlags.signatures()        
            log.debug('GenerateMenu : Electrons and Photons : %s', chains)
        else:
            self.doEgammaChains   = False

        if (CombinedSliceFlags.signatures() or JetSliceFlags.signatures()) and self.doJetChains:
            chains += JetSliceFlags.signatures()
        else:
            self.doJetChains = False

        if (CombinedSliceFlags.signatures() or BjetSliceFlags.signatures()) and self.doBjetChains:
            chains += BjetSliceFlags.signatures()
        else:
            self.doBjetChains = False

        if (CombinedSliceFlags.signatures() or MuonSliceFlags.signatures()) and self.doMuonChains:
            chains += MuonSliceFlags.signatures()
            log.debug('GenerateMenu : Muons : %s', chains)
        else:
            self.doMuonChains = False

        if (CombinedSliceFlags.signatures() or BphysicsSliceFlags.signatures()) and self.doBphysicsChains:
            chains += BphysicsSliceFlags.signatures()
            log.debug('GenerateMenu : Bphysics : %s', chains)
        else:
            self.doBphysicsChains = False

        if (CombinedSliceFlags.signatures() or METSliceFlags.signatures()) and self.doMETChains:
            chains += METSliceFlags.signatures()
            log.debug('GenerateMenu : MET : %s', chains)
        else:
            self.doMETChains = False

        if (CombinedSliceFlags.signatures() or TauSliceFlags.signatures()) and self.doTauChains:
            chains += TauSliceFlags.signatures()
            log.debug('GenerateMenu : Tau : %s', chains)
        else:
            self.doTauChains = False

        if (CombinedSliceFlags.signatures() or AFPSliceFlags.signatures()) and self.doAFPChains:
            chains += AFPSliceFlags.signatures()
            log.debug('GenerateMenu : AFP : %s', chains)
        else:
            self.doAFPChains = False

        if (CombinedSliceFlags.signatures() or MinBiasSliceFlags.signatures()) and self.doMinBiasChains:
            chains += MinBiasSliceFlags.signatures()
            log.debug('GenerateMenu : MinBias : %s', chains)
        else:
            self.doMinBiasChains = False

        if (CombinedSliceFlags.signatures() or HeavyIonSliceFlags.signatures()) and self.doHeavyIonChains:
            chains += HeavyIonSliceFlags.signatures()
            log.debug('GenerateMenu : HeavyIon : %s', chains)
        else:
            self.doHeavyIonChains = False

        if CosmicSliceFlags.signatures() and self.doCosmicChains:
            chains += CosmicSliceFlags.signatures()
            log.debug('GenerateMenu : Cosmic : %s', chains)
        else:
            self.doCosmicChains = False

        if CalibSliceFlags.signatures() and self.doCalibrationChains:
            chains += CalibSliceFlags.signatures()
            log.debug('GenerateMenu : Calibration : %s', chains)
        else:
            self.doCalibrationChains = False

        if StreamingSliceFlags.signatures() and self.doStreamingChains:
            chains += StreamingSliceFlags.signatures()            
            log.debug('GenerateMenu : Streaming : %s', chains)
        else:
            self.doStreamingChains   = False

        if MonitorSliceFlags.signatures() and self.doMonitorChains:
            chains += MonitorSliceFlags.signatures()            
            log.debug('GenerateMenu : Monitor : %s', chains)
        else:
            self.doMonitorChains   = False

        if BeamspotSliceFlags.signatures() and self.doBeamspotChains:
            chains += BeamspotSliceFlags.signatures()            
            log.debug('GenerateMenu : Beamspot : %s', chains)
        else:
            self.doBeamspotChains   = False

        if EnhancedBiasSliceFlags.signatures() and self.doEnhancedBiasChains:
            chains += EnhancedBiasSliceFlags.signatures()            
            log.debug('GenerateMenu : EnhancedBias : %s', chains)
        else:
            self.doEnhancedBiasChains   = False

        if TestSliceFlags.signatures() and self.doTestChains:
            chains += TestSliceFlags.signatures()            
            log.debug('GenerateMenu : Test chains : %s', chains)
        else:
            self.doTestChains   = False

        log.debug( 'Enabled before comb chains: %s', chains)
        if CombinedSliceFlags.signatures():
            chains += CombinedSliceFlags.signatures()
        else:
            self.doCombinedChains = False
        log.debug( 'Enabled after comb chains: %s', chains)
        
        log.debug('GenerateMenu : all Chains : %s', chains)
        return chains
        

    def checkL1SeedsForChainsFromMenu(self,chains):
        from TriggerMenu.menu.L1Seeds import getSpecificL1Seeds
        l1itemnames = [i.name for i in self.trigConfL1.menu.items]
        missingL1items = []
        for chain in chains:
            log.debug('chain %s', chain)
            l1item = chain[1]
            if (l1item not in l1itemnames) & (l1item != '') and ',' not in l1item:
                myl1item = getSpecificL1Seeds(l1item, self.trigConfL1.menu.items)
                if ('ERROR_' in myl1item):
                    if (l1item not in missingL1items):  missingL1items.append(l1item)                    
                else:
                    chain[1] = myl1item
            elif ',' in l1item:
                myl1item = l1item
                for each_l1item in l1item.split(','):
                    if each_l1item not in l1itemnames:
                        myl1item = 'ERROR'
                        missingL1items.append(l1item)
                if ('ERROR' in myl1item):
                    if (l1item not in missingL1items):  missingL1items.append(l1item)                    
                else:
                    chain[1] = myl1item

        if  len(missingL1items) > 0 :
            log.error('The following L1 items were not found in the corresponding L1 menu: '+str(missingL1items))

        log.debug('GenerateMenu : all Chains : %s', chains)
        return chains

    def checkUnusedL1Items(self,chains):
        l1ItemsInL1Menu = [i.name for i in self.trigConfL1.menu.items]
        l1ItemsInHLTMenu = []
        for chain in chains:
            l1ItemsInHLTMenu += [chain[1]]
        list(set(l1ItemsInHLTMenu))
        for l1ItemInL1Menu in l1ItemsInL1Menu:
            if not l1ItemInL1Menu in l1ItemsInHLTMenu:
                log.info('L1 item %s is not used by any HLT chain' % l1ItemInL1Menu)
            
            
    def CheckIntraSignatureTopo(self,chainDicts):
        if not chainDicts[0]['topo']:
            return False

        IntraSignatureList=[]
        for ChainPart in chainDicts:
            if ChainPart['signature'] not in IntraSignatureList:
                IntraSignatureList.append(ChainPart['signature'])
        if len(IntraSignatureList)>1:
            return True        
        else:
            return False


    def getChainDef(self,chainDicts):
        """
        == gets ChainDef object from the signature code
        """

        # generation of ChainDef objects for each signature
        if self.doMuonChains:
            try:
                import TriggerMenu.muon.generateMuonChainDefs 
            except:
                log.error('Problems when importing MuonDef.py, disabling muon chains.')
                log.info(traceback.print_exc())
                self.doMuonChains = False
                        
        if self.doBphysicsChains:
            try:
                import TriggerMenu.bphysics.generateBPhysicsChainDefs 
            except:
                log.error('Problems when importing BphysicsDef.py, disabling Bphysics chains.')
                log.info(traceback.print_exc())
                self.doBphysicsChains = False
                
        if self.doMETChains: 
            try:
                import TriggerMenu.met.generateMETChainDefs 
            except:
                log.error('Problems when importing MissingETDef.py, disabling MET chains.')
                log.info(traceback.print_exc())
                self.doMETChains = False

        if self.doTauChains:
            try:
                import TriggerMenu.tau.generateTauChainDefs 
            except:
                log.error('Problems when importing TauDef.py, disabling tau chains.')
                log.info(traceback.print_exc())
                self.doTauChains = False

        if self.doEgammaChains:
            try:
                import TriggerMenu.egamma.generateElectronChainDefs 
                import TriggerMenu.egamma.generatePhotonChainDefs 
            except:
                log.error('Problems when importing EgammaDef.py or PhotonDef.py, disabling egamma chains.')
                log.info(traceback.print_exc())
                self.doEgammaChains = False

        if self.doJetChains:
            try:
                import TriggerMenu.jet.generateJetChainDefs 
            except:
                log.error('Problems when importing JetDef.py or JetDef_HT.py, disabling jet chains.')
                log.info(traceback.print_exc())
                self.doJetChains = False

        if self.doBjetChains:
            try:
                import TriggerMenu.bjet.generateBjetChainDefs 
            except:
                log.error('Problems when importing BjetDef.py disabling bjet chains.')
                log.info(traceback.print_exc())
                self.doBjetChains = False

        if self.doAFPChains:
            try:
                import TriggerMenu.afp.generateAFPChainDefs
            except:
                log.error('Problems when importing AFP.py, disabling AFP chains.')
                log.info(traceback.print_exc())
                self.doAFPChains = False

        if self.doMinBiasChains:
            try:
                import TriggerMenu.minbias.generateMinBiasChainDefs 
            except:
                log.error('Problems when importing MinBiasDef.py, disabling MinBias chains.')
                log.info(traceback.print_exc())
                self.doMinBiasChains = False

        if self.doHeavyIonChains:
            try:
                import TriggerMenu.heavyion.generateHeavyIonChainDefs
            except:
                log.error('Problems when importing HeavyIonDef.py, disabling HeavyIon chains.')
                log.info(traceback.print_exc())
                self.doHeavyIonChains = False

        if self.doCosmicChains:
            try:
                import TriggerMenu.calibcosmicmon.generateCosmicChainDefs 
            except:
                log.error('Problems when importing CosmicDef.py, disabling cosmic chains.')
                log.info(traceback.print_exc())
                self.doCosmicChains = False

        if self.doCalibrationChains:
            try:
                import TriggerMenu.calibcosmicmon.generateCalibChainDefs 
            except:
                log.error('Problems when importing CalibDef.py, disabling calibration chains.')
                log.info(traceback.print_exc())
                self.doCalibrationChains = False
                
        if self.doStreamingChains:
            try:
                import TriggerMenu.calibcosmicmon.generateStreamingChainDefs 
            except:
                log.error('Problems when importing Streaming.py, disabling streaming chains.')
                log.info(traceback.print_exc())
                self.doStreamingChains = False
                

        if self.doMonitorChains:
            try:
                import TriggerMenu.calibcosmicmon.generateMonitoringChainDefs 
            except:
                log.error('Problems when importing Monitor.py, disabling monitoring chains.')
                log.info(traceback.print_exc())
                self.doMonitorChains = False

        if self.doBeamspotChains:
            try:
                import TriggerMenu.calibcosmicmon.generateBeamspotChainDefs 
            except:
                log.error('Problems when importing Beamspot.py, disabling beamspot chains.')
                log.info(traceback.print_exc())
                self.doBeamspotChains = False

        if self.doEnhancedBiasChains:
            try:
                import TriggerMenu.calibcosmicmon.generateEnhancedBiasChainDefs 
            except:
                log.error('Problems when importing EnhancedBias.py, disabling EnhancedBias chains.')
                log.info(traceback.print_exc())
                self.doEnhancedBiasChains = False

        if self.doTestChains:
            try:
                import TriggerMenu.test.generateTestChainDefs 
            except:
                log.error('Problems when importing Test.py, disabling Test chains.')
                log.info(traceback.print_exc())
                self.doTestChains = False

                
        if self.doCombinedChains:
            try:
                import TriggerMenu.combined.generateCombinedChainDefs 
            except:
                log.error('Problems when importing generateCombinedChainDefs.py, disabling Topo on combined chains.')
                log.info(traceback.print_exc())
                self.doCombinedChains = False



        #allowedSignatures = ["jet","egamma","muon", "electron", "photon","met","tau", "afp",
        #                     "minbias", "heavyion", "cosmic", "calibration", "streaming", "monitoring", "ht", 'bjet','eb']
        
        listOfChainDefs = []

        log.debug("\n chainDicts1 %s ", chainDicts)
        chainDicts = splitInterSignatureChainDict(chainDicts)        
        log.debug("\n chainDicts2 %s", chainDicts)
        

        #print 'doEgammaChains, doMuonChains', self.doEgammaChains, self.doMuonChains

        for chainDict in chainDicts:
            chainDefs = None
            # print 'checking chainDict for chain %s %s %r' %(chainDict['chainName'],chainDict["signature"], self.doEnhancedBiasChains)

            if (chainDict["signature"] == "Jet" or chainDict["signature"] == "HT") and (self.doJetChains or self.doBjetChains):
                bjetchain = False
                for chainpart in chainDict["chainParts"]:
                    if chainpart['bTag']: bjetchain = True

                if (bjetchain == True) and self.doBjetChains:
                    try:
                        chainDefs = TriggerMenu.bjet.generateBjetChainDefs.generateChainDefs(chainDict)
                    except:
                        log.error('Problems creating ChainDef for bjet chain %s ' % (chainDict['chainName']))
                        log.info(traceback.print_exc())
                        continue
                elif self.doJetChains:                    
                    try:
                        chainDefs = TriggerMenu.jet.generateJetChainDefs.generateChainDefs(chainDict)
                    except:
                        log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                        log.info(traceback.print_exc())
                        continue


            elif chainDict["signature"] == "Muon" and self.doMuonChains:
                try:
                    chainDefs = TriggerMenu.muon.generateMuonChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Bphysics" and self.doBphysicsChains:
                try:
                    chainDefs = TriggerMenu.bphysics.generateBPhysicsChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Electron" and self.doEgammaChains:
                try:
                    chainDefs = TriggerMenu.egamma.generateElectronChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Photon" and self.doEgammaChains:
                try:
                    chainDefs = TriggerMenu.egamma.generatePhotonChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue
                 
            elif (chainDict["signature"] == "MET" or chainDict["signature"] == "XS" or chainDict["signature"] == "TE") and self.doMETChains:
                try:
                    chainDefs = TriggerMenu.met.generateMETChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Tau" and self.doTauChains:
                try:
                    chainDefs = TriggerMenu.tau.generateTauChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue
                
            elif chainDict["signature"] == "AFP" and self.doAFPChains:
                try:
                    chainDefs = TriggerMenu.afp.generateAFPChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "MinBias" and self.doMinBiasChains:
                try:
                    chainDefs = TriggerMenu.minbias.generateMinBiasChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "HeavyIon" and self.doHeavyIonChains:
                try:
                    chainDefs = TriggerMenu.heavyion.generateHeavyIonChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Cosmic" and self.doCosmicChains:
                try:
                    chainDefs = TriggerMenu.calibcosmicmon.generateCosmicChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Calibration" and self.doCalibrationChains:
                try:
                    chainDefs = TriggerMenu.calibcosmicmon.generateCalibChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Streaming" and self.doStreamingChains:
                try:
                    chainDefs = TriggerMenu.calibcosmicmon.generateStreamingChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Monitoring" and self.doMonitorChains:
                try:
                    chainDefs = TriggerMenu.calibcosmicmon.generateMonitoringChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "Beamspot" and self.doBeamspotChains:
                try:
                    chainDefs = TriggerMenu.calibcosmicmon.generateBeamspotChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            elif chainDict["signature"] == "EnhancedBias" and self.doEnhancedBiasChains:
                try:
                    chainDefs = TriggerMenu.calibcosmicmon.generateEnhancedBiasChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue

            
            elif chainDict["signature"] == "Test" and self.doTestChains:
                try:
                    chainDefs = TriggerMenu.test.generateTestChainDefs.generateChainDefs(chainDict)
                except:
                    log.error('Problems creating ChainDef for chain %s ' % (chainDict['chainName']))
                    log.info(traceback.print_exc())
                    continue


            else:                
                log.error('Chain %s ignored - either because the trigger signature ("slice") has been turned off or because the corresponding chain dictionary cannot be read.' %(chainDict['chainName']))
                log.debug('Chain dictionary of failed chain is %s.', chainDict)
                         
            log.debug(' ChainDef  %s ' % chainDefs)
            from ChainDef import ErrorChainDef,ChainDef

            if not isinstance(chainDefs,list):
                chainDefs = [chainDefs]
                
            for chainDef in chainDefs:
                if isinstance(chainDef, ErrorChainDef): 
                    self.listOfErrorChainDefs.append(chainDict['chainName'])
                    continue
                elif isinstance(chainDef, ChainDef):
                    listOfChainDefs.append(chainDef)
                

        doTopo = self.CheckIntraSignatureTopo(chainDicts) and chainDict["topo"]

        if len(listOfChainDefs) == 0:# or not (len(listOfChainDefs)==len(chainDicts)):
            return False
        elif len(listOfChainDefs)>1:
            if ("mergingStrategy" in chainDicts[0].keys()):
                theChainDef = mergeChainDefs(listOfChainDefs,chainDicts[0]["mergingStrategy"],chainDicts[0]["mergingOffset"],preserveL2EFOrder = chainDicts[0]["mergingPreserveL2EFOrder"],doTopo=doTopo,chainDicts=chainDicts)#, noTEreplication = chainDicts[0]["mergingNoTEreplication"])
            else:
                log.error("No merging strategy specified for combined chain %s" % chainDicts[0]['chainName'])
                

        else:
            theChainDef = listOfChainDefs[0]

        #Do TOPO on Combined chains
        if self.doCombinedChains:
            log.info('doTopo for combined chain = %s' % str(doTopo))
            log.info(theChainDef)
            if doTopo:
                log.info('run generateCombinedChainDefs')
                theChainDef = TriggerMenu.combined.generateCombinedChainDefs._addTopoInfo(theChainDef,chainDicts,listOfChainDefs)
                log.info(theChainDef)        
        return theChainDef

    
    def setupMenu(self):
        # go over the slices and put together big list of signatures requested
     
        #(L1Prescales, HLTPrescales, streamConfig) = lumi(self.triggerPythonConfig)
        (self.L1Prescales, self.HLTPrescales) = lumi(self.triggerPythonConfig)
        global _func_to_modify_signatures
        if _func_to_modify_signatures != None:
            log.info('setupMenu:  Modifying trigger signatures in TriggerFlags with %s' % \
                     _func_to_modify_signatures.__name__)
            _func_to_modify_signatures()
            self.signaturesOverwritten = True

        return (self.HLTPrescales)

        

    def generateHLTChain(self,theChainDef,theChainCounter, theStreamTags, theGroups, theEBstep):
        theHLTChain = HLTChain( chain_name = theChainDef.chain_name,
                                chain_counter = theChainCounter,
                                lower_chain_name = theChainDef.lower_chain_name,
                                level = theChainDef.level,
                                #stream_tag = [ ('jettauetmiss','physics','yes','1') ]) # streamtag has to be a tuple: (name, type, obeyLB, prescale)
                                stream_tag = theStreamTags, 
                                groups = theGroups,
                                EBstep = theEBstep)
        
        for signature in theChainDef.signatureList:
            theHLTChain.addHLTSignature(telist=signature['listOfTriggerElements'], sigcounter=signature['signature_counter'], logic='1') 
            # TODO: Signature counter will haev to be overwritten in some cases
        return theHLTChain


    def generateHLTSequences(self,theChainDef):
        theHLTSequences = []
        
        for sequence in theChainDef.sequenceList:
            theHLTSequences += [HLTSequence(inputTEs=sequence["input"], algos=sequence["algorithm"], outputTE=sequence["output"], topo_start_from=sequence["topo_start_from"])]
        return theHLTSequences


    def chainDefIsConsistent(self,theChainDef):
        """
        Check if for each TE the sequences and all their inputs exist
        This test may be CPU intensive and can be switched off if menu generation takes to long
        """

        allTEs = []
        for signature in theChainDef.signatureList:
            allTEs += signature['listOfTriggerElements']

        # remove duplicates 
        allTEs = list(set(allTEs))
        log.debug("allTEs %s", allTEs)
        
        def inputsAreTEs(inputs):
            for input in inputs:
                if not input in allTEs:
                    return False
            return True

        def inputSequenceExists(thisSequence):
            
            listOfSequenceInputs = [thisSequence["input"]] if isinstance(thisSequence["input"],str) else thisSequence["input"] 
            if len(listOfSequenceInputs)<2: 
                listOfSequenceInputs = [listOfSequenceInputs] 
            for thisSequenceInputs in listOfSequenceInputs: 
                print thisSequenceInputs
                if thisSequenceInputs == [""] or thisSequenceInputs[0].isupper() or inputsAreTEs(thisSequenceInputs): 
                    return True 

                inputsExist = [] 
                for thisSequenceInput in thisSequenceInputs:                             
                    for sequence in theChainDef.sequenceList: 
                        if sequence["output"]==thisSequenceInput: 
                            if inputSequenceExists(sequence): 
                                inputsExist += [True] 
                            else: 
                                log.error("Input %s of sequence %s not found." % (thisSequenceInput, str(thisSequence))) 
                                inputsExist += [False] 

         
            if len(inputsExist)==0:
                return False
            else: 
                return reduce(operator.and_, inputsExist)

            
        for TE in allTEs:
            for sequence in theChainDef.sequenceList:
                if sequence["output"]==TE:
                    inputSequenceExists(sequence)

        return True

    def dumpSignatureList(self, l1_items, fname):

        def dumpIt(fp, sigs, slicename):
            log.info("SignatureList %s" % sigs)
            fp.write('%sSliceFlags.signatures = [\n' % slicename)
            for s in sigs:
                fp.write("    '%s', \n" % s)
            fp.write('    ]\n')
        f = open(fname, 'w')
        f.write('Items = [\n')
        for i in l1_items:
            f.write("'%s', \n" % i)
        f.write('    ]\n')
        dumpIt(f, JetSliceFlags.signatures(), 'Jet')
        dumpIt(f, BjetSliceFlags.signatures(), 'Bjet')
        dumpIt(f, MuonSliceFlags.signatures(), 'Muon')
        dumpIt(f, BphysicsSliceFlags.signatures(), 'Bphysics')
        dumpIt(f, EgammaSliceFlags.signatures(), 'Egamma')
        dumpIt(f, METSliceFlags.signatures(), 'MET')
        dumpIt(f, TauSliceFlags.signatures(), 'Tau')
        dumpIt(f, AFPSliceFlags.signatures(), 'AFP')
        dumpIt(f, MinBiasSliceFlags.signatures(), 'MinBias')
        dumpIt(f, HeavyIonSliceFlags.signatures(), 'HeavyIon')
        dumpIt(f, CosmicSliceFlags.signatures(), 'Cosmic')
        dumpIt(f, CalibSliceFlags.signatures(), 'Calibration')
        dumpIt(f, StreamingSliceFlags.signatures(), 'Streaming')
        dumpIt(f, MonitorSliceFlags.signatures(), 'Monitoring')
        dumpIt(f, BeamspotSliceFlags.signatures(), 'Beamspot')
        dumpIt(f, EnhancedBiasSliceFlags.signatures(), 'EnhancedBias')
        dumpIt(f, TestSliceFlags.signatures(), 'Test')
        pass



    def generate(self):
        log.info('GenerateMenu.py:generate ')

        ###########################
        # L1 Topo menu generation #
        ###########################
        if TriggerFlags.doL1Topo():
            if not TriggerFlags.readL1TopoConfigFromXML() and not TriggerFlags.readMenuFromTriggerDb():

                log.info('Generating L1 topo configuration for %s' % TriggerFlags.triggerMenuSetup() )

                from TriggerMenu.TriggerConfigL1Topo import TriggerConfigL1Topo
                self.trigConfL1Topo = TriggerConfigL1Topo( outputFile = TriggerFlags.outputL1TopoConfigFile() )

                # build the menu structure
                self.trigConfL1Topo.generateMenu()        
                log.info('Topo Menu has %i trigger lines' % len(self.trigConfL1Topo.menu) )
                # write xml file
                self.trigConfL1Topo.writeXML()


            elif TriggerFlags.readL1TopoConfigFromXML():

                log.info("Reading L1 topo configuration from '%s'" % TriggerFlags.inputL1TopoConfigFile())

                from TriggerMenu.TriggerConfigL1Topo import TriggerConfigL1Topo
                self.trigConfL1Topo = TriggerConfigL1Topo( inputFile = TriggerFlags.inputL1TopoConfigFile() )

            else:
                log.info("Doing nothing with L1 topo menu configuration...")


        log.info("Trigger xml files L1Topo : in = %s, out = %s (read from XML = %s)" % (TriggerFlags.inputL1TopoConfigFile(), TriggerFlags.outputL1TopoConfigFile(), TriggerFlags.readL1TopoConfigFromXML() ) )
        log.info("Trigger xml files LVL1   : in = %s, out = %s (read from XML = %s)" % (TriggerFlags.inputLVL1configFile(),   TriggerFlags.outputLVL1configFile(), TriggerFlags.readLVL1configFromXML() ) )
        log.info("Trigger xml files HLT    : in = %s, out = %s (read from XML = %s)" % (TriggerFlags.inputHLTconfigFile(),    TriggerFlags.outputHLTconfigFile(), TriggerFlags.readHLTconfigFromXML() ) )

        ######################
        # L1 menu generation #
        ######################
        if not TriggerFlags.readLVL1configFromXML() and not TriggerFlags.readMenuFromTriggerDb():

            log.info('Generating L1 configuration for %s' % TriggerFlags.triggerMenuSetup() )


            from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1
            self.trigConfL1 = TriggerConfigLVL1( outputFile = TriggerFlags.outputLVL1configFile() )        
            
            # build the menu structure
            self.trigConfL1.generateMenu()        
            log.info('Menu has %i items' % len(self.trigConfL1.menu.items) )
            # write xml file
            self.trigConfL1.writeXML()

            

        elif TriggerFlags.readLVL1configFromXML():

            log.info("Reading L1 configuration from '%s'" % TriggerFlags.inputLVL1configFile())

            from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1
            self.trigConfL1 = TriggerConfigLVL1( inputFile = TriggerFlags.inputLVL1configFile() )

        else:
            log.info("Doing nothing with L1 menu configuration...")


        ##################
        #setup of HLT menu
        ##################
        #(HLTPrescales, streamConfig) = self.setupMenu()
        (HLTPrescales) = self.setupMenu()

        #calling TriggerPythonConfig
        self.triggerPythonConfig = TriggerPythonConfig(TriggerFlags.outputHLTconfigFile(),
                                                       self.signaturesOverwritten)
        #Setting trigger menu name
        self.triggerPythonConfig.menuName = TriggerFlags.triggerMenuSetup()
        



        #############################
        # Start HLT menu generation #
        #############################
        chainsInMenu = self.getChainsFromMenu() # get names of chains to be generated
        if hasattr(self, 'trigConfL1'):
            self.checkL1SeedsForChainsFromMenu(chainsInMenu)
            self.checkUnusedL1Items(chainsInMenu)
                      
                            
        # instantiate parser
        theDictFromChainName = DictFromChainName.DictFromChainName()
        
        counter = 0
        for chain in chainsInMenu:
            log.info("Processing chain : %s", chain)
            chainDicts = theDictFromChainName.getChainDict(chain)
            counter += 1 
            chainCounter = counter
            chainDicts['chainCounter'] = chainCounter

            chainDicts['topoThreshold'] = None
            # only when we generate L1 menu we have trigConfL1 available
            if not (TriggerFlags.readHLTconfigFromXML() or TriggerFlags.readMenuFromTriggerDb()):
                if chainDicts['topoStartFrom'] == True:
                    L1item = chainDicts['L1item']
                    for item in self.trigConfL1.menu.items:
                        if str(item.name) == L1item:
                            itemThrNames = item.thresholdNames(include_bgrp=False)
                            myTEs = []
                            for itemThr in itemThrNames: 
                                if ('-' in itemThr): # identifier for L1Topo itesm
                                    myTEs.append(str(itemThr))
                                else:
                                    myTEs = None
                                    
                            chainDicts['topoThreshold'] = myTEs


            chainDef = self.getChainDef(chainDicts)

            #Insert entry for chain counter later
            #For now, just modify it by assigning it automatically
            streams = chain[3]
            groups  = chain[4]
            EBstep = chain[5]
            streamTag = StreamInfo.getStreamTag(streams)

            if not chainDef:
                log.error('No chainDef for chain %s returned. Will ignore and continue.', chain)
                continue                
            log.debug("GenerateMenu chainDef: %s", chainDef)
        
            # generate HLTChains and associated sequences
            if  not self.chainDefIsConsistent(chainDef):
                log.error('ChainDef consistency checks failing for chain %s' % chain) 

                        

            theHLTChain = self.generateHLTChain(chainDef, str(chainDicts['chainCounter']), streamTag, groups, EBstep)
            theHLTSequences = self.generateHLTSequences(chainDef) #replace this function and make separate constructor in HLTSequence

            # add / register HLTChains / sequences in TriggerPythonCofig           
            for theHLTSequence in theHLTSequences:
                self.triggerPythonConfig.addHLTSequence(theHLTSequence)

            # chain_name is a key in allChains    
            self.triggerPythonConfig.registerHLTChain(chain[0], theHLTChain)          
            if not (TriggerFlags.readHLTconfigFromXML() or TriggerFlags.readMenuFromTriggerDb()):
                self.triggerPythonConfig.addHLTChain(theHLTChain)

        self.triggerPythonConfig.printIt()

        #----------DO NOT MOVE THESE LINES FROM THIS POSITION-----------------
        # Even if XML doesn't need to be generated, chains have to be loaded
        #---------------------------------------------------------------------
        if TriggerFlags.readHLTconfigFromXML() or TriggerFlags.readMenuFromTriggerDb():
            log.info('generation of HLT XML file not requested')
            return

        #-----------------------------------
        # info print out
        #-----------------------------------
        log.info('generating XML configuration files (true?): ')
        if self.trigConfL1.inputFile!=None:
            log.info('LVL1: %s (not generated but read in)', self.trigConfL1.inputFile)
        else:
            log.info('LVL1: %s', self.trigConfL1.outputFile)
        log.info('HLT : %s', self.triggerPythonConfig.getHLTConfigFile())
        log.info('N L1 items     : %s ', len(self.trigConfL1.menu.items) )
        log.info('N L1 thresholds: %s ', len(self.trigConfL1.menu.thresholds) )
        log.info("N HLT chains: %s ", len(self.triggerPythonConfig.allChains.keys()))
        log.debug("All chains: %s ", self.triggerPythonConfig.allChains.keys())       
        
        #log.info('checkTriggerGroupAssignment')
        #checkTriggerGroupAssignment(self.triggerPythonConfig)

        log.info('checkGroups')
        checkGroups(self.triggerPythonConfig)

        #cpsMenus = ['Physics_pp_v5','Physics_pp_v6','Physics_pp_v7']
        ##if TriggerFlags.triggerMenuSetup() in cpsMenus:
        if TriggerFlags.triggerMenuSetup().find("pp_v")>=0:            
            log.info('Assigning CPS groups now')
            addCPS(self.triggerPythonConfig,self.signaturesOverwritten)

        applyPrescales(self.triggerPythonConfig, HLTPrescales)

        # Modifying menu in TriggerPythonConfig
        # is not being executed
        global _func_to_modify_the_menu
        if _func_to_modify_the_menu != None:
            log.info('generate: Modifying the trigger menu in TriggerPythonConfig with %s' % \
                     _func_to_modify_the_menu.__name__)
            _func_to_modify_the_menu(self.triggerPythonConfig)


        #dump configuration files
        log.info('generate: dump configuration Files')
        #self.dumpSignatureList(self.trigConfL1.menu.items.itemNames(),'hltsigs.txt')
        self.triggerPythonConfig.writeConfigFiles()
        if log.isEnabledFor(logging.DEBUG): self.triggerPythonConfig.dot(algs=True)

        #checking Stream Consistency
        log.info('checkStreamConsistency')
        checkStreamConsistency(self.triggerPythonConfig)

        # TrigConfConsistencyChecker
        log.info("Running TrigConfConsistencyChecker...")

        # Check for the Lvl1 XML first in the current dir, then in $XMLPATH
        (ret, output) = commands.getstatusoutput('TrigConfConsistencyChecker --lvl1xml %s --hltxml %s --exceptions  %s '% \
                                                 ( self.trigConfL1.inputFile if self.trigConfL1.inputFile!=None else self.trigConfL1.outputFile,
                                                   self.triggerPythonConfig.getHLTConfigFile(),
                                                   "TriggerMenu/menu_check_exceptions.xml") )
        print output
  
        # this does test the triggertype (JS)
        #for bit in xrange(8):
        #    print "TriggerType",bit,":",self.triggerPythonConfig.Lvl1ItemByTriggerType(0x01<<bit,0x01<<bit)

        if ret==0:
            log.info("TrigConfConsistencyChecker successful.")
        else:
            log.warning("TrigConfConsistencyChecker failed.")

        for line in output.split(os.linesep):
            if line.find('Warning Cannot find threshold') >= 0:
                log.error('%s -> add the threshold explicitly' % line.split()[-1])

        # print all chain counters
        if log.isEnabledFor(logging.DEBUG):
            for name, chains in self.triggerPythonConfig.allChains.iteritems():
                for c in chains:
                    log.debug('Registered chain  %04d %s',
                              int(c.chain_counter), c.chain_name)

        # print all the chains that returned an ERROR chainDef object
        if self.listOfErrorChainDefs:
            log.error ('ATTENTION: The following chains returned an ERROR chainDef object.')
            log.error ('           They will not be considered in the menu generation:')
            for chain in self.listOfErrorChainDefs:
                log.error('              chain: %s   ' %chain)
            
        log.info ('Check the List of chains in rerun with a special stream tag')

        self.GetStreamTagForRerunChains()

        log.info('generate END')


    def GetStreamTagForRerunChains(self):        
        if TriggerFlags.triggerMenuSetup().find("Physics_pp_v")==-1:
            log.info('do not retrieve list of stream tags for rerun chain for non-Physics menu ')        
            return []

        log.info('retrieve list of stream tags for rerun chain ')        
        list= getStreamTagForRerunChains(self.triggerPythonConfig, self.HLTPrescales)
        if not list:
            log.warning('no rerun chain with special stream')

        return list
        

