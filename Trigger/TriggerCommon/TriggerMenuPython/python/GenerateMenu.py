# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# flag
from TriggerJobOpts.TriggerFlags            import TriggerFlags
from TriggerMenuPython.EgammaSliceFlags     import EgammaSliceFlags
from TriggerMenuPython.TauSliceFlags        import TauSliceFlags
from TriggerMenuPython.JetSliceFlags        import JetSliceFlags
from TriggerMenuPython.METSliceFlags        import METSliceFlags
from TriggerMenuPython.MuonSliceFlags       import MuonSliceFlags
from TriggerMenuPython.BphysicsSliceFlags   import BphysicsSliceFlags
from TriggerMenuPython.BjetSliceFlags       import BjetSliceFlags
from TriggerMenuPython.CombinedSliceFlags   import CombinedSliceFlags
from TriggerMenuPython.MinBiasSliceFlags    import MinBiasSliceFlags
from TriggerMenuPython.BeamSpotSliceFlags   import BeamSpotSliceFlags
from TriggerMenuPython.CalibSliceFlags      import CalibSliceFlags
from TriggerMenuPython.CosmicSliceFlags     import CosmicSliceFlags
from TriggerMenuPython.L1CaloSliceFlags     import L1CaloSliceFlags
from TriggerMenuPython.HeavyIonSliceFlags   import HeavyIonSliceFlags
from TriggerMenuPython.StreamingSliceFlags  import StreamingSliceFlags
from TriggerMenuPython.PerL1ItemStreamingSliceFlags  import PerL1ItemStreamingSliceFlags
from TriggerMenuPython.Lvl1Flags            import Lvl1Flags
from TriggerMenuPython.GenericSliceFlags    import GenericSliceFlags
from TriggerMenuPython.MonitorSliceFlags    import MonitorSliceFlags

# lvl1
from TriggerMenuPython.Lvl1                 import Lvl1
from TriggerMenuPython.Bphysics_Lvl1        import Bphysics_Lvl1

# hlt
from TriggerMenuPython.TriggerPythonConfig  import TriggerPythonConfig

from TriggerMenuPython.Express              import addExpressStream
from TriggerMenuPython.CPS                  import addCPS
from TriggerMenuPython.Lumi                 import lumi, applyPrescales
from TriggerMenuPython.MenuUtil             import setL1TTStream7Bit, applyLVL1Prescale, assignTriggerGroups, \
     checkTriggerGroupAssignment, overwriteStreamTags, allSignatures, overwriteStreamTag, checkStreamConsistency
from TriggerMenuPython.MenuUtil             import getJetWeights
from TriggerMenuPython.MenuUtil             import remapL1Thresholds, remapL1Items

from AthenaCommon.Logging import logging
log = logging.getLogger( 'GenerateMenu.py' )
# log.setLevel(logging.DEBUG)

def reportSliceImportError(flags):
    if flags.signatures() == None or len(flags.signatures()) == 0:
        log.warning("Error in configuration of "+flags.__class__.__name__+" but it is not required so we can continue")
    else:
        log.error("Error in configuration of "+flags.__class__.__name__)
        raise

_func_to_modify_the_menu = None
_func_to_modify_signatures = None

