# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################
# Common code used for the HIGG4 thinning       #
# Z.Zinonos                                     #
# zenon@cern.ch                                 #
# Nov 2015                                      #
#################################################

from AthenaCommon.GlobalFlags import globalflags

# running on data or MC
DFisMC = (globalflags.DataSource()=='geant4')

# trigger navigation
def TriggerChains(HIGG4DxName):
    if HIGG4DxName == 'HIGG4D1': 
        # single-e, single-mu, di-e, di-mu, and mu-e triggers.
        # removing combined lepton + jet, missingET, gamma, bphys, and performance triggers
        return '^(?!.*_[0-9]*(j|xe|tau|g|b|perf|idperf))(HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*)'
    elif HIGG4DxName == 'HIGG4D2': 
        # single-e, single-mu, and combined lepton+tau triggers
        # Removing multiplepton triggers (also with asymmetric thresholds), lepton+jet, lepton+missingET, lepton-photon, b-phys, and performance triggers
        return '(^(?!.*_[0-9]*(mu|j|xe|g|b|perf|idperf))(?!HLT_e.*_[0-9]*e.*)(HLT_e.*))|(^(?!.*_[0-9]*(e|j|xe|g|b|perf|idperf))(?!HLT_mu.*_[0-9]*mu.*)(HLT_mu.*))'
    elif HIGG4DxName in ['HIGG4D3', 'HIGG4D4', 'HIGG4D5']:
        # keeping all tau triggers as there isn't that many of them
        return 'HLT_tau.*'
    elif HIGG4DxName in ['HIGG4D6']:
        return ''
    else :
        assert False, "HIGG4DxThinning: Unknown derivation stream '{}'".format(HIGG4DxName)

