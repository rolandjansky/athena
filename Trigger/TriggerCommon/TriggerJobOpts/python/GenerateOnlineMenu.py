# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

# flags
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
from TriggerMenuPython.CosmicSliceFlags     import CosmicSliceFlags
from TriggerMenuPython.Lvl1Flags            import Lvl1Flags
from TriggerMenuPython.CosmicSliceFlags     import CosmicSliceFlags
from TriggerMenuPython.GenericSliceFlags    import GenericSliceFlags
from TriggerMenuPython.MonitorSliceFlags    import MonitorSliceFlags

# lvl1
from TriggerMenuPython.Lvl1                 import Lvl1
from TriggerMenuPython.Egamma_Lvl1          import Egamma_Lvl1
from TriggerMenuPython.Bphysics_Lvl1        import Bphysics_Lvl1

# hlt
from TriggerMenuPython.TriggerPythonConfig  import TriggerPythonConfig
from TriggerMenuPython.Express              import addExpressStream
from TriggerMenuPython.Lumi                 import lumi

def reportSliceImportError(flags):
    log = logging.getLogger( 'GenerateOnlineMenu.py' )
    if flags.signatures() == None or len(flags.signatures()) == 0:
        log.warning("Error in configuration of "+flags.__class__.__name__+" but it is not required so we can continue")
    else:
        log.error("Error in configuration of "+flags.__class__.__name__)
        raise