class GenerateMenu:
    def overwriteSignaturesWith(f):
        global _func_to_modify_signatures
        if _func_to_modify_signatures != None:
            log.warning('Updating the function to modify signatures from %s to %s'\
                  % (_func_to_modify_signatures.__name__, f.__name__))
        _func_to_modify_signatures = f
    def overwriteMenuWith(f):
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

    def loadChains(self):
        
        #---------------------------------------------------------------------
        try:

            from TriggerMenuPython.EMObject           import EMObjects
            
            from TriggerMenuPython.ElectronChains     import Electrons

            #PJB rearrange order - load photon chains so can be used in multielectron! (for uspi chains!)
            from TriggerMenuPython.PhotonDef          import Photons
            self.chains += Photons

            from TriggerMenuPython.ElectronDef_v2     import Electrons_v2
            self.chains += Electrons
            self.chains += Electrons_v2

            from TriggerMenuPython.MultiElectron      import MultiElectrons_IdScanDef
            self.chains += MultiElectrons_IdScanDef
            
            from TriggerMenuPython.MultiPhoton        import MultiPhotons
            from TriggerMenuPython.UnseededEgamma     import UnseededPhotons 
            
            self.chains += EMObjects
            self.chains += MultiPhotons
            self.chains += UnseededPhotons
        except:
            reportSliceImportError(EgammaSliceFlags)

        #---------------------------------------------------------------------

        try:

            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MuonHIV1Menus() :
                from TriggerMenuPython.MuonHIV1Chains import Muons
            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MuonHIMenus() :
                from TriggerMenuPython.MuonHIChains import Muons
            else:
                if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
                       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
                       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus() or \
                       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
                    from TriggerMenuPython.MuonPhysicsppChains_v2v3 import Muons
                else:                 
                    from TriggerMenuPython.MuonPhysicsppChains import Muons            
                
            self.chains += Muons
        except:
            reportSliceImportError(MuonSliceFlags)

        #---------------------------------------------------------------------
        try:
            from TriggerMenuPython.Stau               import Staus
            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MuonHIV1Menus() :
                from TriggerMenuPython.MuonHIV1Chains import MultiMuons
                Staus=[]
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.MuonHIMenus() :
                from TriggerMenuPython.MuonHIChains import MultiMuons
                Staus=[]
            else:
                from TriggerMenuPython.MultiMuonPhysicsppChains import MultiMuons
            
            self.chains += MultiMuons
            self.chains += Staus
        except:
            reportSliceImportError(MuonSliceFlags)

        #---------------------------------------------------------------------
        try:
              from TriggerMenuPython.TauDef   import Taus
              from TriggerMenuPython.MultiTau import MultiTaus
              self.chains += Taus
              self.chains += MultiTaus

        except:
            reportSliceImportError(TauSliceFlags)

        #---------------------------------------------------------------------
        try:
            ##if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
            ##   TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
            ##   TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
            ##    from TriggerMenuPython.MissingETDef_v2v3       import MissingETs
            ##else:
            ##    from TriggerMenuPython.MissingETDef       import MissingETs

            from TriggerMenuPython.MissingETDef       import MissingETs
            self.chains += MissingETs
             
                
        except:
            reportSliceImportError(METSliceFlags) 

        #---------------------------------------------------------------------

        try:

            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
               TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
               TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
##               TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
                from TriggerMenuPython.JetDef_v2v3 import Jets
                from TriggerMenuPython.MultiJet_v2v3   import MultiJets
            else:
                from TriggerMenuPython.JetDef   import Jets
                
            self.chains += Jets

            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
                   TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
                   TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