def setup(HIGG4DxName, HIGG4DxThinningSvc, ToolSvc):
    thinningTools=[]

    #fat jets and track thinning
    if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D6']:
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
        HIGG4DxJetTrackThinningTool1 = DerivationFramework__JetTrackParticleThinning( name          	    = HIGG4DxName+"JetTrackThinningTool1",
                                                                                      ThinningService        = HIGG4DxThinningSvc,
                                                                                      JetKey                 = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                                      SelectionString        = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                      ApplyAnd               = True)
        ToolSvc += HIGG4DxJetTrackThinningTool1
        thinningTools.append(HIGG4DxJetTrackThinningTool1)

    if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D6']:
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
        HIGG4DxJetTrackThinningTool2 = DerivationFramework__JetTrackParticleThinning( name          	    = HIGG4DxName+"JetTrackThinningTool2",
                                                                                      ThinningService        = HIGG4DxThinningSvc,
                                                                                      JetKey                 = "AntiKt2PV0TrackJets",
                                                                                      SelectionString        = "AntiKt2PV0TrackJets.pt > 6*GeV",
                                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                      ApplyAnd               = True)
        #ToolSvc += HIGG4DxJetTrackThinningTool2
        #thinningTools.append(HIGG4DxJetTrackThinningTool2)

    # Tracks associated with Muons
    HIGG4DxMuonSelectionString = ""
    if HIGG4DxName in ['HIGG4D3', 'HIGG4D4', 'HIGG4D5', 'HIGG4D6']:
        HIGG4DxMuonSelectionString = "Muons.pt > 1*GeV"
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
    HIGG4DxMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = HIGG4DxName+"MuonTPThinningTool",
                                                                               ThinningService         = HIGG4DxThinningSvc,
                                                                               MuonKey                 = "Muons",
                                                                               SelectionString 	       = HIGG4DxMuonSelectionString,
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
    ToolSvc += HIGG4DxMuonTPThinningTool
    thinningTools.append(HIGG4DxMuonTPThinningTool)

    # Tracks associated with Electrons
    HIGG4DxElectronSelectionString = ""
    if HIGG4DxName in ['HIGG4D3', 'HIGG4D4', 'HIGG4D5', 'HIGG4D6']:
        HIGG4DxElectronSelectionString = "Electrons.pt > 5*GeV"
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
    HIGG4DxElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = HIGG4DxName+"ElectronTPThinningTool",
                                                                                     ThinningService         = HIGG4DxThinningSvc,
                                                                                     SGKey                   = "Electrons",
                                                                                     SelectionString 	     = HIGG4DxElectronSelectionString,
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
    ToolSvc += HIGG4DxElectronTPThinningTool
    thinningTools.append(HIGG4DxElectronTPThinningTool)

    # Tracks associated with taus
    HIGG4DxTauSelectionString = "TauJets.pt > 18*GeV"
    if HIGG4DxName in ['HIGG4D4', 'HIGG4D5', 'HIGG4D6']:
        HIGG4DxTauSelectionString = "TauJets.pt > 40*GeV"
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
    HIGG4DxTauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = HIGG4DxName+"TauTPThinningTool",
                                                                             ThinningService         = HIGG4DxThinningSvc,
                                                                             TauKey                  = "TauJets",
                                                                             SelectionString         = HIGG4DxTauSelectionString,
                                                                             ConeSize                = 0.6,
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
    ToolSvc += HIGG4DxTauTPThinningTool
    thinningTools.append(HIGG4DxTauTPThinningTool)

    # tracks associated with DiTaus
    if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D5', 'HIGG4D6']:
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
        HIGG4DxDiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = HIGG4DxName+"DiTauTPThinningTool",
                                                                                     ThinningService         = HIGG4DxThinningSvc,
                                                                                     DiTauKey                = "DiTauJets",
                                                                                     SelectionString         = "",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += HIGG4DxDiTauTPThinningTool
        thinningTools.append(HIGG4DxDiTauTPThinningTool)
    


    #Truth tau/nutau and their ancestors and descendants
    truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
    truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
    truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

    #truth_photon_thinning = "(TruthPhotons.pt > 1.0*GeV) && (abs(TruthPhotons.eta)<3.0)"
    # PreserveGeneratorDescendants only keeps particles that came directly from the event generator
    # PreserveDescendants keeps all particles including those that come from Geant processes

    if DFisMC:
    	#thin special containers
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    	#thin taus
        HIGG4DxTruthTool_TAU = DerivationFramework__GenericTruthThinning(name                         = HIGG4DxName+"TruthTool_TAU",
                                                                         ThinningService              = HIGG4DxThinningSvc,
                                                                         ParticleSelectionString      = truth_cond_tau,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = False,
                                                                         TauHandling                  = False)
        ToolSvc += HIGG4DxTruthTool_TAU
        thinningTools.append(HIGG4DxTruthTool_TAU)

	#thin leptons and taus
        HIGG4DxTruthTool_COMB = DerivationFramework__GenericTruthThinning(name                         = HIGG4DxName+"TruthTool_COMB",
                                                                          ThinningService              = HIGG4DxThinningSvc,
                                                                          ParticleSelectionString      = truth_cond_comb,
                                                                          PreserveDescendants          = False,
                                                                          PreserveGeneratorDescendants = False,
                                                                          PreserveAncestors            = True,
                                                                          TauHandling                  = False)

        ToolSvc += HIGG4DxTruthTool_COMB
        thinningTools.append(HIGG4DxTruthTool_COMB)

    	#thin photons
        """HIGG4DxTruthTool_PHO = DerivationFramework__GenericTruthThinning(name                    = "HIGG4DxTruthTool_PHO",
                                                                     ThinningService         = HIGG4DxThinningHelper.ThinningSvc(),
                                                                     ParticlesKey            = "TruthPhotons",
                                                                     ParticleSelectionString = truth_photon_thinning)
    
                                                                     ToolSvc += HIGG4DxTruthTool_PHO
                                                                     thinningTools.append(HIGG4DxTruthTool_PHO)"""

    	## thinning the master truth collection
        WriteFirstN = 15
        WriteBHadrons = False
        PreserveAncestors = True
        #relevant for D3,D4,D5,D6 only
        if HIGG4DxName in ['HIGG4D3', 'HIGG4D4', 'HIGG4D5', 'HIGG4D6']:
            WriteBHadrons = True

            # adding more samples
            dsids = []
            #graviton, 2HDM, Non-resonant HH samples
            dsids += range(303349,303436+1)
            dsids += range(342626,342643+1)
            dsids += range(342622, 342623+1)
            # 2HDM
            dsids += [342626,342627,342628,342629,342630,342631,342632,342633,342634,345190,345191,345192,345193,345194]
            # 2HDM NLO
            dsids += [343722,343725,343727,343729,343733]
            # AZH
            dsids += [344981,344983,344985,344987,344989,344991,344993,344995,344997,344999,345001,345003,345005,345007,345009,345011,345013,345015,345017,345019]
            # LQ3
            dsids += [308043,308044,308045,308046,308046,308047,308048,308049,308050,308051,308052,308053,308054,308055,308056,308443,308444,308445,308446,308447,308448,308449]
            # RSG
            dsids += [303395,303396,303397,303398,303399,303400,303401,303402,303403,303417,303421,308272,308273,308274,308289,308290,308450,308451,308452,308453,308454,308455,308456,308457,308458]
            # variable lambda hh samples
            dsids += [345569,345568,345567,345564,345565,345566,345695,345696]

            import PyUtils.AthFile as af
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
            if len(f.mc_channel_number) > 0:
                mcn = int(f.mc_channel_number[0])
                if mcn in dsids:
                    WriteFirstN = 30 #graviton, 2HDM, Non-resonant HH samples


        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
        HIGG4DxTruthTool_MENU = DerivationFramework__MenuTruthThinning(  name 			    = HIGG4DxName+"TruthTool_MENU",
                                                                         ThinningService 	    = HIGG4DxThinningSvc,
                                                                         WritePartons               = False,
                                                                         WriteHadrons               = False,
                                                                         WriteBHadrons              = WriteBHadrons,
                                                                         WriteGeant                 = False,
                                                                         GeantPhotonPtThresh        = -1.0,
                                                                         WriteTauHad                = False,
                                                                         PartonPtThresh             = -1.0,
                                                                         WriteBSM                   = False,
                                                                         WriteBosons                = False,
                                                                         WriteBSMProducts           = False,
                                                                         WriteBosonProducts         = False,
                                                                         WriteTopAndDecays          = False,
                                                                         WriteEverything            = False,
                                                                         WriteAllLeptons            = False,
                                                                         WriteStatus3               = False,
                                                                         PreserveDescendants        = False,
                                                                         PreserveGeneratorDescendants = False,
                                                                         PreserveAncestors          = PreserveAncestors,
                                                                         WriteFirstN                = WriteFirstN)

        ToolSvc += HIGG4DxTruthTool_MENU
        thinningTools.append(HIGG4DxTruthTool_MENU)
    #end of MC

    # return thinning tools
    return thinningTools