class GenerateOnlineMenu:
    def __init__(self):

        log = logging.getLogger( 'GenerateOnlineMenu.py' )
        log.info('Including lvl1 configurations')
        # if TrigT1ConfigVersion:

        #self.egamma_lvl1 = Egamma_Lvl1()
        #self.bphysics_lvl1 = Bphysics_Lvl1()

        log.info('Including HLT configurations')
        self.chains = []

        #-----------------------------------------------------------------------------
        if TriggerFlags.EgammaSlice.signatures():
            try:
                current = TriggerFlags.EgammaSlice.signatures.get_Value()
                hasPhoton =  [ True for x in current if "g" == x[0] or "g" == x[1] ]
                hasElectron =  [ True for x in current if not ("g" == x[0] or "g" == x[1]) ]

                if hasElectron:
                    from TriggerMenuPython.EMObject           import EMObjects
                    from TriggerMenuPython.Electron           import Electrons
                    from TriggerMenuPython.MultiElectron      import MultiElectrons
                    self.chains += EMObjects
                    self.chains += Electrons
                    self.chains += MultiElectrons

                if hasPhoton:
                    from TriggerMenuPython.Photon             import Photons
                    from TriggerMenuPython.MultiPhoton        import MultiPhotons
                    self.chains += Photons
                    self.chains += MultiPhotons
            except:
                reportSliceImportError(EgammaSliceFlags)

        #-----------------------------------------------------------------------------            
        if TriggerFlags.MuonSlice.signatures():
            try:
                from TriggerMenuPython.Muon               import Muons
                from TriggerMenuPython.MultiMuon          import MultiMuons            
                self.chains += Muons
                self.chains += MultiMuons
            except:
                reportSliceImportError(MuonSliceFlags)

        #-----------------------------------------------------------------------------
        if TriggerFlags.TauSlice.signatures():
            try:
                from TriggerMenuPython.Tau                import Taus
                from TriggerMenuPython.MultiTau           import MultiTaus
                self.chains += Taus
                self.chains += MultiTaus
            except:
                reportSliceImportError(TauSliceFlags)

        #-----------------------------------------------------------------------------
        if TriggerFlags.METSlice.signatures():
            try:
                from TriggerMenuPython.MissingET          import MissingETs

                self.chains += MissingETs            
            except:
                reportSliceImportError(METSliceFlags) 

        #-----------------------------------------------------------------------------
        if TriggerFlags.JetSlice.signatures():
            try:
                from TriggerMenuPython.JetSequence        import JetSequences
                from TriggerMenuPython.Jet                import Jets
            
                self.chains += JetSequences
                self.chains += Jets
            except:
                reportSliceImportError(JetSliceFlags)
            
        #-----------------------------------------------------------------------------
        if TriggerFlags.BjetSlice.signatures():
            try:
                from TriggerMenuPython.Bjet               import Bjets
                from TriggerMenuPython.MultiBjet          import MultiBjets
            
                self.chains += Bjets
                self.chains += MultiBjets
            except:
                reportSliceImportError(BjetSliceFlags)

        #-----------------------------------------------------------------------------
        if TriggerFlags.BphysicsSlice.signatures():
            try:
                from TriggerMenuPython.Bphysics           import Bphysics
                self.chains += Bphysics
            except:
                reportSliceImportError(BphysicsSliceFlags)
            
        #-----------------------------------------------------------------------------            
        if TriggerFlags.MinBiasSlice.signatures():
            try:
                from TriggerMenuPython.MinBias import MinBias
                self.chains += MinBias
            except:
                reportSliceImportError(MinBiasSliceFlags) 

        #-----------------------------------------------------------------------------            
        #if TriggerFlags.CosmicSlice.signatures():
        try:
            from TriggerMenuPython.Cosmic import Cosmics
            self.chains += Cosmics
        except:
            reportSliceImportError(CosmicSliceFlags) 

        #-----------------------------------------------------------------------------
        if TriggerFlags.CombinedSlice.signatures():
            try:
                from TriggerMenuPython.Combined           import TauElectrons, TauMuons, TauMETs, JetMETs, ElectronMETs
                from TriggerMenuPython.Combined           import MuonMETs, MuonMETs, ElectronJets, MuonJets, ElectronMuons
                from TriggerMenuPython.Combined           import PhotonMETs, PhotonElectrons, PhotonMuons, TauJets, TauBjets, TripleSignatures
                self.chains += TauElectrons
                self.chains += TauMuons
                self.chains += TauMETs
                self.chains += JetMETs
                self.chains += ElectronMETs
                self.chains += MuonMETs
                self.chains += ElectronJets
                self.chains += MuonJets
                self.chains += ElectronMuons
                self.chains += PhotonMETs
                self.chains += PhotonElectrons
                self.chains += PhotonMuons
                self.chains += TauJets
                self.chains += TauBjets
                self.chains += TripleSignatures
            except:
                reportSliceImportError(CombinedSliceFlags)
                
        #-----------------------------------------------------------------------------            
        if TriggerFlags.GenericSlice.signatures():
            try:
                from TriggerMenuPython.GenericDef import Generic
                self.chains += Generic
            except:
                reportSliceImportError(GenericSliceFlags)
                
        #-----------------------------------------------------------------------------            
        if TriggerFlags.MonitorSlice.signatures():
            try:
                from TriggerMenuPython.MonitorDef import MonitorChainList
                self.chains += MonitorChainList
            except:
                reportSliceImportError(MonitorSliceFlags)                
        
    def generate(self):
        log = logging.getLogger( 'GenerateOnlineMenu.py' )

        if TriggerFlags.readHLTconfigFromXML() and TriggerFlags.readLVL1configFromXML():
            log.info('XML files generation not requested')
            return

        log.info('Doing consistency checks between defined slice signatures flags and accessible HLT configurations')
        log.info('Creating list of all available slice signatures flags')
        slice_signatures = []
        if TriggerFlags.EgammaSlice.signatures():
       	    slice_signatures.extend(EgammaSliceFlags.signatures.allowedValues())
        if TriggerFlags.TauSlice.signatures():
            slice_signatures.extend(TauSliceFlags.signatures.allowedValues())
        if TriggerFlags.JetSlice.signatures():
            slice_signatures.extend(JetSliceFlags.signatures.allowedValues())
        if TriggerFlags.METSlice.signatures():
            slice_signatures.extend(METSliceFlags.signatures.allowedValues())
        if TriggerFlags.MuonSlice.signatures():
            slice_signatures.extend(MuonSliceFlags.signatures.allowedValues())
        if TriggerFlags.BphysicsSlice.signatures():
            slice_signatures.extend(BphysicsSliceFlags.signatures.allowedValues())
        if TriggerFlags.BjetSlice.signatures():
            slice_signatures.extend(BjetSliceFlags.signatures.allowedValues())
        if TriggerFlags.CombinedSlice.signatures():
            slice_signatures.extend(CombinedSliceFlags.signatures.allowedValues())
        if TriggerFlags.MinBiasSlice.signatures():
            slice_signatures.extend(MinBiasSliceFlags.signatures.allowedValues())
        #if TriggerFlags.CosmicSlice.signatures():
        slice_signatures.extend(CosmicSliceFlags.signatures.allowedValues())
        log.info('All possible slice signatures flags: '+str(slice_signatures))

        log.debug('Creating list of available configurations')
        configs = [ x.sig_id for x in self.chains ]
        log.info('All possible configurations: '+str(configs))

        for i in slice_signatures:
            if i not in configs:
                log.warning('Slice signature flag: '+i+' defined but no corresponding configuration exists')

            
        log.info('generating XML configuration files')
        log.info('LVL1: '+str(TriggerFlags.inputLVL1configFile()))
        log.info('HLT : '+str(TriggerFlags.inputHLTconfigFile()))

        triggerPythonConfig = TriggerPythonConfig(TriggerFlags.outputHLTconfigFile(),
                                                  TriggerFlags.outputLVL1configFile())

        # global triggerPythonConfig

        lvl1_items = []
        lvl1_items.extend(Lvl1Flags.items())
        lvl1_thresholds = []
        lvl1_thresholds.extend(Lvl1Flags.thresholds())
        Lvl1.registerItems(triggerPythonConfig)
        log.info('N L1 items     : %s (/256)' % len(lvl1_items) )
        log.info('N L1 thresholds: %s (/54)' % len(lvl1_thresholds) )

        # pregenerate chains
        for chaindef in self.chains:
            log.info("generating: %s" % chaindef.sig_id )
            chaindef.generateMenu(triggerPythonConfig)

        # go over the slices and put together big list of signatures requested
        sigs = []
        log.info('modifying menu according to the luminosity and prescaling setup')
        lumi(triggerPythonConfig)
        addExpressStream(triggerPythonConfig)
        # lumi_old_20071003(triggerPythonConfig)

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

        if CombinedSliceFlags.signatures():
            sigs += CombinedSliceFlags.signatures()

        if MinBiasSliceFlags.signatures():
            sigs += MinBiasSliceFlags.signatures()

        if CosmicSliceFlags.signatures():
            sigs += CosmicSliceFlags.signatures()

        log.info( 'Enabled signatures: '+str(sigs) )


        # note only this chains enter current config
        # print triggerPythonConfig.allChains

##         log.info('modifying menu according to the luminosity and prescaling setup')
##         lumi(triggerPythonConfig)
        if not TriggerFlags.readLVL1configFromXML:
            Lvl1.generateMenu(triggerPythonConfig, \
                              Lvl1Flags.items(), Lvl1Flags.thresholds())
        for name, chains in triggerPythonConfig.allChains.iteritems():
            if name in sigs:
                log.info("placing in the configuration chains for: %s" % name )
                [triggerPythonConfig.addHLTChain(c) for c in chains]

        # dump configuration files
        triggerPythonConfig.writeConfigFiles()
        triggerPythonConfig.dot(algs=True)

        # suggest counters for new chains
        countersL2 = []
        countersEF = []
        for name, chains in triggerPythonConfig.allChains.iteritems():
            for c in chains:

                if c.isL2():
                    countersL2.append(int(c.chain_counter))
                if c.isEF():
                    countersEF.append(int(c.chain_counter))
        countersL2.sort()
        countersEF.sort()
        maxL2 = max(countersL2)
        maxEF = max(countersEF)

        log.info("L2 available chain counters:" +str([x for x in range(0, 1023) if x not in countersL2]))
        log.info("EF available chain counters:" +str([x for x in range(0, 1023) if x not in countersEF]))