##                   TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():            
                self.chains += MultiJets

        except:
            reportSliceImportError(JetSliceFlags)
            
        #---------------------------------------------------------------------
        try:
            from TriggerMenuPython.BjetDef            import Bjets
            from TriggerMenuPython.MultiBjet          import MultiBjets
            
            self.chains += Bjets
            self.chains += MultiBjets

        except:
            reportSliceImportError(BjetSliceFlags)

        #---------------------------------------------------------------------
        try:
            from TriggerMenuPython.BphysicsDef        import Bphysics
            self.chains += Bphysics
        except:
            reportSliceImportError(BphysicsSliceFlags)

        #---------------------------------------------------------------------
        try:
            from TriggerMenuPython.MinBiasDef         import MinBias
            self.chains += MinBias
        except:
            reportSliceImportError(MinBiasSliceFlags) 

        #---------------------------------------------------------------------
        try:
            from TriggerMenuPython.BeamSpot import BeamSpots
            self.chains += BeamSpots
        except:
            reportSliceImportError(BeamSpotSliceFlags) 

        #---------------------------------------------------------------------
        try:
            from TriggerMenuPython.TrkCalibDef import TrkCalibItems
            from TriggerMenuPython.LArCalib import LArCalibChains
            from TriggerMenuPython.TileCalibDef import TileCalib
            
            if TriggerFlags.readLVL1configFromXML() and \
                   self.triggerPythonConfig.l1menuFromXML:
                items = self.triggerPythonConfig.l1menuFromXML.getL1Items()
                tau_items = filter(lambda x: x.find('TAU')>=0, \
                                   map(lambda x: x['name'], items))
                tau_items = ','.join(tau_items)
                runOnAllL1Tau = ('L2_trk9_Central_Tau_IDCalib',
                                 'L2_trk9_Fwd_Tau_IDCalib',
                                 'L2_trk16_Central_Tau_IDCalib',
                                 'L2_trk16_Fwd_Tau_IDCalib',
                                 'L2_trk29_Central_Tau_IDCalib',
                                 'L2_trk29_Fwd_Tau_IDCalib',
                                 )
                for c in TrkCalibItems:
                    if c.l2chain and c.l2chain.chain_name in runOnAllL1Tau:
                        c.l2chain.lower_chain_name = tau_items

            self.chains += TrkCalibItems
            self.chains += TileCalib
            self.chains += LArCalibChains

            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
               TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
               TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
                from TriggerMenuPython.LArCalib import LArCalibJetEMScaleChains
                self.chains += LArCalibJetEMScaleChains
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
                from TriggerMenuPython.LArCalib import LArCalibJetHIChains
                self.chains += LArCalibJetHIChains
            else:
                from TriggerMenuPython.LArCalib import LArCalibJetChains
                self.chains += LArCalibJetChains
            
        except:
            reportSliceImportError(CalibSliceFlags)

        #--------------------- Here we import from CosmicDef.py
        try:
            from TriggerMenuPython.CosmicDef import Cosmics as CosmicsNew
            self.chains += CosmicsNew
        except:
            reportSliceImportError(CosmicSliceFlags)

        #--------------------- Here we import from L1CaloDef.py
        try:
            from TriggerMenuPython.L1CaloDef import L1CaloCalibChains
            self.chains += L1CaloCalibChains
        except:
            reportSliceImportError(L1CaloSliceFlags)

        #-------------------- HeavyIon specific chains
        try:
            from TriggerMenuPython.HeavyIonDef import HeavyIonChains
            self.chains += HeavyIonChains
        except:
            reportSliceImportError(HeavyIonSliceFlags)

        #-------------------- Streams and filters:
        try:
            from TriggerMenuPython.StreamingDef import SeededStreamerMaker, ExseededStreamingChains
            self.chains += ExseededStreamingChains
            SeededStreamerMaker(self.triggerPythonConfig)
            self.chains += SeededStreamerMaker._chains
        except:
            reportSliceImportError(StreamingSliceFlags)
        #-------------------- Per L1 item streaming
        try:
            from TriggerMenuPython.PerL1ItemStreamingDef import L1ItemStreamerMaker
            L1ItemStreamerMaker(self.triggerPythonConfig)
            self.chains += L1ItemStreamerMaker._chains
        except:
            reportSliceImportError(PerL1ItemStreamingSliceFlags)
        

        try:
            from TriggerMenuPython.Combined import PhotonElectrons, PhotonBjets
            from TriggerMenuPython.Combined import TauMuons_tauSiTrk
            from TriggerMenuPython.Combined import TauElectrons_eIdScan
            from TriggerMenuPython.Combined import JetMETs
            from TriggerMenuPython.Combined import BjetMETs
            from TriggerMenuPython.Combined import JetBjets
            from TriggerMenuPython.Combined import mymujets
            from TriggerMenuPython.Combined import MuonJPsi
                                  
            
            self.chains += PhotonBjets
            self.chains += PhotonElectrons
            self.chains += TauElectrons_eIdScan
            self.chains += JetMETs
            self.chains += BjetMETs
            self.chains += JetBjets
            self.chains += mymujets
            self.chains += MuonJPsi
            
                 
            # EM scale jets and MET with noMu (v2 and v3 menus)
            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
                   TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
                   TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
                TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():

                from TriggerMenuPython.Combined import PhotonMETs_v2v3
                from TriggerMenuPython.Combined import ElectronMETs_eIdScan_v2v3
                from TriggerMenuPython.Combined import TauMETs_v2v3, TauMET2s_v2v3
                from TriggerMenuPython.Combined import TauMET_IdScan_v2v3, TauMET2_IdScan_v2v3
                
                from TriggerMenuPython.Combined import JetEMScaleMETs, JetEMScaleGaps, TauJetsEMScale, ElectronJetsEMScale
                from TriggerMenuPython.Combined import TripleSignatures_EMScale
                from TriggerMenuPython.Combined import JetBjetsEMScale

                self.chains += PhotonMETs_v2v3
                self.chains += ElectronMETs_eIdScan_v2v3
                self.chains += TauMETs_v2v3
                self.chains += TauMET2s_v2v3
                self.chains += TauMET_IdScan_v2v3
                self.chains += TauMET2_IdScan_v2v3

                self.chains += ElectronJetsEMScale
                self.chains += JetEMScaleGaps
                self.chains += JetEMScaleMETs
                self.chains += TauJetsEMScale
                self.chains += TripleSignatures_EMScale
                self.chains += JetBjetsEMScale
            
                if TriggerFlags.triggerMenuSetup() not in Lvl1Flags.NoTRTMuonMenus() :
                    from TriggerMenuPython.Combined import MuonJetsEMScale
                    from TriggerMenuPython.Combined import BjetMuonJets
                    from TriggerMenuPython.Combined import MuonMETs_v2v3
                    self.chains += MuonJetsEMScale
                    self.chains += BjetMuonJets
                    self.chains += MuonMETs_v2v3

            else:
                from TriggerMenuPython.Combined import PhotonMETs, ElectronMETs_eIdScan
                from TriggerMenuPython.Combined import TauMETs, TauMET2s, TauMET_IdScan, TauMET2_IdScan
                from TriggerMenuPython.Combined import ElectronJET_tcem
                from TriggerMenuPython.Combined import ElectronJET_tchad
  
                self.chains += PhotonMETs
                self.chains += ElectronMETs_eIdScan
                self.chains += TauMETs
                self.chains += TauMET2s
                self.chains += TauMET_IdScan
                self.chains += TauMET2_IdScan
                self.chains += ElectronJET_tcem
                self.chains += ElectronJET_tchad
            
                if TriggerFlags.triggerMenuSetup() not in Lvl1Flags.NoTRTMuonMenus() :
                    from TriggerMenuPython.Combined import MuonJets
                    from TriggerMenuPython.Combined import MuonMETs
                    self.chains += MuonJets
                    self.chains += MuonMETs

            # Combined items without TRT muons
            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoTRTMuonMenus() :
                from TriggerMenuPython.Combined import BjetMuonJets_noTRTMuons
                self.chains += BjetMuonJets_noTRTMuons

            else:
                from TriggerMenuPython.Combined import TauMuons
                from TriggerMenuPython.Combined import PhotonMuons, MuonPhotons
                from TriggerMenuPython.Combined import TauMuons_tauSiTrk
                from TriggerMenuPython.Combined import ElectronMuons_eIdScan
                from TriggerMenuPython.Combined import MuonEles
                #from TriggerMenuPython.Combined import BjetMuonJets
                from TriggerMenuPython.Combined import  TauMu_VBF
                from TriggerMenuPython.Combined import  TauEl_VBF
                from TriggerMenuPython.Combined import  TauTau_VBF

                self.chains += TauMuons
                self.chains += TauMuons_tauSiTrk
                self.chains += PhotonMuons
                self.chains += MuonPhotons
                self.chains += ElectronMuons_eIdScan
                self.chains += MuonEles
                #self.chains += BjetMuonJets
                self.chains += TauMu_VBF
                self.chains += TauEl_VBF
                self.chains += TauTau_VBF

        except:
            reportSliceImportError(CombinedSliceFlags)
        #---------------------------------------------------------------------            
        try:
            from TriggerMenuPython.GenericDef import Generic
            self.chains += Generic
        except:
            reportSliceImportError(GenericSliceFlags)
        #---------------------------------------------------------------------            
        try:
            from TriggerMenuPython.MonitorDef import MonitorChainList
            self.chains += MonitorChainList
        except:
            reportSliceImportError(MonitorSliceFlags)
        #---------------------------------------------------------------------

    def generateLVL1Config(self):
        Lvl1.registerItems(self.triggerPythonConfig)

        if Lvl1Flags.CtpIdMap()!={}:
            log.debug('Remapping ctpids as defined in the menu')
            lvl1_items=Lvl1Flags.items()
            for item in lvl1_items:
                if item in Lvl1Flags.CtpIdMap():
                    log.debug( item)
                    item2 = self.triggerPythonConfig.getLvl1Item(item)
                    item2.setCtpid(Lvl1Flags.CtpIdMap()[item])
                    

        if (Lvl1Flags.CustomThresholdMap()!={} and TriggerFlags.Lvl1.RemapThresholdsAsListed()!=True):
            log.debug('Remapping thresholds as defined in the menu')
            for thr in Lvl1Flags.thresholds():
                if thr in Lvl1Flags.CustomThresholdMap():
                    thr2 = self.triggerPythonConfig.getLvl1Threshold(thr)
                    log.info("Remapping "+thr+" from "+str(thr2.mapping)+" to "+str(Lvl1Flags.CustomThresholdMap()[thr]))
                    thr2.mapping = Lvl1Flags.CustomThresholdMap()[thr]
                    # Refreshing cabling
                    (slot, connector, bitnum, range_begin, range_end) = self.triggerPythonConfig.cableInput(thr2.ttype, thr2.mapping)
                    thr2.range_begin = range_begin
                    thr2.range_end = range_end
                    thr2.slot = slot
                    thr2.connector = connector

    def generateHLTConfig(self):
        
        log.info('Doing consistency checks between chains defined in signatures flags and accessible HLT configurations')
        log.debug('Creating list of all available slice signatures flags')

        def collect(flags):
            if flags.signatures.statusOn:
                return flags.signatures()
            return []
        
        slice_signatures = []
        slice_signatures.extend(collect(EgammaSliceFlags))
        slice_signatures.extend(collect(TauSliceFlags))
        slice_signatures.extend(collect(JetSliceFlags))
        slice_signatures.extend(collect(METSliceFlags))
        slice_signatures.extend(collect(MuonSliceFlags))
        slice_signatures.extend(collect(BphysicsSliceFlags))
        slice_signatures.extend(collect(BjetSliceFlags))
        slice_signatures.extend(collect(CombinedSliceFlags))
        slice_signatures.extend(collect(MinBiasSliceFlags))
        slice_signatures.extend(collect(BeamSpotSliceFlags))
        slice_signatures.extend(collect(CosmicSliceFlags))
        slice_signatures.extend(collect(L1CaloSliceFlags))
        slice_signatures.extend(collect(HeavyIonSliceFlags))
        slice_signatures.extend(collect(StreamingSliceFlags))
        slice_signatures.extend(collect(CalibSliceFlags))
        slice_signatures.extend(collect(GenericSliceFlags))
        slice_signatures.extend(collect(MonitorSliceFlags))
        
        log.debug('All signatures : '+str(slice_signatures))
        log.debug('Creating list of available configurations')
        
        configs = [ x.sig_id for x in self.chains ]
        log.debug('All possible configurations: '+str(configs))

        problem=False
        for i in slice_signatures:
            if i not in configs:
                log.warning('Slice signature flag: '+i+' defined but no corresponding configuration exists')
                proble=True

        if problem:
            log.warning('Missing chain definitions, see above')
            
        # pregenerate chains
        log.info('generation of chains config ...')
        for chaindef in self.chains:
            log.debug("generating: %s" % chaindef.sig_id )
            chaindef.generateMenu(self.triggerPythonConfig)

        log.info('generation of chains config ... finished')
        pass
    
    def setupMenu(self):
        # go over the slices and put together big list of signatures requested
        log.info('modifying menu according to the luminosity and prescaling setup')
        lvl1_items = []
        lvl1_thresholds = []
        sigs = []

        (L1Prescales, HLTPrescales, streamConfig, triggerGroups, higherGroups) = lumi(self.triggerPythonConfig)
        global _func_to_modify_signatures
        if _func_to_modify_signatures != None:
            log.info('Modifying trigger signatures in TriggerFlags with %s' % \
                     _func_to_modify_signatures.__name__)
            log.info('start')
            _func_to_modify_signatures()
            log.info('stop')

        if Lvl1Flags.items!=None and Lvl1Flags.items():
            lvl1_items.extend(Lvl1Flags.items())
        lvl1_thresholds.extend(Lvl1Flags.thresholds())

#        if Lvl1Flags.CtpIdMap()!={}:
#            log.info('Remapping ctpids as defined in the menu')
#            for item in lvl1_items:
##                if item in Lvl1Flags.CtpIdMap():
#                    log.info( item)
#                    item2 = TriggerPythonConfig.getLvl1Item(item)
#                    item2.setCtpid(ctpmap[item])


        # make one big list of enabled signatures
        if EgammaSliceFlags.signatures():   # this protection for None rather than some list --- it can happen if slice is disabled
            sigs += EgammaSliceFlags.signatures()

        if MuonSliceFlags.signatures():
            sigs += MuonSliceFlags.signatures()

        if TauSliceFlags.signatures():
            sigs += TauSliceFlags.signatures()

        if JetSliceFlags.signatures():
            sigs += JetSliceFlags.signatures()

        if BjetSliceFlags.signatures():
            sigs += BjetSliceFlags.signatures()

        if BphysicsSliceFlags.signatures():
            sigs += BphysicsSliceFlags.signatures()

        if METSliceFlags.signatures():
            sigs += METSliceFlags.signatures()

        log.debug( 'Enabled before comb signatures: '+str(sigs) )
        if CombinedSliceFlags.signatures():
            sigs += CombinedSliceFlags.signatures()
        log.debug( 'Enabled after comb signatures: '+str(sigs) )

        if MinBiasSliceFlags.signatures():
            sigs += MinBiasSliceFlags.signatures()

        if BeamSpotSliceFlags.signatures():
            sigs += BeamSpotSliceFlags.signatures()

        if CalibSliceFlags.signatures():
            sigs += CalibSliceFlags.signatures()

        if CosmicSliceFlags.signatures():
            sigs += CosmicSliceFlags.signatures()

        if L1CaloSliceFlags.signatures():
            sigs += L1CaloSliceFlags.signatures()

        if HeavyIonSliceFlags.signatures():
            sigs += HeavyIonSliceFlags.signatures()
            
        if StreamingSliceFlags.signatures():
            sigs += StreamingSliceFlags.signatures()

        if PerL1ItemStreamingSliceFlags.signatures():
            sigs += PerL1ItemStreamingSliceFlags.signatures()

        if GenericSliceFlags.signatures():
            sigs += GenericSliceFlags.signatures()

        if MonitorSliceFlags.signatures():
            sigs += MonitorSliceFlags.signatures()

        if L1CaloSliceFlags.signatures():
            sigs += L1CaloSliceFlags.signatures()            
            
        self.createCosmicDoChains()

        log.debug( 'Enabled signatures: '+str(sigs) )
        return (lvl1_thresholds, lvl1_items, sigs,
                L1Prescales, HLTPrescales, streamConfig, triggerGroups, higherGroups)

    def dumpSignatureList(self, l1_items, sigs, fname):
        def dumpIt(fp, sigs, slicename):
            fp.write('%sSliceFlags.signatures = [\n' % slicename)
            for s in sigs:
                fp.write("    '%s', \n" % s)
            fp.write('    ]\n')
        f = open(fname, 'w')
        f.write('Items = [\n')
        for i in l1_items:
            f.write("'%s', \n" % i)
        f.write('    ]\n')
        dumpIt(f, EgammaSliceFlags.signatures(), 'Egamma')
        dumpIt(f, MuonSliceFlags.signatures(), 'Muon')
        dumpIt(f, TauSliceFlags.signatures(), 'Tau')
        dumpIt(f, JetSliceFlags.signatures(), 'Jet')
        dumpIt(f, BjetSliceFlags.signatures(), 'Bjet')
        dumpIt(f, METSliceFlags.signatures(), 'MET')
        dumpIt(f, MinBiasSliceFlags.signatures(), 'MinBias')
        dumpIt(f, BeamSpotSliceFlags.signatures(), 'BeamSpot')
        dumpIt(f, CalibSliceFlags.signatures(), 'Calib')
        dumpIt(f, CosmicSliceFlags.signatures(), 'Cosmic')
        dumpIt(f, HeavyIonSliceFlags.signatures(), 'HeavyIon')
        dumpIt(f, StreamingSliceFlags.signatures(), 'Streaming')
        dumpIt(f, GenericSliceFlags.signatures(), 'Generic')
        dumpIt(f, MonitorSliceFlags.signatures(), 'Monitor')
        pass
    
    def createCosmicDoChains(self):
        if TriggerFlags.CosmicSlice.signatures():
            for c in TriggerFlags.CosmicSlice.signatures():
                exec 'TriggerFlags.CosmicSlice.do%s = True' % c

    def chainCounterAvailability(self, clist):
        s = ''
        clist.sort()
        line = ''
        for i in range(1024):
            tmp = '----'
            if i not in clist: tmp = '%4d' % i
            line = '%s %s' % (line, tmp)
            if ( (i+1) % 16) == 0:
                s += line + '\n'
                line = ''
        return s
    
    def generate(self):        
        (lvl1_thresholds, lvl1_items, sigs, \
         L1Prescales, HLTPrescales, streamConfig, triggerGroups, higherGroups) = self.setupMenu()

        self.triggerPythonConfig = TriggerPythonConfig(TriggerFlags.outputHLTconfigFile(),
                                                       TriggerFlags.outputLVL1configFile())
        self.triggerPythonConfig.menuName = TriggerFlags.triggerMenuSetup()


        # Generate configuration for the L1 menu
        self.generateLVL1Config() # Needs TriggerPythonConfig object
        if not TriggerFlags.readLVL1configFromXML() and not TriggerFlags.readMenuFromTriggerDb():
            Lvl1.generateMenu(self.triggerPythonConfig, \
                              lvl1_items, lvl1_thresholds)
            jw = getJetWeights(self.triggerPythonConfig)
            log.debug('Calculated jet weights are %s' % str(jw))
            self.triggerPythonConfig.Lvl1CaloInfo().setJetWeights(jw)

        if TriggerFlags.readLVL1configFromXML():
            Lvl1.setThresholdTypesFromXML(self.triggerPythonConfig,TriggerFlags.inputLVL1configFile())


        if not TriggerFlags.doLVL2() and not TriggerFlags.doEF():
            log.debug('Both LVL2 and EF are switched off.')
            log.debug('No menu generation required for HLT')
            if TriggerFlags.doLVL1() and not TriggerFlags.readLVL1configFromXML() and not TriggerFlags.readMenuFromTriggerDb():
                log.debug('LVL1 is on, so generation only the L1 XML file')
                applyPrescales(self.triggerPythonConfig, L1Prescales, None)

                self.triggerPythonConfig.writeConfigFiles("l1")
                #self.triggerPythonConfig.dot(algs=True)
            return

        # Even if XML doesn't need to be generated, chains have to be loaded
        self.loadChains()
        if ((TriggerFlags.readHLTconfigFromXML() and TriggerFlags.readLVL1configFromXML())
            or TriggerFlags.readMenuFromTriggerDb()):
            log.debug('XML files generation not requested')
            return

        # Generate configuration for the HLT menu
        
        self.generateHLTConfig() # Needs TriggerPythonConfig object

        # (*) Moving applyPrescales to after setting up the express stream
        # Only keeping the streamConfig here.
        log.info( 'OverwriteStreamTags')
        overwriteStreamTags(self.triggerPythonConfig, streamConfig)
        
        log.debug('assign TriggerGroups')
        assignTriggerGroups(self.triggerPythonConfig, triggerGroups, higherGroups)

        # Remap L1 thresholds and items
        remapL1Thresholds(self.triggerPythonConfig, Lvl1Flags.ThresholdMap())
        remapL1Items     (self.triggerPythonConfig, Lvl1Flags.ItemMap())
        
        log.info('generating XML configuration files')
        log.info('LVL1: '+str(self.triggerPythonConfig.getL1ConfigFile()))
        log.info('HLT : '+str(self.triggerPythonConfig.getHLTConfigFile()))
        log.info('N L1 items     : %s (/256)' % len(lvl1_items) )
        log.info('N L1 thresholds: %s (/63)' % len(lvl1_thresholds) )
        log.debug('L1 items: ' % Lvl1Flags.items())
        log.debug('L1 thresholds: ' % Lvl1Flags.thresholds())

        # note only this chains enter current config
        # print self.triggerPythonConfig.allChains

        for name, chains in self.triggerPythonConfig.allChains.iteritems():
            if name in sigs:
                log.debug("placing in the configuration chains for: %s" % name )
                # [self.triggerPythonConfig.addHLTChain(c) for c in chains]
                for c in chains:
                    if c.level == 'L2':
                        names = map(lambda x: x.chain_name, self.triggerPythonConfig.theL2HLTChains)
                        if c.chain_name not in names:
                            log.debug('add L2 chain: %s <- %s (%d sigs)' % (c.chain_name, c.lower_chain_name, len(c.siglist)))
                            self.triggerPythonConfig.addHLTChain(c)
                        else:
                            log.debug('not adding %s' % c.chain_name)
                    if TriggerFlags.doEF() and c.level == 'EF':
                        names = map(lambda x: x.chain_name, self.triggerPythonConfig.theEFHLTChains)
                        if c.chain_name not in names:
                            self.triggerPythonConfig.addHLTChain(c)

        checkTriggerGroupAssignment(self.triggerPythonConfig)

        # Overwrite stream tags for SingleBeam menu
        # if TriggerFlags.CosmicSlice.UseSingleBeam7BitL1Menu():
        l1tt_menus = [ 'Cosmic_v1', 'Cosmic_v2', 'Cosmic_v3',
                       'Cosmic2009_v1', 'Cosmic2009_v2','Cosmic2009_simpleL1Calib',
                       'Cosmic2009_inclMuons','InitialBeam_v1', 'MC_InitialBeam_v1', 'MC_InitialBeam_v1_no_prescale',
                       'InitialBeam_v2', 'MC_InitialBeam_v2', 'MC_InitialBeam_v2_no_prescale',
                       'InitialBeam_v3', 'MC_InitialBeam_v3', 'MC_InitialBeam_v3_no_prescale', ]

        if TriggerFlags.triggerMenuSetup() in l1tt_menus:
            for c in self.triggerPythonConfig.theL2HLTChains:
                if 'L1ItemStreamer' in c.chain_name:
                    continue
                setL1TTStream7Bit(c, self.triggerPythonConfig)
            for c in self.triggerPythonConfig.theEFHLTChains:
                if 'L1ItemStreamer' in c.chain_name:
                    continue
                setL1TTStream7Bit(c, self.triggerPythonConfig)
        else:
            for c in self.triggerPythonConfig.theL2HLTChains:
                overwriteStreamTag(c, self.triggerPythonConfig)
            for c in self.triggerPythonConfig.theEFHLTChains:
                overwriteStreamTag(c, self.triggerPythonConfig)
                 
        log.debug('assign express stream info')
        addExpressStream(self.triggerPythonConfig)

        #cps_menus = ['Physics_pp_v1','MC_pp_v1','Physics_pp_v2', 'Physics_pp_v3', 'Physics_pp_v4', 'MC_pp_v2', 'MC_pp_v3', 'MC_pp_v4']
        cps_menus = ['Physics_pp_v4']
        
        if TriggerFlags.triggerMenuSetup() in cps_menus:
            log.debug('assign CPS groups')
            addCPS(self.triggerPythonConfig)

        # (*)
        applyPrescales(self.triggerPythonConfig, L1Prescales, HLTPrescales)

        global _func_to_modify_the_menu
        if _func_to_modify_the_menu != None:
            log.info('Modifying the trigger menu in TriggerPythonConfig with %s' % \
                     _func_to_modify_the_menu.__name__)
            _func_to_modify_the_menu(self.triggerPythonConfig)
        # dump configuration files
        #self.dumpSignatureList(lvl1_items, sigs, 'hltsigs.txt')
        self.triggerPythonConfig.writeConfigFiles()
        self.triggerPythonConfig.dot(algs=True)

        #Betta
        checkStreamConsistency(self.triggerPythonConfig,streamConfig)


        import os
        import commands
        from AthenaCommon.Utils.unixtools import FindFile
        log.info("Running TrigConfConsistencyChecker...")

        # Check for the Lvl1 XML first in the current dir, then in $XMLPATH
        #path = ["./"] + os.environ.get("XMLPATH","").split(":")
        #(ret, output) = commands.getstatusoutput('TrigConfConsistencyChecker --lvl1xml %s --hltxml %s --exceptions %s' % \
        #                                         (FindFile(TriggerFlags.inputLVL1configFile(),path,os.R_OK),  # FindFile doesn't work when the package name is
        #                                                                                                      # missing, but then it doesn't work with local path
        #                                          TriggerFlags.inputHLTconfigFile(),
        #                                          "TriggerMenuPython/menu_check_exceptions.xml"))  
##Z.C(ret, output) = commands.getstatusoutput('TrigConfConsistencyChecker --lvl1xml %s --hltxml %s --exceptions --disable L1CaloThrOrder %s '% \
        (ret, output) = commands.getstatusoutput('TrigConfConsistencyChecker --lvl1xml %s --hltxml %s --exceptions  %s '% \
                                                 ( self.triggerPythonConfig.getL1ConfigFile(),
                                                   self.triggerPythonConfig.getHLTConfigFile(),
                                                   "TriggerMenuPython/menu_check_exceptions.xml") )
        print output

        # this does test the triggertype (JS)
        #for bit in xrange(8):
        #    print "TriggerType",bit,":",self.triggerPythonConfig.Lvl1ItemByTriggerType(0x01<<bit,0x01<<bit)

        if ret==0:
            log.info("TrigConfConsistencyChecker successful.")
        else:
            log.info("TrigConfConsistencyChecker failed.")

        for line in output.split(os.linesep):
            if line.find('Warning Cannot find threshold') >= 0:
                log.error('%s -> add the threshold explicitly' % line.split()[-1])


        # Suggest counters for new chains
        countersL2 = []
        countersEF = []
        for name, chains in self.triggerPythonConfig.allChains.iteritems():
            for c in chains:
                if c.isL2():
                    countersL2.append(int(c.chain_counter))
                if c.isEF():
                    countersEF.append(int(c.chain_counter))
        countersL2.sort()
        countersEF.sort()
        maxL2 = max(countersL2)
        maxEF = max(countersEF)

        if not TriggerFlags.readHLTconfigFromXML() and not TriggerFlags.readMenuFromTriggerDb():
            #log.info("L2 available chain counters:\n" +str([x for x in range(0, 1023) if x not in countersL2]))
            #log.info("EF available chain counters:\n" +str([x for x in range(0, 1023) if x not in countersEF]))
            log.info("L2 available chain counters:\n" +\
                     self.chainCounterAvailability(countersL2))
            log.info("EF available chain counters:\n" +\
                     self.chainCounterAvailability(countersEF))





        physics_menu = ['Physics_pp_v2', 'Physics_pp_v3', 'Physics_pp_v4']
        #physics_menu = ['Physics_pp_v2']
        for ppmenu in physics_menu:
            countersL2_physics = []
            countersEF_physics = []
            if TriggerFlags.triggerMenuSetup() in ppmenu:
#            if TriggerFlags.triggerMenuSetup() in physics_menu:
                for c in self.triggerPythonConfig.theL2HLTChains:
                    countersL2_physics.append(int(c.chain_counter))
                for c in self.triggerPythonConfig.theEFHLTChains:
                    countersEF_physics.append(int(c.chain_counter))
                    countersL2_physics.sort()
                    countersEF_physics.sort()
                    maxL2_physics = max(countersL2_physics)
                    maxEF_physics = max(countersEF_physics)
                
                if not TriggerFlags.readHLTconfigFromXML() and not TriggerFlags.readMenuFromTriggerDb():
                    log.info("L2 available chain counters for " +\
                             ppmenu +\
                             " \n" +\
                             self.chainCounterAvailability(countersL2_physics))
                    log.info("EF available chain counters for " +\
                             ppmenu +\
                             " \n" +\
                             self.chainCounterAvailability(countersEF_physics))


                    #log.info("EF available chain counters for Physics_pp_v2 \n" +\
                    #         self.chainCounterAvailability(countersEF_physics))
                
                



        for name, chains in self.triggerPythonConfig.allChains.iteritems():
            for c in chains:
                level = 'XX'
                if c.isL2(): level = 'L2'
                elif c.isEF(): level = 'EF'
                log.debug('Registered chain %s %04d %s' % \
                          (level, int(c.chain_counter), c.chain_name))
