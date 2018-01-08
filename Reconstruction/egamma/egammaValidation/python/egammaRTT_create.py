# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import user
import ROOT

def execute(g,electron = True, photon = True, expert = False) :
    from ROOT import TObjArray

    print "::: Creating File ::: "
    if(expert) :
        file = ROOT.TFile("expert.root","RECREATE")
        pass
    else :
        file =  ROOT.TFile("shifter.root","RECREATE")
        pass
    
    ## Default flags
    flags = {}
    
    flags["Electron"] = electron               ## Electrons variables
    flags["Photon"] = photon                   ## Photons variables
    flags["Expert"] = expert                   ## expert/shifter mode
    
    treeg = g.egammaD3PD

    
    print "::: Histos definition ::: BEGIN"
    
    if flags["Electron"] :
        histoEleAll = {}
        histoEleAllLoose = {}
        histoEleAllMedium = {}
        histoEleAllTight = {}
        
        histoEleLoose = {}
        histoEleLooseLoose = {}
        histoEleLooseMedium = {}
        histoEleLooseTight = {}
        
        histoEleMedium = {}
        histoEleMediumLoose = {}
        histoEleMediumMedium = {}
        histoEleMediumTight = {}
        
        histoEleTight = {}
        histoEleTightLoose = {}
        histoEleTightMedium = {}
        histoEleTightTight = {}
        
        histoEleAuthor1 = {}
        histoEleAuthor1Loose = {}
        histoEleAuthor1Medium = {}
        histoEleAuthor1Tight = {}
        
        histoEleAuthor2 = {}
        histoEleAuthor2Loose = {}
        histoEleAuthor2Medium = {}
        histoEleAuthor2Tight = {}
        
        histoEleAuthor3 = {}
        histoEleAuthor3Loose = {}
        histoEleAuthor3Medium = {}
        histoEleAuthor3Tight = {}
        
        histoEleAuthor8 = {}
        histoEleAuthor8Loose = {}
        histoEleAuthor8Medium = {}
        histoEleAuthor8Tight = {}

        histoEleAll = defHistEle(0,histoEleAll)
        histoEleAllLoose = defHistEle(1,histoEleAllLoose)
        histoEleAllMedium = defHistEle(2,histoEleAllMedium)
        histoEleAllTight = defHistEle(3,histoEleAllTight)
        
        histoEleLoose = defHistEle(10,histoEleLoose)
        histoEleLooseLoose = defHistEle(11,histoEleLooseLoose)
        histoEleLooseMedium = defHistEle(12,histoEleLooseMedium)
        histoEleLooseTight = defHistEle(13,histoEleLooseTight)
        
        histoEleMedium = defHistEle(20,histoEleMedium)
        histoEleMediumLoose = defHistEle(21,histoEleMediumLoose)
        histoEleMediumMedium = defHistEle(22,histoEleMediumMedium)
        histoEleMediumTight = defHistEle(23,histoEleMediumTight)
        
        histoEleTight = defHistEle(30,histoEleTight)
        histoEleTightLoose = defHistEle(31,histoEleTightLoose)
        histoEleTightMedium = defHistEle(32,histoEleTightMedium)
        histoEleTightTight = defHistEle(33,histoEleTightTight)
        
        histoEleAuthor1 = defHistEle(40,histoEleAuthor1)
        histoEleAuthor1Loose = defHistEle(41,histoEleAuthor1Loose)
        histoEleAuthor1Medium = defHistEle(42,histoEleAuthor1Medium)
        histoEleAuthor1Tight = defHistEle(43,histoEleAuthor1Tight)
        
        histoEleAuthor2 = defHistEle(50,histoEleAuthor2)
        histoEleAuthor2Loose = defHistEle(51,histoEleAuthor2Loose)
        histoEleAuthor2Medium = defHistEle(52,histoEleAuthor2Medium)
        histoEleAuthor2Tight = defHistEle(53,histoEleAuthor2Tight)
        
        histoEleAuthor3 = defHistEle(60,histoEleAuthor3)
        histoEleAuthor3Loose = defHistEle(61,histoEleAuthor3Loose)
        histoEleAuthor3Medium = defHistEle(62,histoEleAuthor3Medium)
        histoEleAuthor3Tight = defHistEle(63,histoEleAuthor3Tight)
        
        histoEleAuthor8 = defHistEle(70,histoEleAuthor8)
        histoEleAuthor8Loose = defHistEle(71,histoEleAuthor8Loose)
        histoEleAuthor8Medium = defHistEle(72,histoEleAuthor8Medium)
        histoEleAuthor8Tight = defHistEle(73,histoEleAuthor8Tight)        

        pass

    if flags["Photon"] :
        histoPhoAll = {}
        histoPhoAllLoose = {}
        histoPhoAllTight = {}
        
        histoPhoLoose = {}
        histoPhoLooseLoose = {}
        histoPhoLooseTight = {}
        
        histoPhoTight = {}
        histoPhoTightLoose = {}
        histoPhoTightTight = {}
        
        histoPhoAuthor4 = {}
        histoPhoAuthor4Loose = {}
        histoPhoAuthor4Tight = {}
        
        histoPhoAuthor16 = {}
        histoPhoAuthor16Loose = {}
        histoPhoAuthor16Tight = {}
        
        histoPhoIsConv = {}
        histoPhoIsConvLoose = {}
        histoPhoIsConvTight = {}
        
        histoPhoUnConv = {}
        histoPhoUnConvLoose = {}
        histoPhoUnConvTight = {}
        
        histoPhoAll = defHistPho(0,histoPhoAll)
        histoPhoAllLoose = defHistPho(1,histoPhoAllLoose)
        histoPhoAllTight = defHistPho(2,histoPhoAllTight)
        
        histoPhoLoose = defHistPho(10,histoPhoLoose)
        histoPhoLooseLoose = defHistPho(11,histoPhoLooseLoose)
        histoPhoLooseTight = defHistPho(12,histoPhoLooseTight)

        hitoPhoTight = defHistPho(20,histoPhoTight)
        histoPhoTightLoose = defHistPho(21,histoPhoTightLoose)
        histoPhoTightTight = defHistPho(22,histoPhoTightTight)

        histoPhoAuthor4 = defHistPho(30,histoPhoAuthor4)
        histoPhoAuthor4Loose = defHistPho(31,histoPhoAuthor4Loose)
        histoPhoAuthor4Tight = defHistPho(32,histoPhoAuthor4Tight)

        histoPhoAuthor16 = defHistPho(40,histoPhoAuthor16)
        histoPhoAuthor16Loose = defHistPho(41,histoPhoAuthor16Loose)
        histoPhoAuthor16Tight = defHistPho(42,histoPhoAuthor16Tight)

        histoPhoIsConv = defHistPho(50,histoPhoIsConv)
        histoPhoIsConvLoose = defHistPho(51,histoPhoIsConvLoose)
        histoPhoIsConvTight = defHistPho(52,histoPhoIsConvTight)

        histoPhoUncov = defHistPho(60,histoPhoUnConv)
        histoPhoUnConvLoose = defHistPho(61,histoPhoUnConvLoose)
        histoPhoUnConvTight = defHistPho(62,histoPhoUnConvTight)

        pass

    if flags["Expert"] :
        
        histoExpEle = {}
        histoExpEleIden = {} # identification variables
        histoExpEleKin = {} # kinematics variables
        histoExpEleConv = {} # conversion0 variables
        histoExpEleEM = {} # EM variables
        histoExpEleIso = {} # isolation variables
        histoExpEleDis = {} # discriminants variables
        histoExpElePoint = {} # pointing variables
        histoExpEleClKin = {} # cluster kinematics
        histoExpEleL1 = {} # L1 variables
        histoExpEleEF = {} # EF variables
        histoExpEleL2 = {} # L2 variables
        histoExpEleJet = {} # jet variables
        histoExpEleTrk = {} # track variables
        histoExpEleRaw = {} # rawcl variables
        histoExpEleVtx = {} # vertex variables
        histoExpEleBrem = {} # Brem variables
        histoExpEleMC = {} # MC variables
        histoExpEleTrkMatch = {} # track match variables
        histoExpEleEMTrkFit = {} # EM track fit variables
        histoExpEleForward = {} # forward variables

        histoExpEle = defHistExpEle(0,histoExpEle)
        histoExpEleIden = defHistExpEle(1,histoExpEleIden)
        histoExpEleKin = defHistExpEle(2,histoExpEleKin)
        histoExpEleConv = defHistExpEle(3,histoExpEleConv)
        histoExpEleEM = defHistExpEle(4,histoExpEleEM)
        histoExpEleIso = defHistExpEle(5,histoExpEleIso)
        histoExpEleDis = defHistExpEle(6,histoExpEleDis)
        histoExpElePoint = defHistExpEle(7,histoExpElePoint)
        histoExpEleClKin = defHistExpEle(8,histoExpEleClKin)
        histoExpEleL1 = defHistExpEle(9,histoExpEleL1)
        histoExpEleL2 = defHistExpEle(10,histoExpEleL2)
        histoExpEleEF = defHistExpEle(11,histoExpEleEF)
        histoExpEleJet = defHistExpEle(12,histoExpEleJet)
        histoExpEleTrk = defHistExpEle(13,histoExpEleTrk)
        histoExpEleRaw = defHistExpEle(14,histoExpEleRaw)
        histoExpEleVtx = defHistExpEle(15,histoExpEleVtx)
        histoExpEleBrem = defHistExpEle(16,histoExpEleBrem)
        histoExpEleMC = defHistExpEle(17,histoExpEleMC)
        histoExpEleTrkMatch = defHistExpEle(18,histoExpEleTrkMatch)
        histoExpEleEMTrkFit = defHistExpEle(19,histoExpEleEMTrkFit)
        histoExpEleForward = defHistExpEle(20,histoExpEleForward)
        
        histoExpPho = {}
        histoExpPhoIden = {} # identification variables
        histoExpPhoKin = {} # kinematics variables
        histoExpPhoConv = {} # conversion0 variables
        histoExpPhoEM = {} # EM variables
        histoExpPhoIso = {} # isolation variables
        histoExpPhoDis = {} # discriminants variables
        histoExpPhoPoint = {} # pointing variables
        histoExpPhoClKin = {} # cluster kinematics
        histoExpPhoL1 = {} # L1 variables
        histoExpPhoL2 = {} # L2 variables
        histoExpPhoEF = {} # EF variables
        histoExpPhoJet = {} # jet variables
        histoExpPhoTrk = {} # track variables
        histoExpPhoRaw = {} # rawcl variables
        histoExpPhoTopo = {} # Topo variables
        histoExpPhoMC = {} # MC variables

        histoExpPho = defHistExpPho(0,histoExpPho)
        histoExpPhoIden = defHistExpPho(1,histoExpPhoIden)
        histoExpPhoKin = defHistExpPho(2,histoExpPhoKin)
        histoExpPhoConv = defHistExpPho(3,histoExpPhoConv)
        histoExpPhoEM = defHistExpPho(4,histoExpPhoEM)
        histoExpPhoIso = defHistExpPho(5,histoExpPhoIso)
        histoExpPhoDis = defHistExpPho(6,histoExpPhoDis)
        histoExpPhoPoint = defHistExpPho(7,histoExpPhoPoint)
        histoExpPhoClKin = defHistExpPho(8,histoExpPhoClKin)
        histoExpPhoL1 = defHistExpPho(9,histoExpPhoL1)
        histoExpPhoL2 = defHistExpPho(10,histoExpPhoL2)
        histoExpPhoEF = defHistExpPho(11,histoExpPhoEF)
        histoExpPhoJet = defHistExpPho(12,histoExpPhoJet)
        histoExpPhoTrk = defHistExpPho(13,histoExpPhoTrk)
        histoExpPhoRaw = defHistExpPho(14,histoExpPhoRaw)
        histoExpPhoTopo = defHistExpPho(15,histoExpPhoTopo)
        histoExpPhoMC = defHistExpPho(16,histoExpPhoMC)


        histoExpMu = {}
        histoExpMuKin = {} # kinematics varaibles
        histoExpMuAlgo = {} # Algorithms
        histoExpMuEnerCalo = {} # Energy loss in the Calo
        histoExpMuHits = {} # Muon Spectrometer Hits Summary
        histoExpMuIso = {} # Isolation
        histoExpMuTrkPat = {} # Track pattern
        histoExpMuTrk = {} # Track parameters
        histoExpMuQ = {} # Track quality
        histoExpMuL1 = {} # L1 variables
        histoExpMuL2 = {} # L2 variables
        histoExpMuEF = {} # EF variables
        histoExpMuMC = {} # MC variables

        histoExpMu = defHistExpMu(0,histoExpMu)
        histoExpMuKin = defHistExpMu(1,histoExpMuKin)
        histoExpMuAlgo = defHistExpMu(2,histoExpMuAlgo)
        histoExpMuEnerCalo = defHistExpMu(3,histoExpMuEnerCalo)
        histoExpMuHits = defHistExpMu(4,histoExpMuHits)
        histoExpMuIso = defHistExpMu(5,histoExpMuIso)
        histoExpMuTrkPat = defHistExpMu(6,histoExpMuTrkPat)
        histoExpMuTrk = defHistExpMu(7,histoExpMuTrk)
        histoExpMuQ = defHistExpMu(8,histoExpMuQ)
        histoExpMuL1 = defHistExpMu(9,histoExpMuL1)
        histoExpMuL2 = defHistExpMu(10,histoExpMuL2)
        histoExpMuEF = defHistExpMu(11,histoExpMuEF)
        histoExpMuMC = defHistExpMu(12,histoExpMuMC)


        histoExpMC = {} # D3PD McEvent
        histoExpGlobal = {} # D3PD GlobalEvent
        histoExpJet = {} # D3PD JetContent
        histoExpMET = {} # D3PD MetContent
        histoExpMb = {} # D3PD Minimum Bias Content
        histoExpTrig = {} # D3PD Trigger Bit

        histoExpMC = defHistExpOther(0,histoExpMC)
        histoExpGlobal = defHistExpOther(1,histoExpGlobal)
        histoExpJet = defHistExpOther(2,histoExpJet)
        histoExpMET = defHistExpOther(3,histoExpMET)
        histoExpMb = defHistExpOther(4,histoExpMb)
        histoExpTrig = defHistExpOther(5,histoExpTrig)


        

        pass
    
    print "::: Histos definition ::: END"


    
    print "::: Opening the branches ::: BEGIN"
    for i in xrange(treeg.GetEntriesFast()) :
        if treeg.GetEntry(i) <= 0 :
            print "problem with the entrie", i
            break
        if flags["Electron"] : branchEle(treeg,histoEleAll,histoEleAllLoose,histoEleAllMedium,histoEleAllTight,histoEleLoose,histoEleLooseLoose,histoEleLooseMedium,histoEleLooseTight,histoEleMedium,histoEleMediumLoose,histoEleMediumMedium,histoEleMediumTight,histoEleTight,histoEleTightLoose,histoEleTightMedium,histoEleTightTight,histoEleAuthor1,histoEleAuthor1Loose,histoEleAuthor1Medium,histoEleAuthor1Tight,histoEleAuthor2,histoEleAuthor2Loose,histoEleAuthor2Medium,histoEleAuthor2Tight,histoEleAuthor3,histoEleAuthor3Loose,histoEleAuthor3Medium,histoEleAuthor3Tight,histoEleAuthor8,histoEleAuthor8Loose,histoEleAuthor8Medium,histoEleAuthor8Tight)
        if flags["Photon"] : branchPho(treeg,histoPhoAll,histoPhoAllLoose,histoPhoAllTight,histoPhoLoose,histoPhoLooseLoose,histoPhoLooseTight,histoPhoTight,histoPhoTightLoose,histoPhoTightTight,histoPhoAuthor4,histoPhoAuthor4Loose,histoPhoAuthor4Tight,histoPhoAuthor16,histoPhoAuthor16Loose,histoPhoAuthor16Tight,histoPhoIsConv,histoPhoIsConvLoose,histoPhoIsConvTight,histoPhoUnConv,histoPhoUnConvLoose,histoPhoUnConvTight)
        if flags["Expert"] :
            branchExpert(treeg)
            branchExpEle(treeg,histoExpEleIden,histoExpEleKin,histoExpEleConv,histoExpEleEM)
            pass
        pass
    print "::: Opening the branches ::: END"
    
    
    print "::: Writing histos ::: BEGIN"
    if flags["Electron"] : WriteEle(file,histoEleAll,histoEleAllLoose,histoEleAllMedium,histoEleAllTight,histoEleLoose,histoEleLooseLoose,histoEleLooseMedium,histoEleLooseTight,histoEleMedium,histoEleMediumLoose,histoEleMediumMedium,histoEleMediumTight,histoEleTight,histoEleTightLoose,histoEleTightMedium,histoEleTightTight,histoEleAuthor1,histoEleAuthor1Loose,histoEleAuthor1Medium,histoEleAuthor1Tight,histoEleAuthor2,histoEleAuthor2Loose,histoEleAuthor2Medium,histoEleAuthor2Tight,histoEleAuthor3,histoEleAuthor3Loose,histoEleAuthor3Medium,histoEleAuthor3Tight,histoEleAuthor8,histoEleAuthor8Loose,histoEleAuthor8Medium,histoEleAuthor8Tight)
    if flags["Photon"] : WritePho(file,histoPhoAll,histoPhoAllLoose,histoPhoAllTight,histoPhoLoose,histoPhoLooseLoose,histoPhoLooseTight,histoPhoTight,histoPhoTightLoose,histoPhoTightTight,histoPhoAuthor4,histoPhoAuthor4Loose,histoPhoAuthor4Tight,histoPhoAuthor16,histoPhoAuthor16Loose,histoPhoAuthor16Tight,histoPhoIsConv,histoPhoIsConvLoose,histoPhoIsConvTight,histoPhoUnConv,histoPhoUnConvLoose,histoPhoUnConvTight)
    if flags["Expert"] :
        Exp = WriteExpert(file)
        WriteExpEle(file,Exp, histoExpEleIden, histoExpEleKin, histoExpEleConv, histoExpEleEM)
        WriteExpPho(file,Exp)
        WriteExpMu(file,Exp)
        WriteExpOther(file,Exp)
        pass
    print "::: Writing histos ::: END"
    
    file.Close()
    print "::: File closed ::: "
    
    return True



#######################
#######################
###                 ###
###   fonctions     ###
###                 ###
#######################
#######################


###################
## Def of histos ##
###################

def defHistEle(n,histos) :

    if n == 0 : ## histos for TDirectory Electron -> All
        el_charge_h1 = ROOT.TH1F("el_charge_h1","el_charge",3,-1.5,1.5)
        histos["el_charge"] = el_charge_h1
        el_author_h1 = ROOT.TH1F("el_author_h1","el_author",10,0.,10.)
        histos["el_author"] = el_author_h1
        el_n_h1 = ROOT.TH1F("el_n_h1","el_n",20,0.,20.)
        histos["el_n"] = el_n_h1
        el_eta_h1 = ROOT.TH1F("el_eta_h1","el_eta",100,-4.5,4.5)
        histos["el_eta"] = el_eta_h1
        el_phi_h1 = ROOT.TH1F("el_phi_h1","el_phi",100,-4.5,4.5)
        histos["el_phi"] = el_phi_h1
        el_pt_h1 = ROOT.TH1F("el_pt_h1","el_pt",100,0.,100000.)
        histos["el_pt"] = el_pt_h1
        el_loose_h1 = ROOT.TH1F("el_loose_h1","el_loose",2,0.,2.)
        histos["el_loose"] = el_loose_h1
        el_medium_h1 = ROOT.TH1F("el_medium_h1","el_medium",2,0.,2.)
        histos["el_medium"] = el_medium_h1
        el_mediumIso_h1 = ROOT.TH1F("el_mediumIso_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso"] = el_mediumIso_h1
        el_tight_h1 = ROOT.TH1F("el_tight_h1","el_tight",2,0.,2.)
        histos["el_tight"] = el_tight_h1
        el_tightIso_h1 = ROOT.TH1F("el_tightIso_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso"] = el_tightIso_h1
        pass

    elif n == 1 : ## histos for TDirectory Electron -> All -> LooseVariables
        el_weta2_h1 = ROOT.TH1F("el_weta2_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2"] = el_weta2_h1
        el_E237_h1 = ROOT.TH1F("el_E237_h1","el_E237",100,0.,200000.)
        histos["el_E237"] = el_E237_h1
        el_E277_h1 = ROOT.TH1F("el_E277_h1","el_E277",100,0.,200000.)
        histos["el_E277"] = el_E277_h1
        el_Ethad_h1 = ROOT.TH1F("el_Ethad_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad"] = el_Ethad_h1
        el_Ethad1_h1 = ROOT.TH1F("el_Ethad1_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1"] = el_Ethad1_h1
        pass

    elif n == 2 : ## histos for TDirectory Electron -> All -> MediumVariables
        el_f1_h1 = ROOT.TH1F("el_f1_h1","el_f1",50,0.,1.)
        histos["el_f1"] = el_f1_h1
        el_wstot_h1 = ROOT.TH1F("el_wstot_h1","el_wstot",100,0.,10.)
        histos["el_wstot"] = el_wstot_h1
        el_emaxs1_h1 = ROOT.TH1F("el_emaxs1_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1"] = el_emaxs1_h1
        el_nPixHits_h1 = ROOT.TH1F("el_nPixHits_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits"] = el_nPixHits_h1
        el_nSCTHits_h1 = ROOT.TH1F("el_nSCTHits_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits"] = el_nSCTHits_h1
        el_nSiHits_h1 = ROOT.TH1F("el_nSiHits_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits"] = el_nSiHits_h1
        pass

    elif n == 3 : ## histos for TDirectory Electron -> All -> TightVariables       
        el_nBLHits_h1 = ROOT.TH1F("el_nBLHits_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits"] = el_nBLHits_h1
        el_expectHitInBLayer_h1 = ROOT.TH1F("el_expectHitInBLayer_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer"] = el_expectHitInBLayer_h1
        el_cl_E_h1 = ROOT.TH1F("el_cl_E_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E"] = el_cl_E_h1
        el_trackpt_h1 = ROOT.TH1F("el_trackpt_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt"] = el_trackpt_h1
        el_nTRTHits_h1 = ROOT.TH1F("el_nTRTHits_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits"] = el_nTRTHits_h1
        el_TRTHighTHitsRatio_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio"] = el_TRTHighTHitsRatio_h1
        pass
    
    elif n == 10 : ## histos for TDirectory Electron -> LooseElectrons
        el_charge_ifLoose_h1 = ROOT.TH1F("el_charge_ifLoose_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifLoose"] = el_charge_ifLoose_h1
        el_author_ifLoose_h1 = ROOT.TH1F("el_author_ifLoose_h1","el_author",10,0.,10.)
        histos["el_author_ifLoose"] = el_author_ifLoose_h1
        el_n_ifLoose_h1 = ROOT.TH1F("el_n_ifLoose_h1","el_n",20,0.,20.)
        histos["el_n_ifLoose"] = el_n_ifLoose_h1
        el_eta_ifLoose_h1 = ROOT.TH1F("el_eta_ifLoose_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifLoose"] = el_eta_ifLoose_h1
        el_phi_ifLoose_h1 = ROOT.TH1F("el_phi_ifLoose_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifLoose"] = el_phi_ifLoose_h1
        el_pt_ifLoose_h1 = ROOT.TH1F("el_pt_ifLoose_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifLoose"] = el_pt_ifLoose_h1
        el_loose_ifLoose_h1 = ROOT.TH1F("el_loose_ifLoose_h1","el_loose",10,0.,2.5)
        histos["el_loose_ifLoose"] = el_loose_ifLoose_h1
        el_medium_ifLoose_h1 = ROOT.TH1F("el_medium_ifLoose_h1","el_medium",2,0.,2.)
        histos["el_medium_ifLoose"] = el_medium_ifLoose_h1
        el_mediumIso_ifLoose_h1 = ROOT.TH1F("el_mediumIso_ifLoose_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifLoose"] = el_mediumIso_ifLoose_h1
        el_tight_ifLoose_h1 = ROOT.TH1F("el_tight_ifLoose_h1","el_tight",2,0.,2.)
        histos["el_tight_ifLoose"] = el_tight_ifLoose_h1
        el_tightIso_ifLoose_h1 = ROOT.TH1F("el_tightIso_ifLoose_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifLoose"] = el_tightIso_ifLoose_h1
        pass

    elif n == 11 : ## histos for TDirectory Electron -> LooseElectrons -> LooseVariables
        el_weta2_isLoose_h1 = ROOT.TH1F("el_weta2_isLoose_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifLoose"] = el_weta2_isLoose_h1
        el_E237_isLoose_h1 = ROOT.TH1F("el_E237_isLoose_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifLoose"] = el_E237_isLoose_h1
        el_E277_isLoose_h1 = ROOT.TH1F("el_E277_isLoose_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifLoose"] = el_E277_isLoose_h1
        el_Ethad_isLoose_h1 = ROOT.TH1F("el_Ethad_isLoose_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifLoose"] = el_Ethad_isLoose_h1
        el_Ethad1_isLoose_h1 = ROOT.TH1F("el_Ethad1_isLoose_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifLoose"] = el_Ethad1_isLoose_h1
        pass

    elif n == 12 : ## histos for TDirectory Electron -> LooseElectrons -> MediumVariables
        el_f1_isLoose_h1 = ROOT.TH1F("el_f1_isLoose_h1","el_f1",50,0.,1.)
        histos["el_f1_ifLoose"] = el_f1_isLoose_h1
        el_wstot_isLoose_h1 = ROOT.TH1F("el_wstot_isLoose_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifLoose"] = el_wstot_isLoose_h1
        el_emaxs1_isLoose_h1 = ROOT.TH1F("el_emaxs1_isLoose_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifLoose"] = el_emaxs1_isLoose_h1
        el_nPixHits_isLoose_h1 = ROOT.TH1F("el_nPixHits_isLoose_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifLoose"] = el_nPixHits_isLoose_h1
        el_nSCTHits_isLoose_h1 = ROOT.TH1F("el_nSCTHits_isLoose_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifLoose"] = el_nSCTHits_isLoose_h1
        el_nSiHits_isLoose_h1 = ROOT.TH1F("el_nSiHits_isLoose_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifLoose"] = el_nSiHits_isLoose_h1
        pass

    elif n == 13 : ## histos for TDirectory Electron -> LooseElectrons -> TightVariables       
        el_nBLHits_isLoose_h1 = ROOT.TH1F("el_nBLHits_isLoose_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifLoose"] = el_nBLHits_isLoose_h1
        el_expectHitInBLayer_isLoose_h1 = ROOT.TH1F("el_expectHitInBLayer_isLoose_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifLoose"] = el_expectHitInBLayer_isLoose_h1
        el_cl_E_isLoose_h1 = ROOT.TH1F("el_cl_E_isLoose_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifLoose"] = el_cl_E_isLoose_h1
        el_trackpt_isLoose_h1 = ROOT.TH1F("el_trackpt_isLoose_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifLoose"] = el_trackpt_isLoose_h1
        el_nTRTHits_isLoose_h1 = ROOT.TH1F("el_nTRTHits_isLoose_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifLoose"] = el_nTRTHits_isLoose_h1
        el_TRTHighTHitsRatio_isLoose_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isLoose_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifLoose"] = el_TRTHighTHitsRatio_isLoose_h1
        pass

    
    elif n == 20 : ## histos for TDirectory Electron -> MediumElectrons
        el_charge_ifMedium_h1 = ROOT.TH1F("el_charge_ifMedium_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifMedium"] = el_charge_ifMedium_h1
        el_author_ifMedium_h1 = ROOT.TH1F("el_author_ifMedium_h1","el_author",10,0.,10.)
        histos["el_author_ifMedium"] = el_author_ifMedium_h1
        el_n_ifMedium_h1 = ROOT.TH1F("el_n_ifMedium_h1","el_n",20,0.,20.)
        histos["el_n_ifMedium"] = el_n_ifMedium_h1
        el_eta_ifMedium_h1 = ROOT.TH1F("el_eta_ifMedium_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifMedium"] = el_eta_ifMedium_h1
        el_phi_ifMedium_h1 = ROOT.TH1F("el_phi_ifMedium_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifMedium"] = el_phi_ifMedium_h1
        el_pt_ifMedium_h1 = ROOT.TH1F("el_pt_ifMedium_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifMedium"] = el_pt_ifMedium_h1
        el_loose_ifMedium_h1 = ROOT.TH1F("el_loose_ifMedium_h1","el_medium",2,0.,2.)
        histos["el_loose_ifMedium"] = el_loose_ifMedium_h1
        el_medium_ifMedium_h1 = ROOT.TH1F("el_medium_ifMedium_h1","el_medium",2,0.,2.)
        histos["el_medium_ifMedium"] = el_medium_ifMedium_h1
        el_mediumIso_ifMedium_h1 = ROOT.TH1F("el_mediumIso_ifMedium_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifMedium"] = el_mediumIso_ifMedium_h1
        el_tight_ifMedium_h1 = ROOT.TH1F("el_tight_ifMedium_h1","el_tight",2,0.,2.)
        histos["el_tight_ifMedium"] = el_tight_ifMedium_h1
        el_tightIso_ifMedium_h1 = ROOT.TH1F("el_tightIso_ifMedium_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifMedium"] = el_tightIso_ifMedium_h1
        pass

    elif n == 21 : ## histos for TDirectory Electron -> MediumElectrons -> MediumVariables
        el_weta2_isMedium_h1 = ROOT.TH1F("el_weta2_isMedium_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifMedium"] = el_weta2_isMedium_h1
        el_E237_isMedium_h1 = ROOT.TH1F("el_E237_isMedium_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifMedium"] = el_E237_isMedium_h1
        el_E277_isMedium_h1 = ROOT.TH1F("el_E277_isMedium_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifMedium"] = el_E277_isMedium_h1
        el_Ethad_isMedium_h1 = ROOT.TH1F("el_Ethad_isMedium_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifMedium"] = el_Ethad_isMedium_h1
        el_Ethad1_isMedium_h1 = ROOT.TH1F("el_Ethad1_isMedium_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifMedium"] = el_Ethad1_isMedium_h1
        pass

    elif n == 22 : ## histos for TDirectory Electron -> MediumElectrons -> MediumVariables
        el_f1_isMedium_h1 = ROOT.TH1F("el_f1_isMedium_h1","el_f1",50,0.,1.)
        histos["el_f1_ifMedium"] = el_f1_isMedium_h1
        el_wstot_isMedium_h1 = ROOT.TH1F("el_wstot_isMedium_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifMedium"] = el_wstot_isMedium_h1
        el_emaxs1_isMedium_h1 = ROOT.TH1F("el_emaxs1_isMedium_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifMedium"] = el_emaxs1_isMedium_h1
        el_nPixHits_isMedium_h1 = ROOT.TH1F("el_nPixHits_isMedium_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifMedium"] = el_nPixHits_isMedium_h1
        el_nSCTHits_isMedium_h1 = ROOT.TH1F("el_nSCTHits_isMedium_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifMedium"] = el_nSCTHits_isMedium_h1
        el_nSiHits_isMedium_h1 = ROOT.TH1F("el_nSiHits_isMedium_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifMedium"] = el_nSiHits_isMedium_h1
        pass

    elif n == 23 : ## histos for TDirectory Electron -> MediumElectrons -> TightVariables       
        el_nBLHits_isMedium_h1 = ROOT.TH1F("el_nBLHits_isMedium_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifMedium"] = el_nBLHits_isMedium_h1
        el_expectHitInBLayer_isMedium_h1 = ROOT.TH1F("el_expectHitInBLayer_isMedium_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifMedium"] = el_expectHitInBLayer_isMedium_h1
        el_cl_E_isMedium_h1 = ROOT.TH1F("el_cl_E_isMedium_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifMedium"] = el_cl_E_isMedium_h1
        el_trackpt_isMedium_h1 = ROOT.TH1F("el_trackpt_isMedium_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifMedium"] = el_trackpt_isMedium_h1
        el_nTRTHits_isMedium_h1 = ROOT.TH1F("el_nTRTHits_isMedium_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifMedium"] = el_nTRTHits_isMedium_h1
        el_TRTHighTHitsRatio_isMedium_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isMedium_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifMedium"] = el_TRTHighTHitsRatio_isMedium_h1
        pass
    
    
    elif n == 30 : ## histos for TDirectory Electron -> TightElectrons
        el_charge_ifTight_h1 = ROOT.TH1F("el_charge_ifTight_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifTight"] = el_charge_ifTight_h1
        el_author_ifTight_h1 = ROOT.TH1F("el_author_ifTight_h1","el_author",10,0.,10.)
        histos["el_author_ifTight"] = el_author_ifTight_h1
        el_n_ifTight_h1 = ROOT.TH1F("el_n_ifTight_h1","el_n",20,0.,20.)
        histos["el_n_ifTight"] = el_n_ifTight_h1
        el_eta_ifTight_h1 = ROOT.TH1F("el_eta_ifTight_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifTight"] = el_eta_ifTight_h1
        el_phi_ifTight_h1 = ROOT.TH1F("el_phi_ifTight_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifTight"] = el_phi_ifTight_h1
        el_pt_ifTight_h1 = ROOT.TH1F("el_pt_ifTight_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifTight"] = el_pt_ifTight_h1
        el_loose_ifTight_h1 = ROOT.TH1F("el_loose_ifTight_h1","el_tight",2,0.,2.)
        histos["el_loose_ifTight"] = el_loose_ifTight_h1
        el_medium_ifTight_h1 = ROOT.TH1F("el_medium_ifTight_h1","el_medium",2,0.,2.)
        histos["el_medium_ifTight"] = el_medium_ifTight_h1
        el_mediumIso_ifTight_h1 = ROOT.TH1F("el_mediumIso_ifTight_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifTight"] = el_mediumIso_ifTight_h1
        el_tight_ifTight_h1 = ROOT.TH1F("el_tight_ifTight_h1","el_tight",2,0.,2.)
        histos["el_tight_ifTight"] = el_tight_ifTight_h1
        el_tightIso_ifTight_h1 = ROOT.TH1F("el_tightIso_ifTight_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifTight"] = el_tightIso_ifTight_h1
        pass

    elif n == 31 : ## histos for TDirectory Electron -> TightElectrons -> TightVariables
        el_weta2_isTight_h1 = ROOT.TH1F("el_weta2_isTight_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifTight"] = el_weta2_isTight_h1
        el_E237_isTight_h1 = ROOT.TH1F("el_E237_isTight_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifTight"] = el_E237_isTight_h1
        el_E277_isTight_h1 = ROOT.TH1F("el_E277_isTight_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifTight"] = el_E277_isTight_h1
        el_Ethad_isTight_h1 = ROOT.TH1F("el_Ethad_isTight_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifTight"] = el_Ethad_isTight_h1
        el_Ethad1_isTight_h1 = ROOT.TH1F("el_Ethad1_isTight_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifTight"] = el_Ethad1_isTight_h1
        pass

    elif n == 32 : ## histos for TDirectory Electron -> TightElectrons -> MediumVariables
        el_f1_isTight_h1 = ROOT.TH1F("el_f1_isTight_h1","el_f1",50,0.,1.)
        histos["el_f1_ifTight"] = el_f1_isTight_h1
        el_wstot_isTight_h1 = ROOT.TH1F("el_wstot_isTight_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifTight"] = el_wstot_isTight_h1
        el_emaxs1_isTight_h1 = ROOT.TH1F("el_emaxs1_isTight_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifTight"] = el_emaxs1_isTight_h1
        el_nPixHits_isTight_h1 = ROOT.TH1F("el_nPixHits_isTight_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifTight"] = el_nPixHits_isTight_h1
        el_nSCTHits_isTight_h1 = ROOT.TH1F("el_nSCTHits_isTight_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifTight"] = el_nSCTHits_isTight_h1
        el_nSiHits_isTight_h1 = ROOT.TH1F("el_nSiHits_isTight_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifTight"] = el_nSiHits_isTight_h1
        pass

    elif n == 33 : ## histos for TDirectory Electron -> TightElectrons -> TightVariables       
        el_nBLHits_isTight_h1 = ROOT.TH1F("el_nBLHits_isTight_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifTight"] = el_nBLHits_isTight_h1
        el_expectHitInBLayer_isTight_h1 = ROOT.TH1F("el_expectHitInBLayer_isTight_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifTight"] = el_expectHitInBLayer_isTight_h1
        el_cl_E_isTight_h1 = ROOT.TH1F("el_cl_E_isTight_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifTight"] = el_cl_E_isTight_h1
        el_trackpt_isTight_h1 = ROOT.TH1F("el_trackpt_isTight_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifTight"] = el_trackpt_isTight_h1
        el_nTRTHits_isTight_h1 = ROOT.TH1F("el_nTRTHits_isTight_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifTight"] = el_nTRTHits_isTight_h1
        el_TRTHighTHitsRatio_isTight_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isTight_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifTight"] = el_TRTHighTHitsRatio_isTight_h1
        pass
    
    
    elif n == 40 : ## histos for TDirectory Electron -> Author1Electrons
        el_charge_ifAuthor1_h1 = ROOT.TH1F("el_charge_ifAuthor1_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifAuthor1"] = el_charge_ifAuthor1_h1
        el_author_ifAuthor1_h1 = ROOT.TH1F("el_author_ifAuthor1_h1","el_author",10,0.,10.)
        histos["el_author_ifAuthor1"] = el_author_ifAuthor1_h1
        el_n_ifAuthor1_h1 = ROOT.TH1F("el_n_ifAuthor1_h1","el_n",20,0.,20.)
        histos["el_n_ifAuthor1"] = el_n_ifAuthor1_h1
        el_eta_ifAuthor1_h1 = ROOT.TH1F("el_eta_ifAuthor1_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifAuthor1"] = el_eta_ifAuthor1_h1
        el_phi_ifAuthor1_h1 = ROOT.TH1F("el_phi_ifAuthor1_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifAuthor1"] = el_phi_ifAuthor1_h1
        el_pt_ifAuthor1_h1 = ROOT.TH1F("el_pt_ifAuthor1_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifAuthor1"] = el_pt_ifAuthor1_h1
        el_loose_ifAuthor1_h1 = ROOT.TH1F("el_loose_ifAuthor1_h1","el_author1",2,0.,2.)
        histos["el_loose_ifAuthor1"] = el_loose_ifAuthor1_h1
        el_medium_ifAuthor1_h1 = ROOT.TH1F("el_medium_ifAuthor1_h1","el_medium",2,0.,2.)
        histos["el_medium_ifAuthor1"] = el_medium_ifAuthor1_h1
        el_mediumIso_ifAuthor1_h1 = ROOT.TH1F("el_mediumIso_ifAuthor1_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifAuthor1"] = el_mediumIso_ifAuthor1_h1
        el_tight_ifAuthor1_h1 = ROOT.TH1F("el_tight_ifAuthor1_h1","el_tight",2,0.,2.)
        histos["el_tight_ifAuthor1"] = el_tight_ifAuthor1_h1
        el_tightIso_ifAuthor1_h1 = ROOT.TH1F("el_tightIso_ifAuthor1_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifAuthor1"] = el_tightIso_ifAuthor1_h1
        pass

    elif n == 41 : ## histos for TDirectory Electron -> Author1Electrons -> Author1Variables
        el_weta2_isAuthor1_h1 = ROOT.TH1F("el_weta2_isAuthor1_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifAuthor1"] = el_weta2_isAuthor1_h1
        el_E237_isAuthor1_h1 = ROOT.TH1F("el_E237_isAuthor1_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifAuthor1"] = el_E237_isAuthor1_h1
        el_E277_isAuthor1_h1 = ROOT.TH1F("el_E277_isAuthor1_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifAuthor1"] = el_E277_isAuthor1_h1
        el_Ethad_isAuthor1_h1 = ROOT.TH1F("el_Ethad_isAuthor1_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifAuthor1"] = el_Ethad_isAuthor1_h1
        el_Ethad1_isAuthor1_h1 = ROOT.TH1F("el_Ethad1_isAuthor1_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifAuthor1"] = el_Ethad1_isAuthor1_h1
        pass

    elif n == 42 : ## histos for TDirectory Electron -> Author1Electrons -> MediumVariables
        el_f1_isAuthor1_h1 = ROOT.TH1F("el_f1_isAuthor1_h1","el_f1",50,0.,1.)
        histos["el_f1_ifAuthor1"] = el_f1_isAuthor1_h1
        el_wstot_isAuthor1_h1 = ROOT.TH1F("el_wstot_isAuthor1_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifAuthor1"] = el_wstot_isAuthor1_h1
        el_emaxs1_isAuthor1_h1 = ROOT.TH1F("el_emaxs1_isAuthor1_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifAuthor1"] = el_emaxs1_isAuthor1_h1
        el_nPixHits_isAuthor1_h1 = ROOT.TH1F("el_nPixHits_isAuthor1_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifAuthor1"] = el_nPixHits_isAuthor1_h1
        el_nSCTHits_isAuthor1_h1 = ROOT.TH1F("el_nSCTHits_isAuthor1_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifAuthor1"] = el_nSCTHits_isAuthor1_h1
        el_nSiHits_isAuthor1_h1 = ROOT.TH1F("el_nSiHits_isAuthor1_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifAuthor1"] = el_nSiHits_isAuthor1_h1
        pass

    elif n == 43 : ## histos for TDirectory Electron -> Author1Electrons -> TightVariables       
        el_nBLHits_isAuthor1_h1 = ROOT.TH1F("el_nBLHits_isAuthor1_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifAuthor1"] = el_nBLHits_isAuthor1_h1
        el_expectHitInBLayer_isAuthor1_h1 = ROOT.TH1F("el_expectHitInBLayer_isAuthor1_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifAuthor1"] = el_expectHitInBLayer_isAuthor1_h1
        el_cl_E_isAuthor1_h1 = ROOT.TH1F("el_cl_E_isAuthor1_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifAuthor1"] = el_cl_E_isAuthor1_h1
        el_trackpt_isAuthor1_h1 = ROOT.TH1F("el_trackpt_isAuthor1_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifAuthor1"] = el_trackpt_isAuthor1_h1
        el_nTRTHits_isAuthor1_h1 = ROOT.TH1F("el_nTRTHits_isAuthor1_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifAuthor1"] = el_nTRTHits_isAuthor1_h1
        el_TRTHighTHitsRatio_isAuthor1_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isAuthor1_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifAuthor1"] = el_TRTHighTHitsRatio_isAuthor1_h1
        pass
    
    
    elif n == 50 : ## histos for TDirectory Electron -> Author2Electrons
        el_charge_ifAuthor2_h1 = ROOT.TH1F("el_charge_ifAuthor2_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifAuthor2"] = el_charge_ifAuthor2_h1
        el_author_ifAuthor2_h1 = ROOT.TH1F("el_author_ifAuthor2_h1","el_author",10,0.,10.)
        histos["el_author_ifAuthor2"] = el_author_ifAuthor2_h1
        el_n_ifAuthor2_h1 = ROOT.TH1F("el_n_ifAuthor2_h1","el_n",20,0.,20.)
        histos["el_n_ifAuthor2"] = el_n_ifAuthor2_h1
        el_eta_ifAuthor2_h1 = ROOT.TH1F("el_eta_ifAuthor2_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifAuthor2"] = el_eta_ifAuthor2_h1
        el_phi_ifAuthor2_h1 = ROOT.TH1F("el_phi_ifAuthor2_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifAuthor2"] = el_phi_ifAuthor2_h1
        el_pt_ifAuthor2_h1 = ROOT.TH1F("el_pt_ifAuthor2_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifAuthor2"] = el_pt_ifAuthor2_h1
        el_loose_ifAuthor2_h1 = ROOT.TH1F("el_loose_ifAuthor2_h1","el_author2",2,0.,2.)
        histos["el_loose_ifAuthor2"] = el_loose_ifAuthor2_h1
        el_medium_ifAuthor2_h1 = ROOT.TH1F("el_medium_ifAuthor2_h1","el_medium",2,0.,2.)
        histos["el_medium_ifAuthor2"] = el_medium_ifAuthor2_h1
        el_mediumIso_ifAuthor2_h1 = ROOT.TH1F("el_mediumIso_ifAuthor2_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifAuthor2"] = el_mediumIso_ifAuthor2_h1
        el_tight_ifAuthor2_h1 = ROOT.TH1F("el_tight_ifAuthor2_h1","el_tight",2,0.,2.)
        histos["el_tight_ifAuthor2"] = el_tight_ifAuthor2_h1
        el_tightIso_ifAuthor2_h1 = ROOT.TH1F("el_tightIso_ifAuthor2_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifAuthor2"] = el_tightIso_ifAuthor2_h1
        pass

    elif n == 51 : ## histos for TDirectory Electron -> Author2Electrons -> Author2Variables
        el_weta2_isAuthor2_h1 = ROOT.TH1F("el_weta2_isAuthor2_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifAuthor2"] = el_weta2_isAuthor2_h1
        el_E237_isAuthor2_h1 = ROOT.TH1F("el_E237_isAuthor2_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifAuthor2"] = el_E237_isAuthor2_h1
        el_E277_isAuthor2_h1 = ROOT.TH1F("el_E277_isAuthor2_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifAuthor2"] = el_E277_isAuthor2_h1
        el_Ethad_isAuthor2_h1 = ROOT.TH1F("el_Ethad_isAuthor2_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifAuthor2"] = el_Ethad_isAuthor2_h1
        el_Ethad1_isAuthor2_h1 = ROOT.TH1F("el_Ethad1_isAuthor2_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifAuthor2"] = el_Ethad1_isAuthor2_h1
        pass

    elif n == 52 : ## histos for TDirectory Electron -> Author2Electrons -> MediumVariables
        el_f1_isAuthor2_h1 = ROOT.TH1F("el_f1_isAuthor2_h1","el_f1",50,0.,1.)
        histos["el_f1_ifAuthor2"] = el_f1_isAuthor2_h1
        el_wstot_isAuthor2_h1 = ROOT.TH1F("el_wstot_isAuthor2_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifAuthor2"] = el_wstot_isAuthor2_h1
        el_emaxs1_isAuthor2_h1 = ROOT.TH1F("el_emaxs1_isAuthor2_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifAuthor2"] = el_emaxs1_isAuthor2_h1
        el_nPixHits_isAuthor2_h1 = ROOT.TH1F("el_nPixHits_isAuthor2_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifAuthor2"] = el_nPixHits_isAuthor2_h1
        el_nSCTHits_isAuthor2_h1 = ROOT.TH1F("el_nSCTHits_isAuthor2_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifAuthor2"] = el_nSCTHits_isAuthor2_h1
        el_nSiHits_isAuthor2_h1 = ROOT.TH1F("el_nSiHits_isAuthor2_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifAuthor2"] = el_nSiHits_isAuthor2_h1
        pass

    elif n == 53 : ## histos for TDirectory Electron -> Author2Electrons -> TightVariables       
        el_nBLHits_isAuthor2_h1 = ROOT.TH1F("el_nBLHits_isAuthor2_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifAuthor2"] = el_nBLHits_isAuthor2_h1
        el_expectHitInBLayer_isAuthor2_h1 = ROOT.TH1F("el_expectHitInBLayer_isAuthor2_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifAuthor2"] = el_expectHitInBLayer_isAuthor2_h1
        el_cl_E_isAuthor2_h1 = ROOT.TH1F("el_cl_E_isAuthor2_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifAuthor2"] = el_cl_E_isAuthor2_h1
        el_trackpt_isAuthor2_h1 = ROOT.TH1F("el_trackpt_isAuthor2_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifAuthor2"] = el_trackpt_isAuthor2_h1
        el_nTRTHits_isAuthor2_h1 = ROOT.TH1F("el_nTRTHits_isAuthor2_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifAuthor2"] = el_nTRTHits_isAuthor2_h1
        el_TRTHighTHitsRatio_isAuthor2_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isAuthor2_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifAuthor2"] = el_TRTHighTHitsRatio_isAuthor2_h1
        pass
    
    
    elif n == 60 : ## histos for TDirectory Electron -> Author3Electrons
        el_charge_ifAuthor3_h1 = ROOT.TH1F("el_charge_ifAuthor3_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifAuthor3"] = el_charge_ifAuthor3_h1
        el_author_ifAuthor3_h1 = ROOT.TH1F("el_author_ifAuthor3_h1","el_author",10,0.,10.)
        histos["el_author_ifAuthor3"] = el_author_ifAuthor3_h1
        el_n_ifAuthor3_h1 = ROOT.TH1F("el_n_ifAuthor3_h1","el_n",20,0.,20.)
        histos["el_n_ifAuthor3"] = el_n_ifAuthor3_h1
        el_eta_ifAuthor3_h1 = ROOT.TH1F("el_eta_ifAuthor3_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifAuthor3"] = el_eta_ifAuthor3_h1
        el_phi_ifAuthor3_h1 = ROOT.TH1F("el_phi_ifAuthor3_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifAuthor3"] = el_phi_ifAuthor3_h1
        el_pt_ifAuthor3_h1 = ROOT.TH1F("el_pt_ifAuthor3_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifAuthor3"] = el_pt_ifAuthor3_h1
        el_loose_ifAuthor3_h1 = ROOT.TH1F("el_loose_ifAuthor3_h1","el_author3",2,0.,2.)
        histos["el_loose_ifAuthor3"] = el_loose_ifAuthor3_h1
        el_medium_ifAuthor3_h1 = ROOT.TH1F("el_medium_ifAuthor3_h1","el_medium",2,0.,2.)
        histos["el_medium_ifAuthor3"] = el_medium_ifAuthor3_h1
        el_mediumIso_ifAuthor3_h1 = ROOT.TH1F("el_mediumIso_ifAuthor3_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifAuthor3"] = el_mediumIso_ifAuthor3_h1
        el_tight_ifAuthor3_h1 = ROOT.TH1F("el_tight_ifAuthor3_h1","el_tight",2,0.,2.)
        histos["el_tight_ifAuthor3"] = el_tight_ifAuthor3_h1
        el_tightIso_ifAuthor3_h1 = ROOT.TH1F("el_tightIso_ifAuthor3_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifAuthor3"] = el_tightIso_ifAuthor3_h1
        pass

    elif n == 61 : ## histos for TDirectory Electron -> Author3Electrons -> Author3Variables
        el_weta2_isAuthor3_h1 = ROOT.TH1F("el_weta2_isAuthor3_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifAuthor3"] = el_weta2_isAuthor3_h1
        el_E237_isAuthor3_h1 = ROOT.TH1F("el_E237_isAuthor3_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifAuthor3"] = el_E237_isAuthor3_h1
        el_E277_isAuthor3_h1 = ROOT.TH1F("el_E277_isAuthor3_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifAuthor3"] = el_E277_isAuthor3_h1
        el_Ethad_isAuthor3_h1 = ROOT.TH1F("el_Ethad_isAuthor3_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifAuthor3"] = el_Ethad_isAuthor3_h1
        el_Ethad1_isAuthor3_h1 = ROOT.TH1F("el_Ethad1_isAuthor3_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifAuthor3"] = el_Ethad1_isAuthor3_h1
        pass

    elif n == 62 : ## histos for TDirectory Electron -> Author3Electrons -> MediumVariables
        el_f1_isAuthor3_h1 = ROOT.TH1F("el_f1_isAuthor3_h1","el_f1",50,0.,1.)
        histos["el_f1_ifAuthor3"] = el_f1_isAuthor3_h1
        el_wstot_isAuthor3_h1 = ROOT.TH1F("el_wstot_isAuthor3_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifAuthor3"] = el_wstot_isAuthor3_h1
        el_emaxs1_isAuthor3_h1 = ROOT.TH1F("el_emaxs1_isAuthor3_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifAuthor3"] = el_emaxs1_isAuthor3_h1
        el_nPixHits_isAuthor3_h1 = ROOT.TH1F("el_nPixHits_isAuthor3_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifAuthor3"] = el_nPixHits_isAuthor3_h1
        el_nSCTHits_isAuthor3_h1 = ROOT.TH1F("el_nSCTHits_isAuthor3_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifAuthor3"] = el_nSCTHits_isAuthor3_h1
        el_nSiHits_isAuthor3_h1 = ROOT.TH1F("el_nSiHits_isAuthor3_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifAuthor3"] = el_nSiHits_isAuthor3_h1
        pass

    elif n == 63 : ## histos for TDirectory Electron -> Author3Electrons -> TightVariables       
        el_nBLHits_isAuthor3_h1 = ROOT.TH1F("el_nBLHits_isAuthor3_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifAuthor3"] = el_nBLHits_isAuthor3_h1
        el_expectHitInBLayer_isAuthor3_h1 = ROOT.TH1F("el_expectHitInBLayer_isAuthor3_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifAuthor3"] = el_expectHitInBLayer_isAuthor3_h1
        el_cl_E_isAuthor3_h1 = ROOT.TH1F("el_cl_E_isAuthor3_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifAuthor3"] = el_cl_E_isAuthor3_h1
        el_trackpt_isAuthor3_h1 = ROOT.TH1F("el_trackpt_isAuthor3_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifAuthor3"] = el_trackpt_isAuthor3_h1
        el_nTRTHits_isAuthor3_h1 = ROOT.TH1F("el_nTRTHits_isAuthor3_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifAuthor3"] = el_nTRTHits_isAuthor3_h1
        el_TRTHighTHitsRatio_isAuthor3_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isAuthor3_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifAuthor3"] = el_TRTHighTHitsRatio_isAuthor3_h1
        pass
    
    
    elif n == 70 : ## histos for TDirectory Electron -> Author8Electrons
        el_charge_ifAuthor8_h1 = ROOT.TH1F("el_charge_ifAuthor8_h1","el_charge",3,-1.5,1.5)
        histos["el_charge_ifAuthor8"] = el_charge_ifAuthor8_h1
        el_author_ifAuthor8_h1 = ROOT.TH1F("el_author_ifAuthor8_h1","el_author",10,0.,10.)
        histos["el_author_ifAuthor8"] = el_author_ifAuthor8_h1
        el_n_ifAuthor8_h1 = ROOT.TH1F("el_n_ifAuthor8_h1","el_n",20,0.,20.)
        histos["el_n_ifAuthor8"] = el_n_ifAuthor8_h1
        el_eta_ifAuthor8_h1 = ROOT.TH1F("el_eta_ifAuthor8_h1","el_eta",100,-4.5,4.5)
        histos["el_eta_ifAuthor8"] = el_eta_ifAuthor8_h1
        el_phi_ifAuthor8_h1 = ROOT.TH1F("el_phi_ifAuthor8_h1","el_phi",100,-4.5,4.5)
        histos["el_phi_ifAuthor8"] = el_phi_ifAuthor8_h1
        el_pt_ifAuthor8_h1 = ROOT.TH1F("el_pt_ifAuthor8_h1","el_pt",100,0.,100000.)
        histos["el_pt_ifAuthor8"] = el_pt_ifAuthor8_h1
        el_loose_ifAuthor8_h1 = ROOT.TH1F("el_loose_ifAuthor8_h1","el_author8",2,0.,2.)
        histos["el_loose_ifAuthor8"] = el_loose_ifAuthor8_h1
        el_medium_ifAuthor8_h1 = ROOT.TH1F("el_medium_ifAuthor8_h1","el_medium",2,0.,2.)
        histos["el_medium_ifAuthor8"] = el_medium_ifAuthor8_h1
        el_mediumIso_ifAuthor8_h1 = ROOT.TH1F("el_mediumIso_ifAuthor8_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso_ifAuthor8"] = el_mediumIso_ifAuthor8_h1
        el_tight_ifAuthor8_h1 = ROOT.TH1F("el_tight_ifAuthor8_h1","el_tight",2,0.,2.)
        histos["el_tight_ifAuthor8"] = el_tight_ifAuthor8_h1
        el_tightIso_ifAuthor8_h1 = ROOT.TH1F("el_tightIso_ifAuthor8_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso_ifAuthor8"] = el_tightIso_ifAuthor8_h1
        pass

    elif n == 71 : ## histos for TDirectory Electron -> Author8Electrons -> Author8Variables
        el_weta2_isAuthor8_h1 = ROOT.TH1F("el_weta2_isAuthor8_h1","el_weta2",100,-0.05,0.05)
        histos["el_weta2_ifAuthor8"] = el_weta2_isAuthor8_h1
        el_E237_isAuthor8_h1 = ROOT.TH1F("el_E237_isAuthor8_h1","el_E237",100,0.,200000.)
        histos["el_E237_ifAuthor8"] = el_E237_isAuthor8_h1
        el_E277_isAuthor8_h1 = ROOT.TH1F("el_E277_isAuthor8_h1","el_E277",100,0.,200000.)
        histos["el_E277_ifAuthor8"] = el_E277_isAuthor8_h1
        el_Ethad_isAuthor8_h1 = ROOT.TH1F("el_Ethad_isAuthor8_h1","el_Ethad",100,0.,10000.)
        histos["el_Ethad_ifAuthor8"] = el_Ethad_isAuthor8_h1
        el_Ethad1_isAuthor8_h1 = ROOT.TH1F("el_Ethad1_isAuthor8_h1","el_Ethad1",100,0.,10000.)
        histos["el_Ethad1_ifAuthor8"] = el_Ethad1_isAuthor8_h1
        pass

    elif n == 72 : ## histos for TDirectory Electron -> Author8Electrons -> MediumVariables
        el_f1_isAuthor8_h1 = ROOT.TH1F("el_f1_isAuthor8_h1","el_f1",50,0.,1.)
        histos["el_f1_ifAuthor8"] = el_f1_isAuthor8_h1
        el_wstot_isAuthor8_h1 = ROOT.TH1F("el_wstot_isAuthor8_h1","el_wstot",100,0.,10.)
        histos["el_wstot_ifAuthor8"] = el_wstot_isAuthor8_h1
        el_emaxs1_isAuthor8_h1 = ROOT.TH1F("el_emaxs1_isAuthor8_h1","el_emaxs1",100,-5000.,50000.)
        histos["el_emaxs1_ifAuthor8"] = el_emaxs1_isAuthor8_h1
        el_nPixHits_isAuthor8_h1 = ROOT.TH1F("el_nPixHits_isAuthor8_h1","el_nPixHits",10,0.,10.)
        histos["el_nPixHits_ifAuthor8"] = el_nPixHits_isAuthor8_h1
        el_nSCTHits_isAuthor8_h1 = ROOT.TH1F("el_nSCTHits_isAuthor8_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits_ifAuthor8"] = el_nSCTHits_isAuthor8_h1
        el_nSiHits_isAuthor8_h1 = ROOT.TH1F("el_nSiHits_isAuthor8_h1","el_nSiHits",20,0.,20.)
        histos["el_nSiHits_ifAuthor8"] = el_nSiHits_isAuthor8_h1
        pass

    elif n == 73 : ## histos for TDirectory Electron -> Author8Electrons -> TightVariables       
        el_nBLHits_isAuthor8_h1 = ROOT.TH1F("el_nBLHits_isAuthor8_h1","el_nBLHits",5,0.,5.)
        histos["el_nBLHits_ifAuthor8"] = el_nBLHits_isAuthor8_h1
        el_expectHitInBLayer_isAuthor8_h1 = ROOT.TH1F("el_expectHitInBLayer_isAuthor8_h1","el_expectHitInBLayer",100,-1100.,100.)
        histos["el_expectHitInBLayer_ifAuthor8"] = el_expectHitInBLayer_isAuthor8_h1
        el_cl_E_isAuthor8_h1 = ROOT.TH1F("el_cl_E_isAuthor8_h1","el_cl_E",100,0.,250000.)
        histos["el_cl_E_ifAuthor8"] = el_cl_E_isAuthor8_h1
        el_trackpt_isAuthor8_h1 = ROOT.TH1F("el_trackpt_isAuthor8_h1","el_trackpt",100,0.,10000.)
        histos["el_trackpt_ifAuthor8"] = el_trackpt_isAuthor8_h1
        el_nTRTHits_isAuthor8_h1 = ROOT.TH1F("el_nTRTHits_isAuthor8_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits_ifAuthor8"] = el_nTRTHits_isAuthor8_h1
        el_TRTHighTHitsRatio_isAuthor8_h1 = ROOT.TH1F("el_TRTHighTHitsRatio_isAuthor8_h1","el_TRTHighTHitsRatio",100,0.,1.)
        histos["el_TRTHighTHitsRatio_ifAuthor8"] = el_TRTHighTHitsRatio_isAuthor8_h1
        pass
    
    else : ## ERROR
        print "::: ERROR wrong flag :::"
        pass
    return histos

def defHistPho(n,histos) :

    if n == 0 : ## histos for TDirectory Photons -> AllPhotons
        ph_E_h1 = ROOT.TH1F("ph_E_h1","ph_E",100,0.,100000.)
        histos["ph_E"] = ph_E_h1
        ph_author_h1 = ROOT.TH1F("ph_author_h1","ph_author",17,0.,17.)
        histos["ph_author"] = ph_author_h1
        ph_eta_h1 = ROOT.TH1F("ph_eta_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta"] = ph_eta_h1
        ph_loose_h1 = ROOT.TH1F("ph_loose_h1","ph_loose",2,0.,2.)
        histos["ph_loose"] = ph_loose_h1
        ph_n_h1 = ROOT.TH1F("ph_n_h1","ph_n",20,0.,20.)
        histos["ph_n"] = ph_n_h1
        ph_pt_h1 = ROOT.TH1F("ph_pt_h1","ph_pt",100,0.,100000.)
        histos["ph_pt"] = ph_pt_h1
        ph_phi_h1 = ROOT.TH1F("ph_phi_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi"] = ph_phi_h1
        ph_tight_h1 = ROOT.TH1F("ph_tight_h1","ph_tight",2,0.,2.)
        histos["ph_tight"] = ph_tight_h1
        ph_tightIso_h1 = ROOT.TH1F("ph_tightIso_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso"] = ph_tightIso_h1
        ph_isConv_h1 = ROOT.TH1F("ph_isConv_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv"] = ph_isConv_h1
        ph_nConv_h1 = ROOT.TH1F("ph_nConv_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv"] = ph_nConv_h1
        pass
    
    elif n == 1 : ## histos for TDirectory Photons -> AllPhotons -> LooseVariables
        ph_Es2_h1 = ROOT.TH1F("ph_Es2_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2"] = ph_Es2_h1
        ph_E233_h1 = ROOT.TH1F("ph_E233_h1","ph_E233",100,0.,250000.)
        histos["ph_E233"] = ph_E233_h1
        ph_E237_h1 = ROOT.TH1F("ph_E237_h1","ph_E237",100,0.,250000.)
        histos["ph_E237"] = ph_E237_h1
        ph_E277_h1 = ROOT.TH1F("ph_E277_h1","ph_E277",100,0.,250000.)
        histos["ph_E277"] = ph_E277_h1
        ph_weta2_h1 = ROOT.TH1F("ph_weta2_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2"] = ph_weta2_h1
        ph_Ethad_h1 = ROOT.TH1F("ph_Ethad_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad"] = ph_Ethad_h1
        ph_Ethad1_h1 = ROOT.TH1F("ph_Ethad1_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1"] = ph_Ethad1_h1
        pass
    
    elif n == 2 : ## histos for TDirectory Photons -> AllPhotons -> TightVariables
        ph_wstot_h1 = ROOT.TH1F("ph_wstot_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot"] = ph_wstot_h1
        ph_deltaEmax2_h1 = ROOT.TH1F("ph_deltaEmax2_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2"] = ph_deltaEmax2_h1
        ph_emaxs1_h1 = ROOT.TH1F("ph_emaxs1_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1"] = ph_emaxs1_h1
        ph_deltaEs_h1 = ROOT.TH1F("ph_deltaEs_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs"] = ph_deltaEs_h1
        ph_cl_E_h1 = ROOT.TH1F("ph_cl_E_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E"] = ph_cl_E_h1
        ph_f1_h1 = ROOT.TH1F("ph_f1_h1","ph_f1",100,0.,1.)
        histos["ph_f1"] = ph_f1_h1
        pass
    
    elif n == 10 : ## histos for TDirectory Photons -> LoosePhotons
        ph_E_ifLoose_h1 = ROOT.TH1F("ph_E_ifLoose_h1","ph_E",100,0.,100000.)
        histos["ph_E_ifLoose"] = ph_E_ifLoose_h1
        ph_author_ifLoose_h1 = ROOT.TH1F("ph_author_ifLoose_h1","ph_author",17,0.,17.)
        histos["ph_author_ifLoose"] = ph_author_ifLoose_h1
        ph_eta_ifLoose_h1 = ROOT.TH1F("ph_eta_ifLoose_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta_ifLoose"] = ph_eta_ifLoose_h1
        ph_loose_ifLoose_h1 = ROOT.TH1F("ph_loose_ifLoose_h1","ph_loose",2,0.,2.)
        histos["ph_loose_ifLoose"] = ph_loose_ifLoose_h1
        ph_n_ifLoose_h1 = ROOT.TH1F("ph_n_ifLoose_h1","ph_n",20,0.,20.)
        histos["ph_n_ifLoose"] = ph_n_ifLoose_h1
        ph_pt_ifLoose_h1 = ROOT.TH1F("ph_pt_ifLoose_h1","ph_pt",100,0.,100000.)
        histos["ph_pt_ifLoose"] = ph_pt_ifLoose_h1
        ph_phi_ifLoose_h1 = ROOT.TH1F("ph_phi_ifLoose_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi_ifLoose"] = ph_phi_ifLoose_h1
        ph_tight_ifLoose_h1 = ROOT.TH1F("ph_tight_ifLoose_h1","ph_tight",2,0.,2.)
        histos["ph_tight_ifLoose"] = ph_tight_ifLoose_h1
        ph_tightIso_ifLoose_h1 = ROOT.TH1F("ph_tightIso_ifLoose_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso_ifLoose"] = ph_tightIso_ifLoose_h1
        ph_isConv_ifLoose_h1 = ROOT.TH1F("ph_isConv_ifLoose_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv_ifLoose"] = ph_isConv_ifLoose_h1
        ph_nConv_ifLoose_h1 = ROOT.TH1F("ph_nConv_ifLoose_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv_ifLoose"] = ph_nConv_ifLoose_h1
        pass
    elif n == 11 : ## histos for TDirectory Photons -> LoosePhotons -> LooseVariables
        ph_Es2_ifLoose_h1 = ROOT.TH1F("ph_Es2_ifLoose_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2_ifLoose"] = ph_Es2_ifLoose_h1
        ph_E233_ifLoose_h1 = ROOT.TH1F("ph_E233_ifLoose_h1","ph_E233",100,0.,250000.)
        histos["ph_E233_ifLoose"] = ph_E233_ifLoose_h1
        ph_E237_ifLoose_h1 = ROOT.TH1F("ph_E237_ifLoose_h1","ph_E237",100,0.,250000.)
        histos["ph_E237_ifLoose"] = ph_E237_ifLoose_h1
        ph_E277_ifLoose_h1 = ROOT.TH1F("ph_E277_ifLoose_h1","ph_E277",100,0.,250000.)
        histos["ph_E277_ifLoose"] = ph_E277_ifLoose_h1
        ph_weta2_ifLoose_h1 = ROOT.TH1F("ph_weta2_ifLoose_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2_ifLoose"] = ph_weta2_ifLoose_h1
        ph_Ethad_ifLoose_h1 = ROOT.TH1F("ph_Ethad_ifLoose_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad_ifLoose"] = ph_Ethad_ifLoose_h1
        ph_Ethad1_ifLoose_h1 = ROOT.TH1F("ph_Ethad1_ifLoose_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1_ifLoose"] = ph_Ethad1_ifLoose_h1
        pass
    elif n == 12 : ## histos for TDirectory Photons -> LoosePhotons -> TightVariables
        ph_wstot_ifLoose_h1 = ROOT.TH1F("ph_wstot_ifLoose_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot_ifLoose"] = ph_wstot_ifLoose_h1
        ph_deltaEmax2_ifLoose_h1 = ROOT.TH1F("ph_deltaEmax2_ifLoose_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2_ifLoose"] = ph_deltaEmax2_ifLoose_h1
        ph_emaxs1_ifLoose_h1 = ROOT.TH1F("ph_emaxs1_ifLoose_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1_ifLoose"] = ph_emaxs1_ifLoose_h1
        ph_deltaEs_ifLoose_h1 = ROOT.TH1F("ph_deltaEs_ifLoose_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs_ifLoose"] = ph_deltaEs_ifLoose_h1
        ph_cl_E_ifLoose_h1 = ROOT.TH1F("ph_cl_E_ifLoose_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E_ifLoose"] = ph_cl_E_ifLoose_h1
        ph_f1_ifLoose_h1 = ROOT.TH1F("ph_f1_ifLoose_h1","ph_f1",100,0.,1.)
        histos["ph_f1_ifLoose"] = ph_f1_ifLoose_h1
        pass
    
    elif n == 20 : ## histos for TDirectory Photons -> TightPhotons
        ph_E_ifTight_h1 = ROOT.TH1F("ph_E_ifTight_h1","ph_E",100,0.,100000.)
        histos["ph_E_ifTight"] = ph_E_ifTight_h1
        ph_author_ifTight_h1 = ROOT.TH1F("ph_author_ifTight_h1","ph_author",17,0.,17.)
        histos["ph_author_ifTight"] = ph_author_ifTight_h1
        ph_eta_ifTight_h1 = ROOT.TH1F("ph_eta_ifTight_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta_ifTight"] = ph_eta_ifTight_h1
        ph_loose_ifTight_h1 = ROOT.TH1F("ph_loose_ifTight_h1","ph_loose",2,0.,2.)
        histos["ph_loose_ifTight"] = ph_loose_ifTight_h1
        ph_n_ifTight_h1 = ROOT.TH1F("ph_n_ifTight_h1","ph_n",20,0.,20.)
        histos["ph_n_ifTight"] = ph_n_ifTight_h1
        ph_pt_ifTight_h1 = ROOT.TH1F("ph_pt_ifTight_h1","ph_pt",100,0.,100000.)
        histos["ph_pt_ifTight"] = ph_pt_ifTight_h1
        ph_phi_ifTight_h1 = ROOT.TH1F("ph_phi_ifTight_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi_ifTight"] = ph_phi_ifTight_h1
        ph_tight_ifTight_h1 = ROOT.TH1F("ph_tight_ifTight_h1","ph_tight",2,0.,2.)
        histos["ph_tight_ifTight"] = ph_tight_ifTight_h1
        ph_tightIso_ifTight_h1 = ROOT.TH1F("ph_tightIso_ifTight_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso_ifTight"] = ph_tightIso_ifTight_h1
        ph_isConv_ifTight_h1 = ROOT.TH1F("ph_isConv_ifTight_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv_ifTight"] = ph_isConv_ifTight_h1
        ph_nConv_ifTight_h1 = ROOT.TH1F("ph_nConv_ifTight_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv_ifTight"] = ph_nConv_ifTight_h1
        pass
    elif n == 21 : ## histos for TDirectory Photons -> TightPhotons -> LooseVariables
        ph_Es2_ifTight_h1 = ROOT.TH1F("ph_Es2_ifTight_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2_ifTight"] = ph_Es2_ifTight_h1
        ph_E233_ifTight_h1 = ROOT.TH1F("ph_E233_ifTight_h1","ph_E233",100,0.,250000.)
        histos["ph_E233_ifTight"] = ph_E233_ifTight_h1
        ph_E237_ifTight_h1 = ROOT.TH1F("ph_E237_ifTight_h1","ph_E237",100,0.,250000.)
        histos["ph_E237_ifTight"] = ph_E237_ifTight_h1
        ph_E277_ifTight_h1 = ROOT.TH1F("ph_E277_ifTight_h1","ph_E277",100,0.,250000.)
        histos["ph_E277_ifTight"] = ph_E277_ifTight_h1
        ph_weta2_ifTight_h1 = ROOT.TH1F("ph_weta2_ifTight_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2_ifTight"] = ph_weta2_ifTight_h1
        ph_Ethad_ifTight_h1 = ROOT.TH1F("ph_Ethad_ifTight_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad_ifTight"] = ph_Ethad_ifTight_h1
        ph_Ethad1_ifTight_h1 = ROOT.TH1F("ph_Ethad1_ifTight_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1_ifTight"] = ph_Ethad1_ifTight_h1
        pass
    elif n == 22 : ## histos for TDirectory Photons -> TightPhotons -> TightVariables
        ph_wstot_ifTight_h1 = ROOT.TH1F("ph_wstot_ifTight_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot_ifTight"] = ph_wstot_ifTight_h1
        ph_deltaEmax2_ifTight_h1 = ROOT.TH1F("ph_deltaEmax2_ifTight_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2_ifTight"] = ph_deltaEmax2_ifTight_h1
        ph_emaxs1_ifTight_h1 = ROOT.TH1F("ph_emaxs1_ifTight_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1_ifTight"] = ph_emaxs1_ifTight_h1
        ph_deltaEs_ifTight_h1 = ROOT.TH1F("ph_deltaEs_ifTight_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs_ifTight"] = ph_deltaEs_ifTight_h1
        ph_cl_E_ifTight_h1 = ROOT.TH1F("ph_cl_E_ifTight_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E_ifTight"] = ph_cl_E_ifTight_h1
        ph_f1_ifTight_h1 = ROOT.TH1F("ph_f1_ifTight_h1","ph_f1",100,0.,1.)
        histos["ph_f1_ifTight"] = ph_f1_ifTight_h1
        pass
    
    elif n == 30 : ## histos for TDirectory Photons -> Author4Photons
        ph_E_ifAuthor4_h1 = ROOT.TH1F("ph_E_ifAuthor4_h1","ph_E",100,0.,100000.)
        histos["ph_E_ifAuthor4"] = ph_E_ifAuthor4_h1
        ph_author_ifAuthor4_h1 = ROOT.TH1F("ph_author_ifAuthor4_h1","ph_author",17,0.,17.)
        histos["ph_author_ifAuthor4"] = ph_author_ifAuthor4_h1
        ph_eta_ifAuthor4_h1 = ROOT.TH1F("ph_eta_ifAuthor4_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta_ifAuthor4"] = ph_eta_ifAuthor4_h1
        ph_loose_ifAuthor4_h1 = ROOT.TH1F("ph_loose_ifAuthor4_h1","ph_loose",2,0.,2.)
        histos["ph_loose_ifAuthor4"] = ph_loose_ifAuthor4_h1
        ph_n_ifAuthor4_h1 = ROOT.TH1F("ph_n_ifAuthor4_h1","ph_n",20,0.,20.)
        histos["ph_n_ifAuthor4"] = ph_n_ifAuthor4_h1
        ph_pt_ifAuthor4_h1 = ROOT.TH1F("ph_pt_ifAuthor4_h1","ph_pt",100,0.,100000.)
        histos["ph_pt_ifAuthor4"] = ph_pt_ifAuthor4_h1
        ph_phi_ifAuthor4_h1 = ROOT.TH1F("ph_phi_ifAuthor4_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi_ifAuthor4"] = ph_phi_ifAuthor4_h1
        ph_tight_ifAuthor4_h1 = ROOT.TH1F("ph_tight_ifAuthor4_h1","ph_tight",2,0.,2.)
        histos["ph_tight_ifAuthor4"] = ph_tight_ifAuthor4_h1
        ph_tightIso_ifAuthor4_h1 = ROOT.TH1F("ph_tightIso_ifAuthor4_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso_ifAuthor4"] = ph_tightIso_ifAuthor4_h1
        ph_isConv_ifAuthor4_h1 = ROOT.TH1F("ph_isConv_ifAuthor4_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv_ifAuthor4"] = ph_isConv_ifAuthor4_h1
        ph_nConv_ifAuthor4_h1 = ROOT.TH1F("ph_nConv_ifAuthor4_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv_ifAuthor4"] = ph_nConv_ifAuthor4_h1
        pass
    elif n == 31 : ## histos for TDirectory Photons -> Author4Photons -> LooseVariables
        ph_Es2_ifAuthor4_h1 = ROOT.TH1F("ph_Es2_ifAuthor4_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2_ifAuthor4"] = ph_Es2_ifAuthor4_h1
        ph_E233_ifAuthor4_h1 = ROOT.TH1F("ph_E233_ifAuthor4_h1","ph_E233",100,0.,250000.)
        histos["ph_E233_ifAuthor4"] = ph_E233_ifAuthor4_h1
        ph_E237_ifAuthor4_h1 = ROOT.TH1F("ph_E237_ifAuthor4_h1","ph_E237",100,0.,250000.)
        histos["ph_E237_ifAuthor4"] = ph_E237_ifAuthor4_h1
        ph_E277_ifAuthor4_h1 = ROOT.TH1F("ph_E277_ifAuthor4_h1","ph_E277",100,0.,250000.)
        histos["ph_E277_ifAuthor4"] = ph_E277_ifAuthor4_h1
        ph_weta2_ifAuthor4_h1 = ROOT.TH1F("ph_weta2_ifAuthor4_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2_ifAuthor4"] = ph_weta2_ifAuthor4_h1
        ph_Ethad_ifAuthor4_h1 = ROOT.TH1F("ph_Ethad_ifAuthor4_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad_ifAuthor4"] = ph_Ethad_ifAuthor4_h1
        ph_Ethad1_ifAuthor4_h1 = ROOT.TH1F("ph_Ethad1_ifAuthor4_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1_ifAuthor4"] = ph_Ethad1_ifAuthor4_h1
        pass
    elif n == 32 : ## histos for TDirectory Photons -> Author4Photons -> TightVariables
        ph_wstot_ifAuthor4_h1 = ROOT.TH1F("ph_wstot_ifAuthor4_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot_ifAuthor4"] = ph_wstot_ifAuthor4_h1
        ph_deltaEmax2_ifAuthor4_h1 = ROOT.TH1F("ph_deltaEmax2_ifAuthor4_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2_ifAuthor4"] = ph_deltaEmax2_ifAuthor4_h1
        ph_emaxs1_ifAuthor4_h1 = ROOT.TH1F("ph_emaxs1_ifAuthor4_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1_ifAuthor4"] = ph_emaxs1_ifAuthor4_h1
        ph_deltaEs_ifAuthor4_h1 = ROOT.TH1F("ph_deltaEs_ifAuthor4_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs_ifAuthor4"] = ph_deltaEs_ifAuthor4_h1
        ph_cl_E_ifAuthor4_h1 = ROOT.TH1F("ph_cl_E_ifAuthor4_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E_ifAuthor4"] = ph_cl_E_ifAuthor4_h1
        ph_f1_ifAuthor4_h1 = ROOT.TH1F("ph_f1_ifAuthor4_h1","ph_f1",100,0.,1.)
        histos["ph_f1_ifAuthor4"] = ph_f1_ifAuthor4_h1
        pass
    
    elif n == 40 : ## histos for TDirectory Photons -> Author16Photons
        ph_E_ifAuthor16_h1 = ROOT.TH1F("ph_E_ifAuthor16_h1","ph_E",100,0.,100000.)
        histos["ph_E_ifAuthor16"] = ph_E_ifAuthor16_h1
        ph_author_ifAuthor16_h1 = ROOT.TH1F("ph_author_ifAuthor16_h1","ph_author",17,0.,17.)
        histos["ph_author_ifAuthor16"] = ph_author_ifAuthor16_h1
        ph_eta_ifAuthor16_h1 = ROOT.TH1F("ph_eta_ifAuthor16_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta_ifAuthor16"] = ph_eta_ifAuthor16_h1
        ph_loose_ifAuthor16_h1 = ROOT.TH1F("ph_loose_ifAuthor16_h1","ph_loose",2,0.,2.)
        histos["ph_loose_ifAuthor16"] = ph_loose_ifAuthor16_h1
        ph_n_ifAuthor16_h1 = ROOT.TH1F("ph_n_ifAuthor16_h1","ph_n",20,0.,20.)
        histos["ph_n_ifAuthor16"] = ph_n_ifAuthor16_h1
        ph_pt_ifAuthor16_h1 = ROOT.TH1F("ph_pt_ifAuthor16_h1","ph_pt",100,0.,100000.)
        histos["ph_pt_ifAuthor16"] = ph_pt_ifAuthor16_h1
        ph_phi_ifAuthor16_h1 = ROOT.TH1F("ph_phi_ifAuthor16_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi_ifAuthor16"] = ph_phi_ifAuthor16_h1
        ph_tight_ifAuthor16_h1 = ROOT.TH1F("ph_tight_ifAuthor16_h1","ph_tight",2,0.,2.)
        histos["ph_tight_ifAuthor16"] = ph_tight_ifAuthor16_h1
        ph_tightIso_ifAuthor16_h1 = ROOT.TH1F("ph_tightIso_ifAuthor16_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso_ifAuthor16"] = ph_tightIso_ifAuthor16_h1
        ph_isConv_ifAuthor16_h1 = ROOT.TH1F("ph_isConv_ifAuthor16_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv_ifAuthor16"] = ph_isConv_ifAuthor16_h1
        ph_nConv_ifAuthor16_h1 = ROOT.TH1F("ph_nConv_ifAuthor16_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv_ifAuthor16"] = ph_nConv_ifAuthor16_h1
        pass
    elif n == 41 : ## histos for TDirectory Photons -> Author16Photons -> LooseVariables
        ph_Es2_ifAuthor16_h1 = ROOT.TH1F("ph_Es2_ifAuthor16_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2_ifAuthor16"] = ph_Es2_ifAuthor16_h1
        ph_E233_ifAuthor16_h1 = ROOT.TH1F("ph_E233_ifAuthor16_h1","ph_E233",100,0.,250000.)
        histos["ph_E233_ifAuthor16"] = ph_E233_ifAuthor16_h1
        ph_E237_ifAuthor16_h1 = ROOT.TH1F("ph_E237_ifAuthor16_h1","ph_E237",100,0.,250000.)
        histos["ph_E237_ifAuthor16"] = ph_E237_ifAuthor16_h1
        ph_E277_ifAuthor16_h1 = ROOT.TH1F("ph_E277_ifAuthor16_h1","ph_E277",100,0.,250000.)
        histos["ph_E277_ifAuthor16"] = ph_E277_ifAuthor16_h1
        ph_weta2_ifAuthor16_h1 = ROOT.TH1F("ph_weta2_ifAuthor16_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2_ifAuthor16"] = ph_weta2_ifAuthor16_h1
        ph_Ethad_ifAuthor16_h1 = ROOT.TH1F("ph_Ethad_ifAuthor16_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad_ifAuthor16"] = ph_Ethad_ifAuthor16_h1
        ph_Ethad1_ifAuthor16_h1 = ROOT.TH1F("ph_Ethad1_ifAuthor16_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1_ifAuthor16"] = ph_Ethad1_ifAuthor16_h1
        pass
    elif n == 42 : ## histos for TDirectory Photons -> Author16Photons -> TightVariables
        ph_wstot_ifAuthor16_h1 = ROOT.TH1F("ph_wstot_ifAuthor16_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot_ifAuthor16"] = ph_wstot_ifAuthor16_h1
        ph_deltaEmax2_ifAuthor16_h1 = ROOT.TH1F("ph_deltaEmax2_ifAuthor16_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2_ifAuthor16"] = ph_deltaEmax2_ifAuthor16_h1
        ph_emaxs1_ifAuthor16_h1 = ROOT.TH1F("ph_emaxs1_ifAuthor16_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1_ifAuthor16"] = ph_emaxs1_ifAuthor16_h1
        ph_deltaEs_ifAuthor16_h1 = ROOT.TH1F("ph_deltaEs_ifAuthor16_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs_ifAuthor16"] = ph_deltaEs_ifAuthor16_h1
        ph_cl_E_ifAuthor16_h1 = ROOT.TH1F("ph_cl_E_ifAuthor16_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E_ifAuthor16"] = ph_cl_E_ifAuthor16_h1
        ph_f1_ifAuthor16_h1 = ROOT.TH1F("ph_f1_ifAuthor16_h1","ph_f1",100,0.,1.)
        histos["ph_f1_ifAuthor16"] = ph_f1_ifAuthor16_h1
        pass
    
    elif n == 50 : ## histos for TDirectory Photons -> IsConvPhotons
        ph_E_ifIsConv_h1 = ROOT.TH1F("ph_E_ifIsConv_h1","ph_E",100,0.,100000.)
        histos["ph_E_ifIsConv"] = ph_E_ifIsConv_h1
        ph_author_ifIsConv_h1 = ROOT.TH1F("ph_author_ifIsConv_h1","ph_author",17,0.,17.)
        histos["ph_author_ifIsConv"] = ph_author_ifIsConv_h1
        ph_eta_ifIsConv_h1 = ROOT.TH1F("ph_eta_ifIsConv_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta_ifIsConv"] = ph_eta_ifIsConv_h1
        ph_loose_ifIsConv_h1 = ROOT.TH1F("ph_loose_ifIsConv_h1","ph_loose",2,0.,2.)
        histos["ph_loose_ifIsConv"] = ph_loose_ifIsConv_h1
        ph_n_ifIsConv_h1 = ROOT.TH1F("ph_n_ifIsConv_h1","ph_n",20,0.,20.)
        histos["ph_n_ifIsConv"] = ph_n_ifIsConv_h1
        ph_pt_ifIsConv_h1 = ROOT.TH1F("ph_pt_ifIsConv_h1","ph_pt",100,0.,100000.)
        histos["ph_pt_ifIsConv"] = ph_pt_ifIsConv_h1
        ph_phi_ifIsConv_h1 = ROOT.TH1F("ph_phi_ifIsConv_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi_ifIsConv"] = ph_phi_ifIsConv_h1
        ph_tight_ifIsConv_h1 = ROOT.TH1F("ph_tight_ifIsConv_h1","ph_tight",2,0.,2.)
        histos["ph_tight_ifIsConv"] = ph_tight_ifIsConv_h1
        ph_tightIso_ifIsConv_h1 = ROOT.TH1F("ph_tightIso_ifIsConv_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso_ifIsConv"] = ph_tightIso_ifIsConv_h1
        ph_isConv_ifIsConv_h1 = ROOT.TH1F("ph_isConv_ifIsConv_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv_ifIsConv"] = ph_isConv_ifIsConv_h1
        ph_nConv_ifIsConv_h1 = ROOT.TH1F("ph_nConv_ifIsConv_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv_ifIsConv"] = ph_nConv_ifIsConv_h1
        pass
    elif n == 51 : ## histos for TDirectory Photons -> IsConvPhotons -> LooseVariables
        ph_Es2_ifIsConv_h1 = ROOT.TH1F("ph_Es2_ifIsConv_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2_ifIsConv"] = ph_Es2_ifIsConv_h1
        ph_E233_ifIsConv_h1 = ROOT.TH1F("ph_E233_ifIsConv_h1","ph_E233",100,0.,250000.)
        histos["ph_E233_ifIsConv"] = ph_E233_ifIsConv_h1
        ph_E237_ifIsConv_h1 = ROOT.TH1F("ph_E237_ifIsConv_h1","ph_E237",100,0.,250000.)
        histos["ph_E237_ifIsConv"] = ph_E237_ifIsConv_h1
        ph_E277_ifIsConv_h1 = ROOT.TH1F("ph_E277_ifIsConv_h1","ph_E277",100,0.,250000.)
        histos["ph_E277_ifIsConv"] = ph_E277_ifIsConv_h1
        ph_weta2_ifIsConv_h1 = ROOT.TH1F("ph_weta2_ifIsConv_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2_ifIsConv"] = ph_weta2_ifIsConv_h1
        ph_Ethad_ifIsConv_h1 = ROOT.TH1F("ph_Ethad_ifIsConv_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad_ifIsConv"] = ph_Ethad_ifIsConv_h1
        ph_Ethad1_ifIsConv_h1 = ROOT.TH1F("ph_Ethad1_ifIsConv_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1_ifIsConv"] = ph_Ethad1_ifIsConv_h1
        pass
    elif n == 52 : ## histos for TDirectory Photons -> IsConvPhotons -> TightVariables
        ph_wstot_ifIsConv_h1 = ROOT.TH1F("ph_wstot_ifIsConv_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot_ifIsConv"] = ph_wstot_ifIsConv_h1
        ph_deltaEmax2_ifIsConv_h1 = ROOT.TH1F("ph_deltaEmax2_ifIsConv_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2_ifIsConv"] = ph_deltaEmax2_ifIsConv_h1
        ph_emaxs1_ifIsConv_h1 = ROOT.TH1F("ph_emaxs1_ifIsConv_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1_ifIsConv"] = ph_emaxs1_ifIsConv_h1
        ph_deltaEs_ifIsConv_h1 = ROOT.TH1F("ph_deltaEs_ifIsConv_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs_ifIsConv"] = ph_deltaEs_ifIsConv_h1
        ph_cl_E_ifIsConv_h1 = ROOT.TH1F("ph_cl_E_ifIsConv_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E_ifIsConv"] = ph_cl_E_ifIsConv_h1
        ph_f1_ifIsConv_h1 = ROOT.TH1F("ph_f1_ifIsConv_h1","ph_f1",100,0.,1.)
        histos["ph_f1_ifIsConv"] = ph_f1_ifIsConv_h1
        pass
    
    elif n == 60 : ## histos for TDirectory Photons -> UnConvPhotons
        ph_E_ifUnConv_h1 = ROOT.TH1F("ph_E_ifUnConv_h1","ph_E",100,0.,100000.)
        histos["ph_E_ifUnConv"] = ph_E_ifUnConv_h1
        ph_author_ifUnConv_h1 = ROOT.TH1F("ph_author_ifUnConv_h1","ph_author",17,0.,17.)
        histos["ph_author_ifUnConv"] = ph_author_ifUnConv_h1
        ph_eta_ifUnConv_h1 = ROOT.TH1F("ph_eta_ifUnConv_h1","ph_eta",10,-3.5,3.5)
        histos["ph_eta_ifUnConv"] = ph_eta_ifUnConv_h1
        ph_loose_ifUnConv_h1 = ROOT.TH1F("ph_loose_ifUnConv_h1","ph_loose",2,0.,2.)
        histos["ph_loose_ifUnConv"] = ph_loose_ifUnConv_h1
        ph_n_ifUnConv_h1 = ROOT.TH1F("ph_n_ifUnConv_h1","ph_n",20,0.,20.)
        histos["ph_n_ifUnConv"] = ph_n_ifUnConv_h1
        ph_pt_ifUnConv_h1 = ROOT.TH1F("ph_pt_ifUnConv_h1","ph_pt",100,0.,100000.)
        histos["ph_pt_ifUnConv"] = ph_pt_ifUnConv_h1
        ph_phi_ifUnConv_h1 = ROOT.TH1F("ph_phi_ifUnConv_h1","ph_phi",100,-3.5,3.5)
        histos["ph_phi_ifUnConv"] = ph_phi_ifUnConv_h1
        ph_tight_ifUnConv_h1 = ROOT.TH1F("ph_tight_ifUnConv_h1","ph_tight",2,0.,2.)
        histos["ph_tight_ifUnConv"] = ph_tight_ifUnConv_h1
        ph_tightIso_ifUnConv_h1 = ROOT.TH1F("ph_tightIso_ifUnConv_h1","ph_tightIso",2,0.,2.)
        histos["ph_tightIso_ifUnConv"] = ph_tightIso_ifUnConv_h1
        ph_isConv_ifUnConv_h1 = ROOT.TH1F("ph_isConv_ifUnConv_h1","ph_isConv",2,0.,2.)
        histos["ph_isConv_ifUnConv"] = ph_isConv_ifUnConv_h1
        ph_nConv_ifUnConv_h1 = ROOT.TH1F("ph_nConv_ifUnConv_h1","ph_nConv",10,0.,10.)
        histos["ph_nConv_ifUnConv"] = ph_nConv_ifUnConv_h1
        pass
    elif n == 61 : ## histos for TDirectory Photons -> UnConvPhotons -> LooseVariables
        ph_Es2_ifUnConv_h1 = ROOT.TH1F("ph_Es2_ifUnConv_h1","ph_Es2",100,0.,250000.)
        histos["ph_Es2_ifUnConv"] = ph_Es2_ifUnConv_h1
        ph_E233_ifUnConv_h1 = ROOT.TH1F("ph_E233_ifUnConv_h1","ph_E233",100,0.,250000.)
        histos["ph_E233_ifUnConv"] = ph_E233_ifUnConv_h1
        ph_E237_ifUnConv_h1 = ROOT.TH1F("ph_E237_ifUnConv_h1","ph_E237",100,0.,250000.)
        histos["ph_E237_ifUnConv"] = ph_E237_ifUnConv_h1
        ph_E277_ifUnConv_h1 = ROOT.TH1F("ph_E277_ifUnConv_h1","ph_E277",100,0.,250000.)
        histos["ph_E277_ifUnConv"] = ph_E277_ifUnConv_h1
        ph_weta2_ifUnConv_h1 = ROOT.TH1F("ph_weta2_ifUnConv_h1","ph_weta2",100,0.,0.05)
        histos["ph_weta2_ifUnConv"] = ph_weta2_ifUnConv_h1
        ph_Ethad_ifUnConv_h1 = ROOT.TH1F("ph_Ethad_ifUnConv_h1","ph_Ethad",100,-5000.,30000.)
        histos["ph_Ethad_ifUnConv"] = ph_Ethad_ifUnConv_h1
        ph_Ethad1_ifUnConv_h1 = ROOT.TH1F("ph_Ethad1_ifUnConv_h1","ph_Ethad1",100,-5000.,20000.)
        histos["ph_Ethad1_ifUnConv"] = ph_Ethad1_ifUnConv_h1
        pass
    elif n == 62 : ## histos for TDirectory Photons -> UnConvPhotons -> TightVariables
        ph_wstot_ifUnConv_h1 = ROOT.TH1F("ph_wstot_ifUnConv_h1","ph_wstot",100,-10000.,1000.)
        histos["ph_wstot_ifUnConv"] = ph_wstot_ifUnConv_h1
        ph_deltaEmax2_ifUnConv_h1 = ROOT.TH1F("ph_deltaEmax2_ifUnConv_h1","ph_deltaEmax2",100,0.,5.)
        histos["ph_deltaEmax2_ifUnConv"] = ph_deltaEmax2_ifUnConv_h1
        ph_emaxs1_ifUnConv_h1 = ROOT.TH1F("ph_emaxs1_ifUnConv_h1","ph_emaxs1",100,0.,40000.)
        histos["ph_emaxs1_ifUnConv"] = ph_emaxs1_ifUnConv_h1
        ph_deltaEs_ifUnConv_h1 = ROOT.TH1F("ph_deltaEs_ifUnConv_h1","ph_deltaEs",100,0.,5000.)
        histos["ph_deltaEs_ifUnConv"] = ph_deltaEs_ifUnConv_h1
        ph_cl_E_ifUnConv_h1 = ROOT.TH1F("ph_cl_E_ifUnConv_h1","ph_cl_E",100,0.,350000.)
        histos["ph_cl_E_ifUnConv"] = ph_cl_E_ifUnConv_h1
        ph_f1_ifUnConv_h1 = ROOT.TH1F("ph_f1_ifUnConv_h1","ph_f1",100,0.,1.)
        histos["ph_f1_ifUnConv"] = ph_f1_ifUnConv_h1
        pass
    
    else : ## ERROR
        print "::: ERROR wrong flag :::"
        pass
    
    return histos


def defHistExpEle(n,histos) :
    
    if n == 0 : ## TDirectory Expert -> Electrons 
        pass
    elif n == 1 : ## TDirectory Expert -> Electrons -> Identification
        el_origin_h1 = ROOT.TH1F("el_origin_h1","el_origin",50,0.,50.)
        histos["el_origin"] = el_origin_h1
        el_type_h1 = ROOT.TH1F("el_type_h1","el_type",20,0.,20.)
        histos["el_type"] = el_type_h1
        el_isEM_h1 = ROOT.TH1F("el_isEM_h1","el_isEM",2,0.,40000000000.)
        histos["el_isEM"] = el_isEM_h1
        el_author_h1 = ROOT.TH1F("el_author_h1","el_author",10,0.,10.)
        histos["el_author"] = el_author_h1
        el_loose_h1 = ROOT.TH1F("el_loose_h1","el_loose",2,0.,2.)
        histos["el_loose"] = el_loose_h1
        el_medium_h1 = ROOT.TH1F("el_medium_h1","el_medium",2,0.,2.)
        histos["el_medium"] = el_medium_h1
        el_tight_h1 = ROOT.TH1F("el_tight_h1","el_tight",2,0.,2.)
        histos["el_tight"] = el_tight_h1
        el_mediumIso_h1 = ROOT.TH1F("el_mediumIso_h1","el_mediumIso",2,0.,2.)
        histos["el_mediumIso"] = el_mediumIso_h1
        el_tightIso_h1 = ROOT.TH1F("el_tightIso_h1","el_tightIso",2,0.,2.)
        histos["el_tightIso"] = el_tightIso_h1
        #  el_tightNoIso_h1 = ROOT.TH1F("el_tightNoIso_h1","el_tightNoIso",2,0.,2.)
        #  histos["el_tightNoIso"] = el_tightNoIso_h1
        #  el_tightTRTNoIso_h1 = ROOT.TH1F("el_tightTRTNoIso_h1","el_tightTRTNoIso",2,0.,2.)
        #  histos["el_tightTRTNoIso"] = el_tightTRTNoIso_h1
        #  el_mediumNoIso_h1 = ROOT.TH1F("el_mediumNoIso_h1","el_mediumNoIso",2,0.,2.)
        #  histos["el_mediumNoIso"] = el_mediumNoIso_h1
        pass
    
    elif n == 2 : ## TDirectory Expert -> Electrons -> Kinematics
        el_E_h1 = ROOT.TH1F("el_E_h1","el_E",100,0.,1800000.)
        histos["el_E"] = el_E_h1
        #  el_EF_E_h1 = ROOT.TH1F("el_EF_E_h1","el_EF_E",100,0.,1800000.)
        #  histos["el_EF_E"] = el_EF_E_h1
        el_Et_h1 = ROOT.TH1F("el_Et_h1","el_Et",100,0.,200000.)
        histos["el_Et"] = el_Et_h1
        #  el_EF_Et_h1 = ROOT.TH1F("el_EF_Et_h1","el_EF_Et",100,0.,200000.)
        #  histos["el_EF_Et"] = el_EF_Et_h1
        el_pt_h1 = ROOT.TH1F("el_pt_h1","el_pt",100,0.,200000.)
        histos["el_pt"] = el_pt_h1
        #  el_EF_pt_h1 = ROOT.TH1F("el_EF_pt_h1","el_EF_pt",100,0.,200000.)
        #  histos["el_EF_pt"] = el_EF_pt_h1
        el_m_h1 = ROOT.TH1F("el_m_h1","el_m",10,-1.,1.)
        histos["el_m"] = el_m_h1
        el_eta_h1 = ROOT.TH1F("el_eta_h1","el_eta",100,-5.,5.)
        histos["el_eta"] = el_eta_h1
        #  el_EF_eta_h1 = ROOT.TH1F("el_EF_eta_h1","el_EF_eta",100,-5.,5.)
        #  histos["el_EF_eta"] = el_EF_eta_h1
        el_phi_h1 = ROOT.TH1F("el_phi_h1","el_phi",100,-3.5,3.5)
        histos["el_phi"] = el_phi_h1
        #  el_EF_phi_h1 = ROOT.TH1F("el_EF_phi_h1","el_EF_phi",100,-3.5,3.5)
        #  histos["el_EF_phi"] = el_EF_phi_h1
        el_px_h1 = ROOT.TH1F("el_px_h1","el_px",100,-150000.,150000.)
        histos["el_px"] = el_px_h1
        el_py_h1 = ROOT.TH1F("el_py_h1","el_py",100,-150000.,150000.)
        histos["el_py"] = el_py_h1
        el_pz_h1 = ROOT.TH1F("el_pz_h1","el_pz",100,-2000000.,2000000.)
        histos["el_pz"] = el_pz_h1
        el_charge_h1 = ROOT.TH1F("el_charge_h1","el_charge",10,-1.1,1.1)
        histos["el_charge"] = el_charge_h1
        #  el_EF_charge_h1 = ROOT.TH1F("el_EF_charge_h1","el_EF_charge",10,-1.1,1.1)
        #  histos["el_EF_charge"] = el_EF_charge_h1
        el_author_h1 = ROOT.TH1F("el_author_h1","el_author",10,0.,10.)
        histos["el_author"] = el_author_h1
        pass
    
    elif n == 3 : ## TDirectory Expert -> Electrons -> Conversions
        el_convFlag_h1 = ROOT.TH1F("el_convFlag_h1","el_convFlag",3,0.,3.)
        histos["el_convFlag"] = el_convFlag_h1
        el_isConv_h1 = ROOT.TH1F("el_isConv_h1","el_isConv",2,0.,2.)
        histos["el_isConv"] = el_isConv_h1
        pass
    
    elif n == 4 : ## TDirectory Expert -> Electrons -> EM
        el_deltaEmax2_h1 = ROOT.TH1F("el_deltaEmax2_h1","el_deltaEmax2",100,-2.,8.)
        histos["el_deltaEmax2"] = el_deltaEmax2_h1
        el_Ethad_h1 = ROOT.TH1F("el_Ethad_h1","el_Ethad",100,-2000.,50000.)
        histos["el_Ethad"] = el_Ethad_h1
        #  el_EF_Ethad_h1 = ROOT.TH1F("el_EF_Ethad_h1","el_EF_Ethad",100,-2000.,50000.)
        #  histos["el_EF_Ethad"] = el_EF_Ethad_h1
        el_Ethad1_h1 = ROOT.TH1F("el_Ethad1_h1","el_Ethad1",100,-1000.,20000.)
        histos["el_Ethad1"] = el_Ethad1_h1
        #  el_EF_Ethad1_h1 = ROOT.TH1F("el_EF_Ethad1_h1","el_EF_Ethad1",100,-1000.,20000.)
        #  histos["el_EF_Ethad1"] = el_EF_Ethad1_h1
        el_f1_h1 = ROOT.TH1F("el_f1_h1","el_f1",100,-0.3,1.3)
        histos["el_f1"] = el_f1_h1
        #  el_EF_f1_h1 = ROOT.TH1F("el_EF_f1_h1","el_EF_f1",100,-0.3,1.3)
        #  histos["el_EF_f1"] = el_EF_f1_h1
        el_f1core_h1 = ROOT.TH1F("el_f1core_h1","el_f1core",100,-0.3,1.3)
        histos["el_f1core"] = el_f1core_h1
        el_Emins1_h1 = ROOT.TH1F("el_Emins1_h1","el_Emins1",100,-1000.,5000.)
        histos["el_Emins1"] = el_Emins1_h1
        #  el_EF_Emins1_h1 = ROOT.TH1F("el_EF_Emins1_h1","el_EF_Emins1",100,-1000.,5000.)
        #  histos["el_EF_Emins1"] = el_EF_Emins1_h1
        el_emaxs1_h1 = ROOT.TH1F("el_emaxs1_h1","el_emaxs1",100,0.,70000.)
        histos["el_emaxs1"] = el_emaxs1_h1
        el_Emax2_h1 = ROOT.TH1F("el_Emax2_h1","el_Emax2",100,-1000.,8000.)
        histos["el_Emax2"] = el_Emax2_h1
        #  el_EF_Emax2_h1 = ROOT.TH1F("el_EF_Emax2_h1","el_EF_Emax2",100,-1000.,8000.)
        #  histos["el_EF_Emax2"] = el_EF_Emax2_h1
        el_fside_h1 = ROOT.TH1F("el_fside_h1","el_fside",100,-1.,2.)
        histos["el_fside"] = el_fside_h1
        #  el_EF_fside_h1 = ROOT.TH1F("el_EF_fside_h1","el_EF_fside",100,-1.,2.)
        #  histos["el_EF_fside"] = el_EF_fside_h1
        el_ws3_h1 = ROOT.TH1F("el_ws3_h1","el_ws3",100,-100.,200.)
        histos["el_ws3"] = el_ws3_h1
        # el_EF_ws3_h1 = ROOT.TH1F("el_EF_ws3_h1","el_EF_ws3",100,-100.,200.)
        # histos["el_EF_ws3"] = el_EF_ws3_h1
        el_wstot_h1 = ROOT.TH1F("el_wstot_h1","el_wstot",100,0.,50.)
        histos["el_wstot"] = el_wstot_h1
        # el_EF_wstot_h1 = ROOT.TH1F("el_EF_wstot_h1","el_EF_wstot",100,0.,50.)
        # histos["el_EF_wstot"] = el_EF_wstot_h1
        el_deltaEs_h1 = ROOT.TH1F("el_deltaEs_h1","el_deltaEs",100,-1000.,6000.)
        histos["el_deltaEs"] = el_deltaEs_h1
        el_E233_h1 = ROOT.TH1F("el_E233_h1","el_E233",100,0.,500000.)
        histos["el_E233"] = el_E233_h1
        # el_EF_E233_h1 = ROOT.TH1F("el_EF_E233_h1","el_EF_E233",100,0.,500000.)
        # histos["el_EF_E233"] = el_EF_E233_h1
        el_E237_h1 = ROOT.TH1F("el_E237_h1","el_E237",100,0.,500000.)
        histos["el_E237"] = el_E237_h1
        # el_EF_E237_h1 = ROOT.TH1F("el_EF_E237_h1","el_EF_E237",100,0.,500000.)
        # histos["el_EF_E237"] = el_EF_E237_h1
        el_E277_h1 = ROOT.TH1F("el_E277_h1","el_E277",100,0.,500000.)
        histos["el_E277"] = el_E277_h1
        # el_EF_E277_h1 = ROOT.TH1F("el_EF_E277_h1","el_EF_E277",100,0.,500000.)
        # histos["el_EF_E277"] = el_EF_E277_h1
        el_weta2_h1 = ROOT.TH1F("el_weta2_h1","el_weta2",100,0.,1.)
        histos["el_weta2"] = el_weta2_h1
        # el_EF_weta2_h1 = ROOT.TH1F("el_EF_weta2_h1","el_EF_weta2",100,0.,1.)
        # histos["el_EF_weta2"] = el_EF_weta2_h1
        el_f3_h1 = ROOT.TH1F("el_f3_h1","el_f3",2,-999.5,-998.5)
        histos["el_f3"] = el_f3_h1
        el_f3core_h1 = ROOT.TH1F("el_f3core_h1","el_f3core",100,0.,1.)
        histos["el_f3core"] = el_f3core_h1
        el_rphiallcalo_h1 = ROOT.TH1F("el_rphiallcalo_h1","el_rphiallcalo",100,-1.,1.)
        histos["el_rphiallcalo"] = el_rphiallcalo_h1
        el_reta_h1 = ROOT.TH1F("el_reta_h1","el_reta",100,-1.,2.)
        histos["el_reta"] = el_reta_h1
        el_rphi_h1 = ROOT.TH1F("el_rphi_h1","el_rphi",100,0.,2.)
        histos["el_rphi"] = el_rphi_h1
        el_Es0_h1 = ROOT.TH1F("el_Es0_h1","el_Es0",100,0.,60000.)
        histos["el_Es0"] = el_Es0_h1
        el_Es1_h1 = ROOT.TH1F("el_Es1_h1","el_Es1",100,0.,120000.)
        histos["el_Es1"] = el_Es1_h1
        el_Es2_h1 = ROOT.TH1F("el_Es2_h1","el_Es2",100,0.,700000.)
        histos["el_Es2"] = el_Es2_h1
        el_Es3_h1 = ROOT.TH1F("el_Es3_h1","el_Es3",100,0.,50000.)
        histos["el_Es3"] = el_Es3_h1
        el_etas0_h1 = ROOT.TH1F("el_etas0_h1","el_etas0",100,-5.,5.)
        histos["el_etas0"] = el_etas0_h1
        el_etas1_h1 = ROOT.TH1F("el_etas1_h1","el_etas1",100,-5.,5.)
        histos["el_etas1"] = el_etas1_h1
        el_etas2_h1 = ROOT.TH1F("el_etas2_h1","el_etas2",100,-5.,5.)
        histos["el_etas2"] = el_etas2_h1
        el_etas3_h1 = ROOT.TH1F("el_etas3_h1","el_etas3",100,-5.,5.)
        histos["el_etas3"] = el_etas3_h1
        el_phis0_h1 = ROOT.TH1F("el_phis0_h1","el_phis0",100,-3.5,3.5)
        histos["el_phis0"] = el_phis0_h1
        el_phis1_h1 = ROOT.TH1F("el_phis1_h1","el_phis1",100,-3.5,3.5)
        histos["el_phis1"] = el_phis1_h1
        el_phis2_h1 = ROOT.TH1F("el_phis2_h1","el_phis2",100,-3.5,3.5)
        histos["el_phis2"] = el_phis2_h1
        el_phis3_h1 = ROOT.TH1F("el_phis3_h1","el_phis3",100,-3.5,3.5)
        histos["el_phis3"] = el_phis3_h1
        pass
    elif n == 5 : ## TDirectory Expert -> Electrons -> Isolation
        el_Etcone45_h1 = ROOT.TH1F("el_Etcone45_h1","el_Etcone45",3,-999.5,-998.5)
        histos["el_Etcone45"] = el_Etcone45_h1
        # el_EF_Etcone45_h1 = ROOT.TH1F("el_EF_Etcone45_h1","el_EF_Etcone45",3,-999.5,-998.5)
        # histos["el_EF_Etcone45"] = el_EF_Etcone45_h1
        el_Etcone20_h1 = ROOT.TH1F("el_Etcone20_h1","el_Etcone20",100,0.,80000.)
        histos["el_Etcone20"] = el_Etcone20_h1
        # el_EF_Etcone20_h1 = ROOT.TH1F("el_EF_Etcone20_h1","el_EF_Etcone20",100,0.,80000.)
        # histos["el_EF_Etcone20"] = el_EF_Etcone20_h1
        el_Etcone30_h1 = ROOT.TH1F("el_Etcone30_h1","el_Etcone30",100,0.,160000.)
        histos["el_Etcone30"] = el_Etcone30_h1
        # el_EF_Etcone30_h1 = ROOT.TH1F("el_EF_Etcone30_h1","el_EF_Etcone30",100,0.,160000.)
        # histos["el_EF_Etcone30"] = el_EF_Etcone30_h1
        el_Etcone40_h1 = ROOT.TH1F("el_Etcone40_h1","el_Etcone40",100,0.,160000.)
        histos["el_Etcone40"] = el_Etcone40_h1
        # el_EF_Etcone40_h1 = ROOT.TH1F("el_EF_Etcone40_h1","el_EF_Etcone40",100,0.,160000.)
        # histos["el_EF_Etcone40"] = el_EF_Etcone40_h1
        el_EtringnoisedR03sig2_h1 = ROOT.TH1F("el_EtringnoisedR03sig2_h1","el_EtringnoisedR03sig2",100,0.,160000.)
        histos["el_EtringnoisedR03sig2"] = el_EtringnoisedR03sig2_h1
        el_EtringnoisedR03sig3_h1 = ROOT.TH1F("el_EtringnoisedR03sig3_h1","el_EtringnoisedR03sig3",100,0.,160000.)
        histos["el_EtringnoisedR03sig3"] = el_EtringnoisedR03sig3_h1
        el_EtringnoisedR03sig4_h1 = ROOT.TH1F("el_EtringnoisedR03sig4_h1","el_EtringnoisedR03sig4",100,0.,160000.)
        histos["el_EtringnoisedR03sig4"] = el_EtringnoisedR03sig4_h1
        pass
    elif n == 6 : ## TDirectory Expert -> Electrons -> Discriminants
        el_isolationlikelihoodhqelectrons_h1 = ROOT.TH1F("el_isolationlikelihoodhqelectrons_h1","el_isolationlikelihoodhqelectrons",100,-200.,10.)
        histos["el_isolationlikelihoodhqelectrons"] = el_isolationlikelihoodhqelectrons_h1
        el_Hmatrix_h1 = ROOT.TH1F("el_Hmatrix_h1","el_Hmatrix",100,0.,2000.)
        histos["el_Hmatrix"] = el_Hmatrix_h1
        el_Hmatrix5_h1 = ROOT.TH1F("el_Hmatrix5_h1","el_Hmatrix5",100,0.,200000.)
        histos["el_Hmatrix5"] = el_Hmatrix5_h1
        el_isolationlikelihoodjets_h1 = ROOT.TH1F("el_isolationlikelihoodjets_h1","el_isolationlikelihoodjets",100,-200.,10.)
        histos["el_isolationlikelihoodjets"] = el_isolationlikelihoodjets_h1
        el_neuralnet_h1 = ROOT.TH1F("el_neuralnet_h1","el_neuralnet",100,0.,1.)
        histos["el_neuralnet"] = el_neuralnet_h1
        el_adaboost_h1 = ROOT.TH1F("el_adaboost_h1","el_adaboost",100,-400.,400.)
        histos["el_adaboost"] = el_adaboost_h1
        el_electronweight_h1 = ROOT.TH1F("el_electronweight_h1","el_electronweight",100,-60.,0.)
        histos["el_electronweight"] = el_electronweight_h1
        el_electronbgweight_h1 = ROOT.TH1F("el_electronbgweight_h1","el_electronbgweight",100,-40.,0.)
        histos["el_electronbgweight"] = el_electronbgweight_h1
        el_softeweight_h1 = ROOT.TH1F("el_softeweight_h1","el_softeweight",100,-20.,2.)
        histos["el_sofetweight"] = el_softeweight_h1
        el_softebgweight_h1 = ROOT.TH1F("el_softebgweight_h1","el_softebgweight",100,-20.,2.)
        histos["el_softebgweight"] = el_softebgweight_h1
        el_softeneuralnet_h1 = ROOT.TH1F("el_softeneuralnet_h1","el_softeneuralnet",3,-1.,1.)
        histos["el_softeneuralnet"] = el_softeneuralnet_h1
        pass
    elif n == 7 : ## TDirectory Expert -> Electrons -> Pointing
        el_zvertex_h1 = ROOT.TH1F("el_zvertex_h1","el_zvertex",100,-5000.,5000.)
        histos["el_zvertex"] = el_zvertex_h1
        # el_EF_zvertex_h1 = ROOT.TH1F("el_EF_zvertex_h1","el_EF_zvertex",100,-5000.,5000.)
        # histos["el_EF_zvertex"] = el_EF_zvertex_h1
        el_errz_h1 = ROOT.TH1F("el_errz_h1","el_errz",100,0.,100.)
        histos["el_errz"] = el_errz_h1
        # el_EF_errz_h1 = ROOT.TH1F("el_EF_errz_h1","el_EF_errz",100,0.,100.)
        # histos["el_EF_errz"] = el_EF_errz_h1
        el_etap_h1 = ROOT.TH1F("el_etap_h1","el_etap",100,-5.,5.)
        histos["el_etap"] = el_etap_h1
        el_depth_h1 = ROOT.TH1F("el_depth_h1","el_depth",100,1400.,1750.)
        histos["el_depth"] = el_depth_h1
        el_pos7_h1 = ROOT.TH1F("el_pos7_h1","el_pos7",100,-10.,10.)
        histos["el_pos7"] = el_pos7_h1
        pass 
    elif n == 8 : ## TDirectory Expert -> Electrons -> ClusterKinematics
        el_cl_E_h1 = ROOT.TH1F("el_cl_E_h1","el_cl_E",100,0.,1600000.)
        histos["el_cl_E"] = el_cl_E_h1
        el_cl_pt_h1 = ROOT.TH1F("el_cl_pt_h1","el_cl_pt",100,0.,120000.)
        histos["el_cl_pt"] = el_cl_pt_h1
        el_cl_eta_h1 = ROOT.TH1F("el_cl_eta_h1","el_cl_eta",100,-5.,5.)
        histos["el_cl_eta"] = el_cl_eta_h1
        el_cl_phi_h1 = ROOT.TH1F("el_cl_phi_h1","el_cl_phi",100,-3.5,3.5)
        histos["el_cl_phi"] = el_cl_phi_h1
        pass
    elif n == 9 : ## TDirectory Expert -> Electrons -> L1
        pass
    elif n == 10 : ## TDirectory Expert -> Electrons -> L2
        pass
    elif n == 11 : ## TDirectory Expert -> Electrons -> EF
        pass
    elif n == 12 : ## TDirectory Expert -> Electrons -> Jet
        el_jet_E_h1 = ROOT.TH1F("el_jet_E_h1","el_jet_E",100,0.,1600000.)
        histos["el_jet_E"] = el_jet_E_h1
        el_jet_pt_h1 = ROOT.TH1F("el_jet_pt_h1","el_jet_pt",100,0.,120000.)
        histos["el_jet_pt"] = el_jet_pt_h1
        el_jet_m_h1 = ROOT.TH1F("el_jet_m_h1","el_jet_m",100,0.,15000.)
        histos["el_jet_m"] = el_jet_m_h1
        el_jet_eta_h1 = ROOT.TH1F("el_jet_eta_h1","el_jet_eta",100,-5.,5.)
        histos["el_jet_eta"] = el_jet_eta_h1
        el_jet_phi_h1 = ROOT.TH1F("el_jet_phi_h1","el_jet_phi",100,-3.5,3.5)
        histos["el_jet_phi"] = el_jet_phi_h1
        el_jet_matched_h1 = ROOT.TH1F("el_jet_matched_h1","el_jet_matched",2,0.,2.)
        histos["el_jet_matched"] = el_jet_matched_h1
        el_jet_dr_h1 = ROOT.TH1F("el_jet_dr_h1","el_jet_dr",100,0.,1600000.)
        histos["el_jet_dr"] = el_jet_dr_h1
        el_jet_truth_dr_h1 = ROOT.TH1F("el_jet_truth_dr_h1","el_jet_truth_dr",100,0.,0.3)
        histos["el_jet_truth_dr"] = el_jet_truth_dr_h1
        el_jet_truth_E_h1 = ROOT.TH1F("el_jet_truth_E_h1","el_jet_truth_E",100,0.,1200000.)
        histos["el_jet_truth_E"] = el_jet_truth_E_h1
        el_jet_truth_pt_h1 = ROOT.TH1F("el_jet_truth_pt_h1","el_jet_truth_pt",100,0.,100000.)
        histos["el_jet_truth_pt"] = el_jet_truth_pt_h1
        el_jet_truth_m_h1 = ROOT.TH1F("el_jet_truth_m_h1","el_jet_truth_m",100,0.,15000.)
        histos["el_jet_truth_m"] = el_jet_truth_m_h1
        el_jet_truth_eta_h1 = ROOT.TH1F("el_jet_truth_eta_h1","el_jet_truth_eta",100,-5.,5.)
        histos["el_jet_truth_eta"] = el_jet_truth_eta_h1
        el_jet_truth_phi_h1 = ROOT.TH1F("el_jet_truth_phi_h1","el_jet_truth_phi",100,-3.5,3.5)
        histos["el_jet_truth_phi"] = el_jet_truth_phi_h1
        el_jet_truth_matched_h1 = ROOT.TH1F("el_jet_truth_matched_h1","el_jet_truth_matched",2,0.,2.)
        histos["el_jet_truth_matched"] = el_jet_truth_matched_h1
        pass
    elif n == 13 : ## TDirectory Expert -> Electrons -> Track variables
        el_convtrk1nBLHits_h1 = ROOT.TH1F("el_convtrk1nBLHits_h1","el_convtrk1nBLHits",3,0.,3.)
        histos["el_convtrk1nBLHits"] = el_convtrk1nBLHits_h1
        el_convtrk1nPixHits_h1 = ROOT.TH1F("el_convtrk1nPixHits_h1","el_convtrk1nPixHits",10,0.,10.)
        histos["el_convtrk1nPixHits"] = el_convtrk1nPixHits_h1
        el_convtrk1nSCTHits_h1 = ROOT.TH1F("el_convtrk1nSCTHits_h1","el_convtrk1nSCTHits",15,0.,15.)
        histos["el_convtrk1nSCTHits"] = el_convtrk1nSCTHits_h1
        el_convtrk1nTRTHits_h1 = ROOT.TH1F("el_convtrk1nTRTHits_h1","el_convtrk1nTRTHits",50,0.,50.)
        histos["el_convtrk1nTRTHits"] = el_convtrk1nTRTHits_h1
        el_convtrk2nBLHits_h1 = ROOT.TH1F("el_convtrk2nBLHits_h1","el_convtrk2nBLHits",3,0.,3.)
        histos["el_convtrk2nBLHits"] = el_convtrk2nBLHits_h1
        el_convtrk2nPixHits_h1 = ROOT.TH1F("el_convtrk2nPixHits_h1","el_convtrk2nPixHits",10,0.,10.)
        histos["el_convtrk2nPixHits"] = el_convtrk2nPixHits_h1
        el_convtrk2nSCTHits_h1 = ROOT.TH1F("el_convtrk2nSCTHits_h1","el_convtrk2nSCTHits",15,0.,15.)
        histos["el_convtrk2nSCTHits"] = el_convtrk2nSCTHits_h1
        el_convtrk2nTRTHits_h1 = ROOT.TH1F("el_convtrk2nTRTHits_h1","el_convtrk2nTRTHits",50,0.,50.)
        histos["el_convtrk2nTRTHits"] = el_convtrk2nTRTHits_h1
        el_convvtxx_h1 = ROOT.TH1F("el_convvtxx_h1","el_convvtxx",100,-800.,800.)
        histos["el_convvtxx"] = el_convvtxx_h1
        el_convvtxy_h1 = ROOT.TH1F("el_convvtxy_h1","el_convvtxy",100,-800.,800.)
        histos["el_convvtxy"] = el_convvtxy_h1
        el_convvtxz_h1 = ROOT.TH1F("el_convvtxz_h1","el_convvtxz",100,-2000.,2000.)
        histos["el_convvtxz"] = el_convvtxz_h1
        el_zconv_h1 = ROOT.TH1F("el_zconv_h1","el_zconv",100,-2000.,2000.)
        histos["el_zconv"] = el_zconv_h1
        el_Rconv_h1 = ROOT.TH1F("el_Rconv_h1","el_Rconv",100,0.,900.)
        histos["el_Rconv"] = el_Rconv_h1
        el_ptconv_h1 = ROOT.TH1F("el_ptconv_h1","el_ptconv",100,0.,100000.)
        histos["el_ptconv"] = el_ptconv_h1
        el_pt1conv_h1 = ROOT.TH1F("el_pt1conv_h1","el_pt1conv",100,0.,100000.)
        histos["el_pt1conv"] = el_pt1conv_h1
        el_pt2conv_h1 = ROOT.TH1F("el_pt2conv_h1","el_pt2conv",100,0.,10000.)
        histos["el_pt2conv"] = el_pt2conv_h1
        el_pzconv_h1 = ROOT.TH1F("el_pzconv_h1","el_pzconv",100,-20000.,20000.)
        histos["el_pzconv"] = el_pzconv_h1
        el_hasconv_h1 = ROOT.TH1F("el_hasconv_h1","el_hasconv",2,0.,2.)
        histos["el_hasconv"] = el_hasconv_h1
        el_convvtxchi2_h1 = ROOT.TH1F("el_convvtxchi2_h1","el_convvtxchi2",100,0.,200.)
        histos["el_convvtxchi2"] = el_convvtxchi2_h1
        el_convanglematch_h1 = ROOT.TH1F("el_convanglematch_h1","el_convanglematch",10,-1000.,2.)
        histos["el_convanglematch"] = el_convanglematch_h1
        el_convtrackmatch_h1 = ROOT.TH1F("el_convtrackmatch_h1","el_convtrackmatch",10,-1000.,2.)
        histos["el_convtrackmatch"] = el_convtrackmatch_h1

        el_hastrack_h1 = ROOT.TH1F("el_hastrack_h1","el_hastrack",2,0.,2.)
        histos["el_hastrack"] = el_hastrack_h1
        el_nBLHits_h1 = ROOT.TH1F("el_nBLHits_h1","el_nBLHits",4,0.,4.)
        histos["el_nBLHits"] = el_nBLHits_h1
        el_nPixHits_h1 = ROOT.TH1F("el_nPixHits_h1","el_nPixHits",9,0.,9.)
        histos["el_nPixHits"] = el_nPixHits_h1
        el_nSCTHits_h1 = ROOT.TH1F("el_nSCTHits_h1","el_nSCTHits",15,0.,15.)
        histos["el_nSCTHits"] = el_nSCTHits_h1
        el_nTRTHits_h1 = ROOT.TH1F("el_nTRTHits_h1","el_nTRTHits",50,0.,50.)
        histos["el_nTRTHits"] = el_nTRTHits_h1
        el_nTRTHighTHits_h1 = ROOT.TH1F("el_nTRTHighTHits_h1","el_nTRTHighTHits",25,0.,25.)
        histos["el_nTRTHighTHits"] = el_nTRTHighTHits_h1
        el_nBLSharedHits_h1 = ROOT.TH1F("el_nBLSharedHits_h1","el_nBLSharedHits",3,0.,3.)
        histos["el_nBLSharedHits"] = el_nBLSharedHits_h1
        el_nPixSharedHits_h1 = ROOT.TH1F("el_nPixSharedHits_h1","el_nPixSharedHits",4,0.,4.)
        histos["el_nPixSharedHits"] = el_nPixSharedHits_h1
        el_nPixHoles_h1 = ROOT.TH1F("el_nPixHoles_h1","el_nPixHoles",5,-2.,3.)
        histos["el_nPixHoles"] = el_nPixHoles_h1
        el_nSCTSharedHits_h1 = ROOT.TH1F("el_nSCTSharedHits_h1","el_nSCTSharedHits",6,0.,6.)
        histos["el_nSCTSharedHits"] = el_nSCTSharedHits_h1
        el_nSCTHoles_h1 = ROOT.TH1F("el_nSCTHoles_h1","el_nSCTHoles",5,-2.,3.)
        histos["el_nSCTHoles"] = el_nSCTHoles_h1
        el_nTRTOutliers_h1 = ROOT.TH1F("el_nTRTOutliers_h1","el_nTRTOutliers",50,0.,50.)
        histos["el_nTRTOutliers"] = el_nTRTOutliers_h1
        el_nTRTHighTOutliers_h1 = ROOT.TH1F("el_nTRTHighTOutliers_h1","el_nTRTHighTOutliers",25,0.,25.)
        histos["el_nTRTHighTOutliers"] = el_nTRTHighTOutliers_h1
        el_nSiHits_h1 = ROOT.TH1F("el_nSiHits_h1","el_nSiHits",2,0.,20.)
        histos["el_nSiHits"] = el_nSiHits_h1
        # el_EF_nSiHits_h1 = ROOT.TH1F("el_EF_nSiHits_h1","el_EF_nSiHits",20.,0.,20.)
        # histos["el_EF_nSiHits"] = el_EF_nSiHits_h1
        pass
    elif n == 14 : ## TDirectory Expert -> Electrons -> Rawcl
        el_rawcl_E_h1 = ROOT.TH1F("el_rawcl_E_h1","el_rawcl_E",100,0.,300000.)
        histos["el_rawcl_E"] = el_rawcl_E_h1
        el_rawcl_pt_h1 = ROOT.TH1F("el_rawcl_pt_h1","el_rawcl_pt",100,0.,80000.)
        histos["el_rawcl_pt"] = el_rawcl_pt_h1
        el_rawcl_eta_h1 = ROOT.TH1F("el_rawcl_eta_h1","el_rawcl_eta",100,-3.,3.)
        histos["el_rawcl_eta"] = el_rawcl_eta_h1
        el_rawcl_phi_h1 = ROOT.TH1F("el_rawcl_phi_h1","el_rawcl_phi",100,-3.5,3.5)
        histos["el_rawcl_phi"] = el_rawcl_phi_h1
        el_rawcl_Es0_h1 = ROOT.TH1F("el_rawcl_Es0_h1","el_rawcl_Es0",100,0.,25000.)
        histos["el_rawcl_Es0"] = el_rawcl_Es0_h1
        el_rawcl_etas0_h1 = ROOT.TH1F("el_rawcl_etas0_h1","el_rawcl_etas0",100,-3.,3.)
        histos["el_rawcl_etas0"] = el_rawcl_etas0_h1
        el_rawcl_phis0_h1 = ROOT.TH1F("el_rawcl_phis0_h1","el_rawcl_phis0",100,3.5,3.5)
        histos["el_rawcl_phis0"] = el_rawcl_phis0_h1
        el_rawcl_Es1_h1 = ROOT.TH1F("el_rawcl_Es1_h1","el_rawcl_Es1",100,0.,25000.)
        histos["el_rawcl_Es1"] = el_rawcl_Es1_h1
        el_rawcl_etas1_h1 = ROOT.TH1F("el_rawcl_etas1_h1","el_rawcl_etas1",100,-3.,3.)
        histos["el_rawcl_etas1"] = el_rawcl_etas1_h1
        el_rawcl_phis1_h1 = ROOT.TH1F("el_rawcl_phis1_h1","el_rawcl_phis1",100,3.5,3.5)
        histos["el_rawcl_phis1"] = el_rawcl_phis1_h1
        el_rawcl_Es2_h1 = ROOT.TH1F("el_rawcl_Es2_h1","el_rawcl_Es2",100,0.,25000.)
        histos["el_rawcl_Es2"] = el_rawcl_Es2_h1
        el_rawcl_etas2_h1 = ROOT.TH1F("el_rawcl_etas2_h1","el_rawcl_etas2",100,-3.,3.)
        histos["el_rawcl_etas2"] = el_rawcl_etas2_h1
        el_rawcl_phis2_h1 = ROOT.TH1F("el_rawcl_phis2_h1","el_rawcl_phis2",100,3.5,3.5)
        histos["el_rawcl_phis2"] = el_rawcl_phis2_h1
        el_rawcl_Es3_h1 = ROOT.TH1F("el_rawcl_Es3_h1","el_rawcl_Es3",100,0.,25000.)
        histos["el_rawcl_Es3"] = el_rawcl_Es3_h1
        el_rawcl_etas3_h1 = ROOT.TH1F("el_rawcl_etas3_h1","el_rawcl_etas3",100,-3.,3.)
        histos["el_rawcl_etas3"] = el_rawcl_etas3_h1
        el_rawcl_phis3_h1 = ROOT.TH1F("el_rawcl_phis3_h1","el_rawcl_phis3",100,3.5,3.5)
        histos["el_rawcl_phis3"] = el_rawcl_phis3_h1
        pass
    elif n == 15 : ## TDirectory Expert -> Electrons -> Vertex
        el_vertx_h1 = ROOT.TH1F("el_vertx_h1","el_vertx",100,-0.1,0.25)
        histos["el_vertx"] = el_vertx_h1
        el_verty_h1 = ROOT.TH1F("el_verty_h1","el_verty",100,0.,1.3)
        histos["el_verty"] = el_verty_h1
        el_vertz_h1 = ROOT.TH1F("el_vertz_h1","el_vertz",100,-200.,250.)
        histos["el_vertz"] = el_vertz_h1
        pass
    elif n == 16 : ## TDirectory Expert -> Electrons -> Brem
        pass
    elif n == 17 : ## TDirectory Expert -> Electrons -> MC
        el_truth_E_h1 = ROOT.TH1F("el_truth_E_h1","el_truth_E",100,0.,1000000.)
        histos["el_truth_E"] = el_truth_E_h1
        el_truth_pt_h1 = ROOT.TH1F("el_truth_pt_h1","el_truth_pt",100,0.,100000.)
        histos["el_truth_pt"] = el_truth_pt_h1
        el_truth_eta_h1 = ROOT.TH1F("el_truth_eta_h1","el_truth_eta",100,-5.,5.)
        histos["el_truth_eta"] = el_truth_eta_h1
        el_truth_phi_h1 = ROOT.TH1F("el_truth_phi_h1","el_truth_phi",100,-3.5,3.5)
        histos["el_truth_phi"] = el_truth_phi_h1
        el_truth_type_h1 = ROOT.TH1F("el_truth_type_h1","el_truth_type",100,-3300.,3300.)
        histos["el_truth_type"] = el_truth_type_h1
        el_truth_mothertype_h1 = ROOT.TH1F("el_truth_mothertype_h1","el_truth_mothertype",100,-2000.,2000.)
        histos["el_truth_mothertype"] = el_truth_mothertype_h1
        el_truth_barcode_h1 = ROOT.TH1F("el_truth_barcode_h1","el_truth_barcode",100,0.,100.)
        histos["el_truth_barcode"] = el_truth_barcode_h1
        el_truth_motherbarcode_h1 = ROOT.TH1F("el_truth_motherbarcode_h1","el_truth_motherbarcode",100,0.,100.)
        histos["el_truth_motherbarcode"] = el_truth_motherbarcode_h1
        el_truth_hasHardBrem_h1 = ROOT.TH1F("el_truth_hasHardBrem_h1","el_truth_hasHardBrem",2,0.,2.)
        histos["el_truth_hasHardBrem"] = el_truth_hasHardBrem_h1
        el_truth_status_h1 = ROOT.TH1F("el_truth_status_h1","el_truth_status",2,0.,2.)
        histos["el_truth_status"] = el_truth_status_h1
        el_truth_matched_h1 = ROOT.TH1F("el_truth_matched_h1","el_truth_matched",2,0.,2.)
        histos["el_truth_matched"] = el_truth_matched_h1
        pass
    elif n == 18 : ## TDirectory Expert -> Electrons -> TrackMatch
        el_etacorrmag_h1 = ROOT.TH1F("el_etacorrmag_h1","el_etacorrmag",3,-999.5,-998.5)
        histos["el_etacorrmag"] = el_etacorrmag_h1
        # el_EF_etacorrmag_h1 = ROOT.TH1F("el_EF_etacorrmag_h1","el_EF_etacorrmag",3,-999.5,-998.5)
        # histos["el_EF_etacorrmag"] = el_EF_etacorrmag_h1
        el_deltaeta1_h1 = ROOT.TH1F("el_deltaeta1_h1","el_deltaeta1",100,-1.5,1.5)
        histos["el_deltaeta1"] = el_deltaeta1_h1
        # el_EF_deltaeta1_h1 = ROOT.TH1F("el_EF_deltaeta1_h1","el_EF_deltaeta1",100,-1.5,1.5)
        # histos["el_EF_deltaeta1"] = el_EF_deltaeta1_h1
        el_deltaeta2_h1 = ROOT.TH1F("el_deltaeta2_h1","el_deltaeta2",100,-1.5,1.5)
        histos["el_deltaeta2"] = el_deltaeta2_h1
        # el_EF_deltaeta2_h1 = ROOT.TH1F("el_EF_deltaeta2_h1","el_EF_deltaeta2",100,-1.5,1.5)
        # histos["el_EF_deltaeta2"] = el_EF_deltaeta2_h1
        el_deltaphi2_h1 = ROOT.TH1F("el_deltaphi2_h1","el_deltaphi2",100,-0.1,0.06)
        histos["el_deltaphi2"] = el_deltaphi2_h1
        # el_EF_deltaphi2_h1 = ROOT.TH1F("el_EF_deltaphi2_h1","el_EF_deltaphi2",100,-0.1,0.06)
        # histos["el_EF_deltaphi2"] = el_EF_deltaphi2_h1
        el_deltaphiRescaled_h1 = ROOT.TH1F("el_deltaphiRescaled_h1","el_deltaphiRescaled",100,-0.5,1.5)
        histos["el_deltaphiRescaled"] = el_deltaphiRescaled_h1
        pass
    elif n == 19 : ## TDirectory Expert -> Electrons -> EMTrackFit
        el_refittedTrack_n_h1 = ROOT.TH1F("el_refittedTrack_n_h1","el_refittedTrack_n",3,0.,3.)
        histos["el_refittedTrack_n"] = el_refittedTrack_n_h1
        el_refittedtrackcovd0_h1 = ROOT.TH1F("el_refittedtrackcovd0_h1","el_refittedtrackcovd0",100,0.,30.)
        histos["el_refittedtrackcovd0"] = el_refittedtrackcovd0_h1
        el_refittedtrackcovd0phi_h1 = ROOT.TH1F("el_refittedtrackcovd0phi_h1","el_refittedtrackcovd0phi",100,-0.18,0.)
        histos["el_refittedtrackcovd0phi"] = el_refittedtrackcovd0phi_h1
        el_refittedtrackcovd0qoverp_h1 = ROOT.TH1F("el_refittedtrackcovd0qoverp_h1","el_refittedtrackcovd0qoverp",100,-0.00018,0.)
        histos["el_refittedtrackcovd0qoverp"] = el_refittedtrackcovd0qoverp_h1
        el_refittedtrackcovd0theta_h1 = ROOT.TH1F("el_refittedtrackcovd0theta_h1","el_refittedtrackcovd0theta",100,-0.006,0.002)
        histos["el_refittedtrackcovd0theta"] = el_refittedtrackcovd0theta_h1
        el_refittedtrackcovd0z0_h1 = ROOT.TH1F("el_refittedtrackcovd0z0_h1","el_refittedtrackcovd0z0",100,-15.,20.)
        histos["el_refittedtrackcovd0z0"] = el_refittedtrackcovd0z0_h1
        el_refittedtrackcovphi_h1 = ROOT.TH1F("el_refittedtrackcovphi_h1","el_refittedtrackcovphi",100,0.,0.001)
        histos["el_refittedtrackcovphi"] = el_refittedtrackcovphi_h1
        el_refittedtrackcovphiqoverp_h1 = ROOT.TH1F("el_refittedtrackcovphiqoverp_h1","el_refittedtrackcovphiqoverp",100,0.,0.000001)
        histos["el_refittedtrackcovphiqoverp"] = el_refittedtrackcovphiqoverp_h1
        el_refittedtrackcovphitheta_h1 = ROOT.TH1F("el_refittedtrackcovphitheta_h1","el_refittedtrackcovphitheta",100,-0.000001,0.0000025)
        histos["el_refittedtrackcovphitheta"] = el_refittedtrackcovphitheta_h1
        el_refittedtrackcovqoverp_h1 = ROOT.TH1F("el_refittedtrackcovqoverp_h1","el_refittedtrackcovqoverp",100,0.,0.000000004)
        histos["el_refittedtrackcovqoverp"] = el_refittedtrackcovqoverp_h1
        el_refittedtrackcovtheta_h1 = ROOT.TH1F("el_refittedtrackcovtheta_h1","el_refittedtrackcovtheta",100,0.,0.00005)
        histos["el_refittedtrackcovtheta"] = el_refittedtrackcovtheta_h1
        el_refittedtrackcovthetaqoverp_h1 = ROOT.TH1F("el_refittedtrackcovthetaqoverp_h1","el_refittedtrackcovthetaqoverp",100,-0.00000002,0.00000004)
        histos["el_refittedtrackcovthetaqoverp"] = el_refittedtrackcovthetaqoverp_h1
        el_refittedtrackcovz0_h1 = ROOT.TH1F("el_refittedtrackcovz0_h1","el_refittedtrackcovz0",100,0.,300.)
        histos["el_refittedtrackcovz0"] = el_refittedtrackcovz0_h1
        el_refittedtrackcovz0phi_h1 = ROOT.TH1F("el_refittedtrackcovz0phi_h1","el_refittedtrackcovz0phi",100,-0.1,0.06)
        histos["el_refittedtrackcovz0phi"] = el_refittedtrackcovz0phi_h1
        el_refittedtrackcovz0qoverp_h1 = ROOT.TH1F("el_refittedtrackcovz0qoverp_h1","el_refittedtrackcovz0qoverp",100,-0.00008,0.0001)
        histos["el_refittedtrackcovz0qoverp"] = el_refittedtrackcovz0qoverp_h1
        el_refittedtrackcovz0theta_h1 = ROOT.TH1F("el_refittedtrackcovz0theta_h1","el_refittedtrackcovz0theta",100,0.,0.2)
        histos["el_refittedtrackcovz0theta"] = el_refittedtrackcovz0theta_h1
        pass
    elif n == 20 : ## TDirectory Expert -> Electrons -> Forward
        el_firstEdens_h1 = ROOT.TH1F("el_firstEdens_h1","el_firstEdens",100,0.,8.)
        histos["el_firstEdens"] = el_firstEdens_h1
        el_cellmaxfrac_h1 = ROOT.TH1F("el_cellmaxfrac_h1","el_cellmaxfrac",100,0.,1.)
        histos["el_cellmaxfrac"] = el_cellmaxfrac_h1
        el_longitudinal_h1 = ROOT.TH1F("el_longitudinal_h1","el_longitudinal",100,0.,1.)
        histos["el_longitudinal"] = el_longitudinal_h1
        el_secondlambda_h1 = ROOT.TH1F("el_secondlambda_h1","el_secondlambda",100,0.,100000.)
        histos["el_secondlambda"] = el_secondlambda_h1
        el_lateral_h1 = ROOT.TH1F("el_lateral_h1","el_lateral",100,0.,1.)
        histos["el_lateral"] = el_lateral_h1
        el_secondR_h1 = ROOT.TH1F("el_secondR_h1","el_secondR",100,0.,40000.)
        histos["el_secondR"] = el_secondR_h1
        el_centerlambda_h1 = ROOT.TH1F("el_centerlambda_h1","el_centerlambda",100,0.,2000.)
        histos["el_centerlambda"] = el_centerlambda_h1
        pass
    else : ## ERROR
        print "::: ERROR wrong flag :::"
        pass
        
    return histos

def defHistExpPho(n,histos) :
    
    if n == 0 : ## TDirectory Expert -> Photons
        pass
    elif n == 1 : ## TDirectory Expert -> Photons -> Identification
        pass
    elif n == 2 : ## TDirectory Expert -> Photons -> Kinematics
        pass
    elif n == 3 : ## TDirectory Expert -> Photons -> Conversion
        pass
    elif n == 4 : ## TDirectory Expert -> Photons -> EM
        pass
    elif n == 5 : ## TDirectory Expert -> Photons -> Isolation
        pass
    elif n == 6 : ## TDirectory Expert -> Photons -> Discriminants
        pass
    elif n == 7 : ## TDirectory Expert -> Photons -> Pointing
        pass
    elif n == 8 : ## TDirectory Expert -> Photons -> ClusterKinematics
        pass
    elif n == 9 : ## TDirectory Expert -> Photons -> L1
        pass
    elif n == 10 : ## TDirectory Expert -> Photons -> L2
        pass
    elif n == 11 : ## TDirectory Expert -> Photons -> EF
        pass
    elif n == 12 : ## TDirectory Expert -> Photons -> Jet
        pass
    elif n == 13 : ## TDirectory Expert -> Photons -> Track
        pass
    elif n == 14 : ## TDirectory Expert -> Photons -> Rawcl
        pass
    elif n == 15 : ## TDirectory Expert -> Photons -> Topo
        pass
    elif n == 16 : ## TDirectory Expert -> Photons -> MC
        pass
    else : ## ERROR
        print "::: ERROR wrong flag :::"
        pass
        
    return histos

def defHistExpMu(n,histos) :
    
    if n == 0 : ## TDirectory Expert -> Muons
        pass
    elif n == 1 : ## TDirectory Expert -> Muons -> Kinematics
        pass
    elif n == 2 : ## TDirectory Expert -> Muons -> Algorithms
        pass
    elif n == 3 : ## TDirectory Expert -> Muons -> EnergyLossCalo
        pass
    elif n == 4 : ## TDirectory Expert -> Muons -> nHitsSummary
        pass
    elif n == 5 : ## TDirectory Expert -> Muons -> Isolation
        pass
    elif n == 6 : ## TDirectory Expert -> Muons -> TrackPattern
        pass
    elif n == 7 : ## TDirectory Expert -> Muons -> TrackParameters
        pass
    elif n == 8 : ## TDirectory Expert -> Muons -> TrackQuality
        pass
    elif n == 9 : ## TDirectory Expert -> Muons -> L1
        pass
    elif n == 10 : ## TDirectory Expert -> Muons -> L2
        pass
    elif n == 11 : ## TDirectory Expert -> Muons -> EF
        pass
    elif n == 12 : ## TDirectory Expert -> Muons -> MC
        pass
    else : ## ERROR
        print "::: ERROR wrong flag :::"
        pass
        
    return histos

def defHistExpOther(n,histos) :
    

    if n == 0 : ## TDirectory Expert -> Other -> MC
        pass
    elif n == 1 : ## TDirectory Expert -> Other -> GlobalEvent
        pass
    elif n == 2 : ## TDirectory Expert -> Other -> Jet
        pass
    elif n == 3 : ## TDirectory Expert -> Other -> Met
        pass
    elif n == 4 : ## TDirectory Expert -> Other -> MinimumBias
        pass
    elif n == 5 : ## TDirectory Expert -> Other -> Trigger
        pass
    else : ## ERROR
        print "::: ERROR wrong flag :::"
        pass
        
    return histos

##############################################
## Get the branch and call Filling function ##
##############################################

def branchEle(tree,histoEleAll,histoEleAllLoose,histoEleAllMedium,histoEleAllTight,histoEleLoose,histoEleLooseLoose,histoEleLooseMedium,histoEleLooseTight,histoEleMedium,histoEleMediumLoose,histoEleMediumMedium,histoEleMediumTight,histoEleTight,histoEleTightLoose,histoEleTightMedium,histoEleTightTight,histoEleAuthor1,histoEleAuthor1Loose,histoEleAuthor1Medium,histoEleAuthor1Tight,histoEleAuthor2,histoEleAuthor2Loose,histoEleAuthor2Medium,histoEleAuthor2Tight,histoEleAuthor3,histoEleAuthor3Loose,histoEleAuthor3Medium,histoEleAuthor3Tight,histoEleAuthor8,histoEleAuthor8Loose,histoEleAuthor8Medium,histoEleAuthor8Tight) : ## Get the differents branches for TDirectory Electron

    bc_el_charge = getattr(tree,'el_charge')
    bc_el_author = getattr(tree,'el_author')
    bc_el_n = getattr(tree,'el_n')
    bc_el_eta = getattr(tree,'el_eta')
    bc_el_phi = getattr(tree,'el_phi')
    bc_el_pt = getattr(tree,'el_pt')
    bc_el_loose = getattr(tree,'el_loose')
    bc_el_medium = getattr(tree,'el_medium')
    bc_el_mediumIso = getattr(tree,'el_mediumIso')
    bc_el_tight = getattr(tree,'el_tight')
    bc_el_tightIso = getattr(tree,'el_tightIso')
    
    bc_el_weta2 = getattr(tree,'el_weta2')
    br_el_E237 = getattr(tree,'el_E237')
    bc_el_E277 = getattr(tree,'el_E277')
    bc_el_Ethad = getattr(tree,'el_Ethad')
    bc_el_Ethad1 = getattr(tree,'el_Ethad1')
    bc_el_f1 = getattr(tree,'el_f1')
    bc_el_wstot = getattr(tree,'el_wstot')
    bc_el_emaxs1 = getattr(tree,'el_emaxs1')
    bc_el_nBLHits = getattr(tree,'el_nBLHits')
    bc_el_nPixHits = getattr(tree,'el_nPixHits')
    bc_el_nSCTHits = getattr(tree,'el_nSCTHits')
    bc_el_nSiHits = getattr(tree,'el_nSiHits')
    bc_el_TRTHighTHitsRatio = getattr(tree,'el_TRTHighTHitsRatio')
    bc_el_nTRTHits = getattr(tree,'el_nTRTHits')
    bc_el_expectHitInBLayer = getattr(tree,'el_expectHitInBLayer')
    bc_el_cl_E = getattr(tree,'el_cl_E')
    bc_el_trackpt = getattr(tree,'el_trackpt')

    #########
    ## ALL ##
    histoEleAll["el_charge"] = fill(bc_el_charge,histoEleAll["el_charge"])
    histoEleAll["el_author"] = fill(bc_el_author,histoEleAll["el_author"])
    histoEleAll["el_n"] = fill(bc_el_n,histoEleAll["el_n"])
    histoEleAll["el_eta"] = fill(bc_el_eta,histoEleAll["el_eta"])
    histoEleAll["el_phi"] = fill(bc_el_phi,histoEleAll["el_phi"])
    histoEleAll["el_pt"] = fill(bc_el_pt,histoEleAll["el_pt"])
    histoEleAll["el_loose"] = fill(bc_el_loose,histoEleAll["el_loose"])
    histoEleAll["el_medium"] = fill(bc_el_medium,histoEleAll["el_medium"])
    histoEleAll["el_mediumIso"] = fill(bc_el_mediumIso,histoEleAll["el_mediumIso"])
    histoEleAll["el_tight"] = fill(bc_el_tight,histoEleAll["el_tight"])
    histoEleAll["el_tightIso"] = fill(bc_el_tightIso,histoEleAll["el_tightIso"])

    ## Loose variables
    histoEleAllLoose["el_weta2"] = fill(bc_el_weta2,histoEleAllLoose["el_weta2"])
    histoEleAllLoose["el_E237"] = fill(br_el_E237,histoEleAllLoose["el_E237"])
    histoEleAllLoose["el_E277"] = fill(bc_el_E277,histoEleAllLoose["el_E277"])
    histoEleAllLoose["el_Ethad"] = fill(bc_el_Ethad,histoEleAllLoose["el_Ethad"])
    histoEleAllLoose["el_Ethad1"] = fill(bc_el_Ethad1,histoEleAllLoose["el_Ethad1"])

    ## Medium variables
    histoEleAllMedium["el_f1"] = fill(bc_el_f1,histoEleAllMedium["el_f1"])
    histoEleAllMedium["el_wstot"] = fill(bc_el_wstot,histoEleAllMedium["el_wstot"])
    histoEleAllMedium["el_emaxs1"] = fill(bc_el_emaxs1,histoEleAllMedium["el_emaxs1"])
    histoEleAllMedium["el_nPixHits"] = fill(bc_el_nPixHits,histoEleAllMedium["el_nPixHits"])
    histoEleAllMedium["el_nSCTHits"] = fill(bc_el_nSCTHits,histoEleAllMedium["el_nSCTHits"])
    histoEleAllMedium["el_nSiHits"] = fill(bc_el_nSiHits,histoEleAllMedium["el_nSiHits"])

    ## Tight variables
    histoEleAllTight["el_TRTHighTHitsRatio"] = fill(bc_el_TRTHighTHitsRatio,histoEleAllTight["el_TRTHighTHitsRatio"])
    histoEleAllTight["el_nTRTHits"] = fill(bc_el_nTRTHits,histoEleAllTight["el_nTRTHits"])
    histoEleAllTight["el_expectHitInBLayer"] = fill(bc_el_expectHitInBLayer,histoEleAllTight["el_expectHitInBLayer"])
    histoEleAllTight["el_cl_E"] = fill(bc_el_cl_E,histoEleAllTight["el_cl_E"])
    histoEleAllTight["el_trackpt"] = fill(bc_el_trackpt,histoEleAllTight["el_trackpt"])
    histoEleAllTight["el_nBLHits"] = fill(bc_el_nBLHits,histoEleAllTight["el_nBLHits"])

    ##############
    ## if Loose ##
    histoEleLoose["el_charge_ifLoose"] = fillIsEM(bc_el_charge,histoEleLoose["el_charge_ifLoose"],bc_el_loose)
    histoEleLoose["el_author_ifLoose"] = fillIsEM(bc_el_author,histoEleLoose["el_author_ifLoose"],bc_el_loose)
    histoEleLoose["el_n_ifLoose"] = fillIsEM_N(bc_el_n,histoEleLoose["el_n_ifLoose"],bc_el_loose)
    histoEleLoose["el_eta_ifLoose"] = fillIsEM(bc_el_eta,histoEleLoose["el_eta_ifLoose"],bc_el_loose)
    histoEleLoose["el_phi_ifLoose"] = fillIsEM(bc_el_phi,histoEleLoose["el_phi_ifLoose"],bc_el_loose)
    histoEleLoose["el_pt_ifLoose"] = fillIsEM(bc_el_pt,histoEleLoose["el_pt_ifLoose"],bc_el_loose)
    histoEleLoose["el_loose_ifLoose"] = fillIsEM(bc_el_loose,histoEleLoose["el_loose_ifLoose"],bc_el_loose)
    histoEleLoose["el_medium_ifLoose"] = fillIsEM(bc_el_medium,histoEleLoose["el_medium_ifLoose"],bc_el_loose)
    histoEleLoose["el_mediumIso_ifLoose"] = fillIsEM(bc_el_mediumIso,histoEleLoose["el_mediumIso_ifLoose"],bc_el_loose)
    histoEleLoose["el_tight_ifLoose"] = fillIsEM(bc_el_tight,histoEleLoose["el_tight_ifLoose"],bc_el_loose)
    histoEleLoose["el_tightIso_ifLoose"] = fillIsEM(bc_el_tightIso,histoEleLoose["el_tightIso_ifLoose"],bc_el_loose)

    ## Loose variables
    histoEleLooseLoose["el_weta2_ifLoose"] = fillIsEM(bc_el_weta2,histoEleLooseLoose["el_weta2_ifLoose"],bc_el_loose)
    histoEleLooseLoose["el_E237_ifLoose"] = fillIsEM(br_el_E237,histoEleLooseLoose["el_E237_ifLoose"],bc_el_loose)
    histoEleLooseLoose["el_E277_ifLoose"] = fillIsEM(bc_el_E277,histoEleLooseLoose["el_E277_ifLoose"],bc_el_loose)
    histoEleLooseLoose["el_Ethad_ifLoose"] = fillIsEM(bc_el_Ethad,histoEleLooseLoose["el_Ethad_ifLoose"],bc_el_loose)
    histoEleLooseLoose["el_Ethad1_ifLoose"] = fillIsEM(bc_el_Ethad1,histoEleLooseLoose["el_Ethad1_ifLoose"],bc_el_loose)

    ## Medium variables
    histoEleLooseMedium["el_f1_ifLoose"] = fillIsEM(bc_el_f1,histoEleLooseMedium["el_f1_ifLoose"],bc_el_loose)
    histoEleLooseMedium["el_wstot_ifLoose"] = fillIsEM(bc_el_wstot,histoEleLooseMedium["el_wstot_ifLoose"],bc_el_loose)
    histoEleLooseMedium["el_emaxs1_ifLoose"] = fillIsEM(bc_el_emaxs1,histoEleLooseMedium["el_emaxs1_ifLoose"],bc_el_loose)
    histoEleLooseMedium["el_nPixHits_ifLoose"] = fillIsEM(bc_el_nPixHits,histoEleLooseMedium["el_nPixHits_ifLoose"],bc_el_loose)
    histoEleLooseMedium["el_nSCTHits_ifLoose"] = fillIsEM(bc_el_nSCTHits,histoEleLooseMedium["el_nSCTHits_ifLoose"],bc_el_loose)
    histoEleLooseMedium["el_nSiHits_ifLoose"] = fillIsEM(bc_el_nSiHits,histoEleLooseMedium["el_nSiHits_ifLoose"],bc_el_loose)

    ## Tight variables
    histoEleLooseTight["el_TRTHighTHitsRatio_ifLoose"] = fillIsEM(bc_el_TRTHighTHitsRatio,histoEleLooseTight["el_TRTHighTHitsRatio_ifLoose"],bc_el_loose)
    histoEleLooseTight["el_nTRTHits_ifLoose"] = fillIsEM(bc_el_nTRTHits,histoEleLooseTight["el_nTRTHits_ifLoose"],bc_el_loose)
    histoEleLooseTight["el_expectHitInBLayer_ifLoose"] = fillIsEM(bc_el_expectHitInBLayer,histoEleLooseTight["el_expectHitInBLayer_ifLoose"],bc_el_loose)
    histoEleLooseTight["el_cl_E_ifLoose"] = fillIsEM(bc_el_cl_E,histoEleLooseTight["el_cl_E_ifLoose"],bc_el_loose)
    histoEleLooseTight["el_trackpt_ifLoose"] = fillIsEM(bc_el_trackpt,histoEleLooseTight["el_trackpt_ifLoose"],bc_el_loose)
    histoEleLooseTight["el_nBLHits_ifLoose"] = fillIsEM(bc_el_nBLHits,histoEleLooseTight["el_nBLHits_ifLoose"],bc_el_loose)

    ################
    ## if Medium ##
    histoEleMedium["el_charge_ifMedium"] = fillIsEM(bc_el_charge,histoEleMedium["el_charge_ifMedium"],bc_el_medium)
    histoEleMedium["el_author_ifMedium"] = fillIsEM(bc_el_author,histoEleMedium["el_author_ifMedium"],bc_el_medium)
    histoEleMedium["el_n_ifMedium"] = fillIsEM_N(bc_el_n,histoEleMedium["el_n_ifMedium"],bc_el_medium)
    histoEleMedium["el_eta_ifMedium"] = fillIsEM(bc_el_eta,histoEleMedium["el_eta_ifMedium"],bc_el_medium)
    histoEleMedium["el_phi_ifMedium"] = fillIsEM(bc_el_phi,histoEleMedium["el_phi_ifMedium"],bc_el_medium)
    histoEleMedium["el_pt_ifMedium"] = fillIsEM(bc_el_pt,histoEleMedium["el_pt_ifMedium"],bc_el_medium)
    histoEleMedium["el_loose_ifMedium"] = fillIsEM(bc_el_loose,histoEleMedium["el_loose_ifMedium"],bc_el_medium)
    histoEleMedium["el_medium_ifMedium"] = fillIsEM(bc_el_medium,histoEleMedium["el_medium_ifMedium"],bc_el_medium)
    histoEleMedium["el_mediumIso_ifMedium"] = fillIsEM(bc_el_mediumIso,histoEleMedium["el_mediumIso_ifMedium"],bc_el_medium)
    histoEleMedium["el_tight_ifMedium"] = fillIsEM(bc_el_tight,histoEleMedium["el_tight_ifMedium"],bc_el_medium)
    histoEleMedium["el_tightIso_ifMedium"] = fillIsEM(bc_el_tightIso,histoEleMedium["el_tightIso_ifMedium"],bc_el_medium)

    ## Loose variables
    histoEleMediumLoose["el_weta2_ifMedium"] = fillIsEM(bc_el_weta2,histoEleMediumLoose["el_weta2_ifMedium"],bc_el_medium)
    histoEleMediumLoose["el_E237_ifMedium"] = fillIsEM(br_el_E237,histoEleMediumLoose["el_E237_ifMedium"],bc_el_medium)
    histoEleMediumLoose["el_E277_ifMedium"] = fillIsEM(bc_el_E277,histoEleMediumLoose["el_E277_ifMedium"],bc_el_medium)
    histoEleMediumLoose["el_Ethad_ifMedium"] = fillIsEM(bc_el_Ethad,histoEleMediumLoose["el_Ethad_ifMedium"],bc_el_medium)
    histoEleMediumLoose["el_Ethad1_ifMedium"] = fillIsEM(bc_el_Ethad1,histoEleMediumLoose["el_Ethad1_ifMedium"],bc_el_medium)

    ## Medium variables
    histoEleMediumMedium["el_f1_ifMedium"] = fillIsEM(bc_el_f1,histoEleMediumMedium["el_f1_ifMedium"],bc_el_medium)
    histoEleMediumMedium["el_wstot_ifMedium"] = fillIsEM(bc_el_wstot,histoEleMediumMedium["el_wstot_ifMedium"],bc_el_medium)
    histoEleMediumMedium["el_emaxs1_ifMedium"] = fillIsEM(bc_el_emaxs1,histoEleMediumMedium["el_emaxs1_ifMedium"],bc_el_medium)
    histoEleMediumMedium["el_nPixHits_ifMedium"] = fillIsEM(bc_el_nPixHits,histoEleMediumMedium["el_nPixHits_ifMedium"],bc_el_medium)
    histoEleMediumMedium["el_nSCTHits_ifMedium"] = fillIsEM(bc_el_nSCTHits,histoEleMediumMedium["el_nSCTHits_ifMedium"],bc_el_medium)
    histoEleMediumMedium["el_nSiHits_ifMedium"] = fillIsEM(bc_el_nSiHits,histoEleMediumMedium["el_nSiHits_ifMedium"],bc_el_medium)

    ## Tight variables
    histoEleMediumTight["el_TRTHighTHitsRatio_ifMedium"] = fillIsEM(bc_el_TRTHighTHitsRatio,histoEleMediumTight["el_TRTHighTHitsRatio_ifMedium"],bc_el_medium)
    histoEleMediumTight["el_nTRTHits_ifMedium"] = fillIsEM(bc_el_nTRTHits,histoEleMediumTight["el_nTRTHits_ifMedium"],bc_el_medium)
    histoEleMediumTight["el_expectHitInBLayer_ifMedium"] = fillIsEM(bc_el_expectHitInBLayer,histoEleMediumTight["el_expectHitInBLayer_ifMedium"],bc_el_medium)
    histoEleMediumTight["el_cl_E_ifMedium"] = fillIsEM(bc_el_cl_E,histoEleMediumTight["el_cl_E_ifMedium"],bc_el_medium)
    histoEleMediumTight["el_trackpt_ifMedium"] = fillIsEM(bc_el_trackpt,histoEleMediumTight["el_trackpt_ifMedium"],bc_el_medium)
    histoEleMediumTight["el_nBLHits_ifMedium"] = fillIsEM(bc_el_nBLHits,histoEleMediumTight["el_nBLHits_ifMedium"],bc_el_medium)

    ################
    ## if Tight ##
    histoEleTight["el_charge_ifTight"] = fillIsEM(bc_el_charge,histoEleTight["el_charge_ifTight"],bc_el_tight)
    histoEleTight["el_author_ifTight"] = fillIsEM(bc_el_author,histoEleTight["el_author_ifTight"],bc_el_tight)
    histoEleTight["el_n_ifTight"] = fillIsEM_N(bc_el_n,histoEleTight["el_n_ifTight"],bc_el_tight)
    histoEleTight["el_eta_ifTight"] = fillIsEM(bc_el_eta,histoEleTight["el_eta_ifTight"],bc_el_tight)
    histoEleTight["el_phi_ifTight"] = fillIsEM(bc_el_phi,histoEleTight["el_phi_ifTight"],bc_el_tight)
    histoEleTight["el_pt_ifTight"] = fillIsEM(bc_el_pt,histoEleTight["el_pt_ifTight"],bc_el_tight)
    histoEleTight["el_loose_ifTight"] = fillIsEM(bc_el_loose,histoEleTight["el_loose_ifTight"],bc_el_tight)
    histoEleTight["el_medium_ifTight"] = fillIsEM(bc_el_medium,histoEleTight["el_medium_ifTight"],bc_el_tight)
    histoEleTight["el_mediumIso_ifTight"] = fillIsEM(bc_el_mediumIso,histoEleTight["el_mediumIso_ifTight"],bc_el_tight)
    histoEleTight["el_tight_ifTight"] = fillIsEM(bc_el_tight,histoEleTight["el_tight_ifTight"],bc_el_tight)
    histoEleTight["el_tightIso_ifTight"] = fillIsEM(bc_el_tightIso,histoEleTight["el_tightIso_ifTight"],bc_el_tight)

    ## Loose variables
    histoEleTightLoose["el_weta2_ifTight"] = fillIsEM(bc_el_weta2,histoEleTightLoose["el_weta2_ifTight"],bc_el_tight)
    histoEleTightLoose["el_E237_ifTight"] = fillIsEM(br_el_E237,histoEleTightLoose["el_E237_ifTight"],bc_el_tight)
    histoEleTightLoose["el_E277_ifTight"] = fillIsEM(bc_el_E277,histoEleTightLoose["el_E277_ifTight"],bc_el_tight)
    histoEleTightLoose["el_Ethad_ifTight"] = fillIsEM(bc_el_Ethad,histoEleTightLoose["el_Ethad_ifTight"],bc_el_tight)
    histoEleTightLoose["el_Ethad1_ifTight"] = fillIsEM(bc_el_Ethad1,histoEleTightLoose["el_Ethad1_ifTight"],bc_el_tight)

    ## Medium variables
    histoEleTightMedium["el_f1_ifTight"] = fillIsEM(bc_el_f1,histoEleTightMedium["el_f1_ifTight"],bc_el_tight)
    histoEleTightMedium["el_wstot_ifTight"] = fillIsEM(bc_el_wstot,histoEleTightMedium["el_wstot_ifTight"],bc_el_tight)
    histoEleTightMedium["el_emaxs1_ifTight"] = fillIsEM(bc_el_emaxs1,histoEleTightMedium["el_emaxs1_ifTight"],bc_el_tight)
    histoEleTightMedium["el_nPixHits_ifTight"] = fillIsEM(bc_el_nPixHits,histoEleTightMedium["el_nPixHits_ifTight"],bc_el_tight)
    histoEleTightMedium["el_nSCTHits_ifTight"] = fillIsEM(bc_el_nSCTHits,histoEleTightMedium["el_nSCTHits_ifTight"],bc_el_tight)
    histoEleTightMedium["el_nSiHits_ifTight"] = fillIsEM(bc_el_nSiHits,histoEleTightMedium["el_nSiHits_ifTight"],bc_el_tight)

    ## Tight variables
    histoEleTightTight["el_TRTHighTHitsRatio_ifTight"] = fillIsEM(bc_el_TRTHighTHitsRatio,histoEleTightTight["el_TRTHighTHitsRatio_ifTight"],bc_el_tight)
    histoEleTightTight["el_nTRTHits_ifTight"] = fillIsEM(bc_el_nTRTHits,histoEleTightTight["el_nTRTHits_ifTight"],bc_el_tight)
    histoEleTightTight["el_expectHitInBLayer_ifTight"] = fillIsEM(bc_el_expectHitInBLayer,histoEleTightTight["el_expectHitInBLayer_ifTight"],bc_el_tight)
    histoEleTightTight["el_cl_E_ifTight"] = fillIsEM(bc_el_cl_E,histoEleTightTight["el_cl_E_ifTight"],bc_el_tight)
    histoEleTightTight["el_trackpt_ifTight"] = fillIsEM(bc_el_trackpt,histoEleTightTight["el_trackpt_ifTight"],bc_el_tight)
    histoEleTightTight["el_nBLHits_ifTight"] = fillIsEM(bc_el_nBLHits,histoEleTightTight["el_nBLHits_ifTight"],bc_el_tight)

    ################
    ## if Author1 ##
    histoEleAuthor1["el_charge_ifAuthor1"] = fillAuthor(bc_el_charge,histoEleAuthor1["el_charge_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_author_ifAuthor1"] = fillAuthor(bc_el_author,histoEleAuthor1["el_author_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_n_ifAuthor1"] = fillAuthor_N(bc_el_n,histoEleAuthor1["el_n_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_eta_ifAuthor1"] = fillAuthor(bc_el_eta,histoEleAuthor1["el_eta_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_phi_ifAuthor1"] = fillAuthor(bc_el_phi,histoEleAuthor1["el_phi_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_pt_ifAuthor1"] = fillAuthor(bc_el_pt,histoEleAuthor1["el_pt_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_loose_ifAuthor1"] = fillAuthor(bc_el_loose,histoEleAuthor1["el_loose_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_medium_ifAuthor1"] = fillAuthor(bc_el_medium,histoEleAuthor1["el_medium_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_mediumIso_ifAuthor1"] = fillAuthor(bc_el_mediumIso,histoEleAuthor1["el_mediumIso_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_tight_ifAuthor1"] = fillAuthor(bc_el_tight,histoEleAuthor1["el_tight_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1["el_tightIso_ifAuthor1"] = fillAuthor(bc_el_tightIso,histoEleAuthor1["el_tightIso_ifAuthor1"],bc_el_author,1)

    ## Author1 variables
    histoEleAuthor1Loose["el_weta2_ifAuthor1"] = fillAuthor(bc_el_weta2,histoEleAuthor1Loose["el_weta2_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Loose["el_E237_ifAuthor1"] = fillAuthor(br_el_E237,histoEleAuthor1Loose["el_E237_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Loose["el_E277_ifAuthor1"] = fillAuthor(bc_el_E277,histoEleAuthor1Loose["el_E277_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Loose["el_Ethad_ifAuthor1"] = fillAuthor(bc_el_Ethad,histoEleAuthor1Loose["el_Ethad_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Loose["el_Ethad1_ifAuthor1"] = fillAuthor(bc_el_Ethad1,histoEleAuthor1Loose["el_Ethad1_ifAuthor1"],bc_el_author,1)

    ## Medium variables
    histoEleAuthor1Medium["el_f1_ifAuthor1"] = fillAuthor(bc_el_f1,histoEleAuthor1Medium["el_f1_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Medium["el_wstot_ifAuthor1"] = fillAuthor(bc_el_wstot,histoEleAuthor1Medium["el_wstot_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Medium["el_emaxs1_ifAuthor1"] = fillAuthor(bc_el_emaxs1,histoEleAuthor1Medium["el_emaxs1_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Medium["el_nPixHits_ifAuthor1"] = fillAuthor(bc_el_nPixHits,histoEleAuthor1Medium["el_nPixHits_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Medium["el_nSCTHits_ifAuthor1"] = fillAuthor(bc_el_nSCTHits,histoEleAuthor1Medium["el_nSCTHits_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Medium["el_nSiHits_ifAuthor1"] = fillAuthor(bc_el_nSiHits,histoEleAuthor1Medium["el_nSiHits_ifAuthor1"],bc_el_author,1)

    ## Tight variables
    histoEleAuthor1Tight["el_TRTHighTHitsRatio_ifAuthor1"] = fillAuthor(bc_el_TRTHighTHitsRatio,histoEleAuthor1Tight["el_TRTHighTHitsRatio_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Tight["el_nTRTHits_ifAuthor1"] = fillAuthor(bc_el_nTRTHits,histoEleAuthor1Tight["el_nTRTHits_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Tight["el_expectHitInBLayer_ifAuthor1"] = fillAuthor(bc_el_expectHitInBLayer,histoEleAuthor1Tight["el_expectHitInBLayer_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Tight["el_cl_E_ifAuthor1"] = fillAuthor(bc_el_cl_E,histoEleAuthor1Tight["el_cl_E_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Tight["el_trackpt_ifAuthor1"] = fillAuthor(bc_el_trackpt,histoEleAuthor1Tight["el_trackpt_ifAuthor1"],bc_el_author,1)
    histoEleAuthor1Tight["el_nBLHits_ifAuthor1"] = fillAuthor(bc_el_nBLHits,histoEleAuthor1Tight["el_nBLHits_ifAuthor1"],bc_el_author,1)

    ################
    ## if Author2 ##
    histoEleAuthor2["el_charge_ifAuthor2"] = fillAuthor(bc_el_charge,histoEleAuthor2["el_charge_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_author_ifAuthor2"] = fillAuthor(bc_el_author,histoEleAuthor2["el_author_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_n_ifAuthor2"] = fillAuthor_N(bc_el_n,histoEleAuthor2["el_n_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_eta_ifAuthor2"] = fillAuthor(bc_el_eta,histoEleAuthor2["el_eta_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_phi_ifAuthor2"] = fillAuthor(bc_el_phi,histoEleAuthor2["el_phi_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_pt_ifAuthor2"] = fillAuthor(bc_el_pt,histoEleAuthor2["el_pt_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_loose_ifAuthor2"] = fillAuthor(bc_el_loose,histoEleAuthor2["el_loose_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_medium_ifAuthor2"] = fillAuthor(bc_el_medium,histoEleAuthor2["el_medium_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_mediumIso_ifAuthor2"] = fillAuthor(bc_el_mediumIso,histoEleAuthor2["el_mediumIso_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_tight_ifAuthor2"] = fillAuthor(bc_el_tight,histoEleAuthor2["el_tight_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2["el_tightIso_ifAuthor2"] = fillAuthor(bc_el_tightIso,histoEleAuthor2["el_tightIso_ifAuthor2"],bc_el_author,2)

    ## Loose variables
    histoEleAuthor2Loose["el_weta2_ifAuthor2"] = fillAuthor(bc_el_weta2,histoEleAuthor2Loose["el_weta2_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Loose["el_E237_ifAuthor2"] = fillAuthor(br_el_E237,histoEleAuthor2Loose["el_E237_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Loose["el_E277_ifAuthor2"] = fillAuthor(bc_el_E277,histoEleAuthor2Loose["el_E277_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Loose["el_Ethad_ifAuthor2"] = fillAuthor(bc_el_Ethad,histoEleAuthor2Loose["el_Ethad_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Loose["el_Ethad1_ifAuthor2"] = fillAuthor(bc_el_Ethad1,histoEleAuthor2Loose["el_Ethad1_ifAuthor2"],bc_el_author,2)

    ## Medium variables
    histoEleAuthor2Medium["el_f1_ifAuthor2"] = fillAuthor(bc_el_f1,histoEleAuthor2Medium["el_f1_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Medium["el_wstot_ifAuthor2"] = fillAuthor(bc_el_wstot,histoEleAuthor2Medium["el_wstot_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Medium["el_emaxs1_ifAuthor2"] = fillAuthor(bc_el_emaxs1,histoEleAuthor2Medium["el_emaxs1_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Medium["el_nPixHits_ifAuthor2"] = fillAuthor(bc_el_nPixHits,histoEleAuthor2Medium["el_nPixHits_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Medium["el_nSCTHits_ifAuthor2"] = fillAuthor(bc_el_nSCTHits,histoEleAuthor2Medium["el_nSCTHits_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Medium["el_nSiHits_ifAuthor2"] = fillAuthor(bc_el_nSiHits,histoEleAuthor2Medium["el_nSiHits_ifAuthor2"],bc_el_author,2)

    ## Tight variables
    histoEleAuthor2Tight["el_TRTHighTHitsRatio_ifAuthor2"] = fillAuthor(bc_el_TRTHighTHitsRatio,histoEleAuthor2Tight["el_TRTHighTHitsRatio_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Tight["el_nTRTHits_ifAuthor2"] = fillAuthor(bc_el_nTRTHits,histoEleAuthor2Tight["el_nTRTHits_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Tight["el_expectHitInBLayer_ifAuthor2"] = fillAuthor(bc_el_expectHitInBLayer,histoEleAuthor2Tight["el_expectHitInBLayer_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Tight["el_cl_E_ifAuthor2"] = fillAuthor(bc_el_cl_E,histoEleAuthor2Tight["el_cl_E_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Tight["el_trackpt_ifAuthor2"] = fillAuthor(bc_el_trackpt,histoEleAuthor2Tight["el_trackpt_ifAuthor2"],bc_el_author,2)
    histoEleAuthor2Tight["el_nBLHits_ifAuthor2"] = fillAuthor(bc_el_nBLHits,histoEleAuthor2Tight["el_nBLHits_ifAuthor2"],bc_el_author,2)

    ################
    ## if Author3 ##
    histoEleAuthor3["el_charge_ifAuthor3"] = fillAuthor(bc_el_charge,histoEleAuthor3["el_charge_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_author_ifAuthor3"] = fillAuthor(bc_el_author,histoEleAuthor3["el_author_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_n_ifAuthor3"] = fillAuthor_N(bc_el_n,histoEleAuthor3["el_n_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_eta_ifAuthor3"] = fillAuthor(bc_el_eta,histoEleAuthor3["el_eta_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_phi_ifAuthor3"] = fillAuthor(bc_el_phi,histoEleAuthor3["el_phi_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_pt_ifAuthor3"] = fillAuthor(bc_el_pt,histoEleAuthor3["el_pt_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_loose_ifAuthor3"] = fillAuthor(bc_el_loose,histoEleAuthor3["el_loose_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_medium_ifAuthor3"] = fillAuthor(bc_el_medium,histoEleAuthor3["el_medium_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_mediumIso_ifAuthor3"] = fillAuthor(bc_el_mediumIso,histoEleAuthor3["el_mediumIso_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_tight_ifAuthor3"] = fillAuthor(bc_el_tight,histoEleAuthor3["el_tight_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3["el_tightIso_ifAuthor3"] = fillAuthor(bc_el_tightIso,histoEleAuthor3["el_tightIso_ifAuthor3"],bc_el_author,3)

    ## Author3 variables
    histoEleAuthor3Loose["el_weta2_ifAuthor3"] = fillAuthor(bc_el_weta2,histoEleAuthor3Loose["el_weta2_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Loose["el_E237_ifAuthor3"] = fillAuthor(br_el_E237,histoEleAuthor3Loose["el_E237_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Loose["el_E277_ifAuthor3"] = fillAuthor(bc_el_E277,histoEleAuthor3Loose["el_E277_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Loose["el_Ethad_ifAuthor3"] = fillAuthor(bc_el_Ethad,histoEleAuthor3Loose["el_Ethad_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Loose["el_Ethad1_ifAuthor3"] = fillAuthor(bc_el_Ethad1,histoEleAuthor3Loose["el_Ethad1_ifAuthor3"],bc_el_author,3)

    ## Medium variables
    histoEleAuthor3Medium["el_f1_ifAuthor3"] = fillAuthor(bc_el_f1,histoEleAuthor3Medium["el_f1_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Medium["el_wstot_ifAuthor3"] = fillAuthor(bc_el_wstot,histoEleAuthor3Medium["el_wstot_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Medium["el_emaxs1_ifAuthor3"] = fillAuthor(bc_el_emaxs1,histoEleAuthor3Medium["el_emaxs1_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Medium["el_nPixHits_ifAuthor3"] = fillAuthor(bc_el_nPixHits,histoEleAuthor3Medium["el_nPixHits_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Medium["el_nSCTHits_ifAuthor3"] = fillAuthor(bc_el_nSCTHits,histoEleAuthor3Medium["el_nSCTHits_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Medium["el_nSiHits_ifAuthor3"] = fillAuthor(bc_el_nSiHits,histoEleAuthor3Medium["el_nSiHits_ifAuthor3"],bc_el_author,3)

    ## Tight variables
    histoEleAuthor3Tight["el_TRTHighTHitsRatio_ifAuthor3"] = fillAuthor(bc_el_TRTHighTHitsRatio,histoEleAuthor3Tight["el_TRTHighTHitsRatio_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Tight["el_nTRTHits_ifAuthor3"] = fillAuthor(bc_el_nTRTHits,histoEleAuthor3Tight["el_nTRTHits_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Tight["el_expectHitInBLayer_ifAuthor3"] = fillAuthor(bc_el_expectHitInBLayer,histoEleAuthor3Tight["el_expectHitInBLayer_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Tight["el_cl_E_ifAuthor3"] = fillAuthor(bc_el_cl_E,histoEleAuthor3Tight["el_cl_E_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Tight["el_trackpt_ifAuthor3"] = fillAuthor(bc_el_trackpt,histoEleAuthor3Tight["el_trackpt_ifAuthor3"],bc_el_author,3)
    histoEleAuthor3Tight["el_nBLHits_ifAuthor3"] = fillAuthor(bc_el_nBLHits,histoEleAuthor3Tight["el_nBLHits_ifAuthor3"],bc_el_author,3)

    ################
    ## if Author8 ##
    histoEleAuthor8["el_charge_ifAuthor8"] = fillAuthor(bc_el_charge,histoEleAuthor8["el_charge_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_author_ifAuthor8"] = fillAuthor(bc_el_author,histoEleAuthor8["el_author_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_n_ifAuthor8"] = fillAuthor_N(bc_el_n,histoEleAuthor8["el_n_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_eta_ifAuthor8"] = fillAuthor(bc_el_eta,histoEleAuthor8["el_eta_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_phi_ifAuthor8"] = fillAuthor(bc_el_phi,histoEleAuthor8["el_phi_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_pt_ifAuthor8"] = fillAuthor(bc_el_pt,histoEleAuthor8["el_pt_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_loose_ifAuthor8"] = fillAuthor(bc_el_loose,histoEleAuthor8["el_loose_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_medium_ifAuthor8"] = fillAuthor(bc_el_medium,histoEleAuthor8["el_medium_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_mediumIso_ifAuthor8"] = fillAuthor(bc_el_mediumIso,histoEleAuthor8["el_mediumIso_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_tight_ifAuthor8"] = fillAuthor(bc_el_tight,histoEleAuthor8["el_tight_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8["el_tightIso_ifAuthor8"] = fillAuthor(bc_el_tightIso,histoEleAuthor8["el_tightIso_ifAuthor8"],bc_el_author,8)

    ## Author8 variables
    histoEleAuthor8Loose["el_weta2_ifAuthor8"] = fillAuthor(bc_el_weta2,histoEleAuthor8Loose["el_weta2_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Loose["el_E237_ifAuthor8"] = fillAuthor(br_el_E237,histoEleAuthor8Loose["el_E237_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Loose["el_E277_ifAuthor8"] = fillAuthor(bc_el_E277,histoEleAuthor8Loose["el_E277_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Loose["el_Ethad_ifAuthor8"] = fillAuthor(bc_el_Ethad,histoEleAuthor8Loose["el_Ethad_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Loose["el_Ethad1_ifAuthor8"] = fillAuthor(bc_el_Ethad1,histoEleAuthor8Loose["el_Ethad1_ifAuthor8"],bc_el_author,8)

    ## Medium variables
    histoEleAuthor8Medium["el_f1_ifAuthor8"] = fillAuthor(bc_el_f1,histoEleAuthor8Medium["el_f1_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Medium["el_wstot_ifAuthor8"] = fillAuthor(bc_el_wstot,histoEleAuthor8Medium["el_wstot_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Medium["el_emaxs1_ifAuthor8"] = fillAuthor(bc_el_emaxs1,histoEleAuthor8Medium["el_emaxs1_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Medium["el_nPixHits_ifAuthor8"] = fillAuthor(bc_el_nPixHits,histoEleAuthor8Medium["el_nPixHits_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Medium["el_nSCTHits_ifAuthor8"] = fillAuthor(bc_el_nSCTHits,histoEleAuthor8Medium["el_nSCTHits_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Medium["el_nSiHits_ifAuthor8"] = fillAuthor(bc_el_nSiHits,histoEleAuthor8Medium["el_nSiHits_ifAuthor8"],bc_el_author,8)

    ## Tight variables
    histoEleAuthor8Tight["el_TRTHighTHitsRatio_ifAuthor8"] = fillAuthor(bc_el_TRTHighTHitsRatio,histoEleAuthor8Tight["el_TRTHighTHitsRatio_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Tight["el_nTRTHits_ifAuthor8"] = fillAuthor(bc_el_nTRTHits,histoEleAuthor8Tight["el_nTRTHits_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Tight["el_expectHitInBLayer_ifAuthor8"] = fillAuthor(bc_el_expectHitInBLayer,histoEleAuthor8Tight["el_expectHitInBLayer_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Tight["el_cl_E_ifAuthor8"] = fillAuthor(bc_el_cl_E,histoEleAuthor8Tight["el_cl_E_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Tight["el_trackpt_ifAuthor8"] = fillAuthor(bc_el_trackpt,histoEleAuthor8Tight["el_trackpt_ifAuthor8"],bc_el_author,8)
    histoEleAuthor8Tight["el_nBLHits_ifAuthor8"] = fillAuthor(bc_el_nBLHits,histoEleAuthor8Tight["el_nBLHits_ifAuthor8"],bc_el_author,8)

    
    return True


def branchPho(tree,histoPhoAll,histoPhoAllLoose,histoPhoAllTight,histoPhoLoose,histoPhoLooseLoose,histoPhoLooseTight,histoPhoTight,histoPhoTightLoose,histoPhoTightTight,histoPhoAuthor4,histoPhoAuthor4Loose,histoPhoAuthor4Tight,histoPhoAuthor16,histoPhoAuthor16Loose,histoPhoAuthor16Tight,histoPhoIsConv,histoPhoIsConvLoose,histoPhoIsConvTight,histoPhoUnConv,histoPhoUnConvLoose,histoPhoUnConvTight) :
    
    bc_ph_phi = getattr(tree,'ph_phi')
    bc_ph_pt = getattr(tree,'ph_pt')
    bc_ph_E = getattr(tree,'ph_E')
    bc_ph_tight = getattr(tree,'ph_tight')
    bc_ph_tightIso = getattr(tree,'ph_tightIso')
    bc_ph_eta = getattr(tree,'ph_eta')
    bc_ph_author = getattr(tree,'ph_author')
    bc_ph_loose = getattr(tree,'ph_loose')
    bc_ph_n = getattr(tree,'ph_n')
    bc_ph_isConv = getattr(tree,'ph_isConv')
    bc_ph_nConv = getattr(tree,'ph_nConv')
    bc_ph_Es2 = getattr(tree,'ph_Es2')
    bc_ph_E233 = getattr(tree,'ph_E233')
    bc_ph_E237 = getattr(tree,'ph_E237')
    bc_ph_E277 = getattr(tree,'ph_E277')
    bc_ph_weta2 = getattr(tree,'ph_weta2')
    bc_ph_Ethad = getattr(tree,'ph_Ethad')
    bc_ph_Ethad1 = getattr(tree,'ph_Ethad1')
    bc_ph_wstot = getattr(tree,'ph_wstot')
    bc_ph_deltaEmax2 = getattr(tree,'ph_deltaEmax2')
    bc_ph_emaxs1 = getattr(tree,'ph_emaxs1')
    bc_ph_deltaEs = getattr(tree,'ph_deltaEs')
    bc_ph_cl_E = getattr(tree,'ph_cl_E')
    bc_ph_f1 = getattr(tree,'ph_f1')
    
    #########
    ## ALL ##
    histoPhoAll["ph_phi"] = fill(bc_ph_phi,histoPhoAll["ph_phi"])
    histoPhoAll["ph_pt"] = fill(bc_ph_pt,histoPhoAll["ph_pt"])
    histoPhoAll["ph_E"] = fill(bc_ph_E,histoPhoAll["ph_E"])
    histoPhoAll["ph_tight"] = fill(bc_ph_tight,histoPhoAll["ph_tight"])
    histoPhoAll["ph_tightIso"] = fill(bc_ph_tightIso,histoPhoAll["ph_tightIso"])
    histoPhoAll["ph_eta"] = fill(bc_ph_eta,histoPhoAll["ph_eta"])
    histoPhoAll["ph_author"] = fill(bc_ph_author,histoPhoAll["ph_author"])
    histoPhoAll["ph_loose"] = fill(bc_ph_loose,histoPhoAll["ph_loose"])
    histoPhoAll["ph_n"] = fill(bc_ph_n,histoPhoAll["ph_n"])
    histoPhoAll["ph_isConv"] = fill(bc_ph_isConv,histoPhoAll["ph_isConv"])
    histoPhoAll["ph_nConv"] = fill(bc_ph_nConv,histoPhoAll["ph_nConv"])
    
    ## Loose variables
    histoPhoAllLoose["ph_Es2"] = fill(bc_ph_Es2,histoPhoAllLoose["ph_Es2"])
    histoPhoAllLoose["ph_E233"] = fill(bc_ph_E233,histoPhoAllLoose["ph_E233"])
    histoPhoAllLoose["ph_E237"] = fill(bc_ph_E237,histoPhoAllLoose["ph_E237"])
    histoPhoAllLoose["ph_E277"] = fill(bc_ph_E277,histoPhoAllLoose["ph_E277"])
    histoPhoAllLoose["ph_weta2"] = fill(bc_ph_weta2,histoPhoAllLoose["ph_weta2"])
    histoPhoAllLoose["ph_Ethad"] = fill(bc_ph_Ethad,histoPhoAllLoose["ph_Ethad"])
    histoPhoAllLoose["ph_Ethad1"] = fill(bc_ph_Ethad1,histoPhoAllLoose["ph_Ethad1"])
    
    ## Tight variables
    histoPhoAllTight["ph_wstot"] = fill(bc_ph_wstot,histoPhoAllTight["ph_wstot"])
    histoPhoAllTight["ph_deltaEmax2"] = fill(bc_ph_deltaEmax2,histoPhoAllTight["ph_deltaEmax2"])
    histoPhoAllTight["ph_emaxs1"] = fill(bc_ph_emaxs1,histoPhoAllTight["ph_emaxs1"])
    histoPhoAllTight["ph_deltaEs"] = fill(bc_ph_deltaEs,histoPhoAllTight["ph_deltaEs"])
    histoPhoAllTight["ph_cl_E"] = fill(bc_ph_cl_E,histoPhoAllTight["ph_cl_E"])
    histoPhoAllTight["ph_f1"] = fill(bc_ph_f1,histoPhoAllTight["ph_f1"])
    
    ###########
    ## LOOSE ##
    histoPhoLoose["ph_phi_ifLoose"] = fillIsEM(bc_ph_phi,histoPhoLoose["ph_phi_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_pt_ifLoose"] = fillIsEM(bc_ph_pt,histoPhoLoose["ph_pt_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_E_ifLoose"] = fillIsEM(bc_ph_E,histoPhoLoose["ph_E_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_tight_ifLoose"] = fillIsEM(bc_ph_tight,histoPhoLoose["ph_tight_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_tightIso_ifLoose"] = fillIsEM(bc_ph_tightIso,histoPhoLoose["ph_tightIso_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_eta_ifLoose"] = fillIsEM(bc_ph_eta,histoPhoLoose["ph_eta_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_author_ifLoose"] = fillIsEM(bc_ph_author,histoPhoLoose["ph_author_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_loose_ifLoose"] = fillIsEM(bc_ph_loose,histoPhoLoose["ph_loose_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_n_ifLoose"] = fillIsEM_N(bc_ph_n,histoPhoLoose["ph_n_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_isConv_ifLoose"] = fillIsEM(bc_ph_isConv,histoPhoLoose["ph_isConv_ifLoose"],bc_ph_loose)
    histoPhoLoose["ph_nConv_ifLoose"] = fillIsEM_N(bc_ph_nConv,histoPhoLoose["ph_nConv_ifLoose"],bc_ph_loose)
    
    ## Loose variables
    histoPhoLooseLoose["ph_Es2_ifLoose"] = fillIsEM(bc_ph_Es2,histoPhoLooseLoose["ph_Es2_ifLoose"],bc_ph_loose)
    histoPhoLooseLoose["ph_E233_ifLoose"] = fillIsEM(bc_ph_E233,histoPhoLooseLoose["ph_E233_ifLoose"],bc_ph_loose)
    histoPhoLooseLoose["ph_E237_ifLoose"] = fillIsEM(bc_ph_E237,histoPhoLooseLoose["ph_E237_ifLoose"],bc_ph_loose)
    histoPhoLooseLoose["ph_E277_ifLoose"] = fillIsEM(bc_ph_E277,histoPhoLooseLoose["ph_E277_ifLoose"],bc_ph_loose)
    histoPhoLooseLoose["ph_weta2_ifLoose"] = fillIsEM(bc_ph_weta2,histoPhoLooseLoose["ph_weta2_ifLoose"],bc_ph_loose)
    histoPhoLooseLoose["ph_Ethad_ifLoose"] = fillIsEM(bc_ph_Ethad,histoPhoLooseLoose["ph_Ethad_ifLoose"],bc_ph_loose)
    histoPhoLooseLoose["ph_Ethad1_ifLoose"] = fillIsEM(bc_ph_Ethad1,histoPhoLooseLoose["ph_Ethad1_ifLoose"],bc_ph_loose)
    
    ## Tight variables
    histoPhoLooseTight["ph_wstot_ifLoose"] = fillIsEM(bc_ph_wstot,histoPhoLooseTight["ph_wstot_ifLoose"],bc_ph_loose)
    histoPhoLooseTight["ph_deltaEmax2_ifLoose"] = fillIsEM(bc_ph_deltaEmax2,histoPhoLooseTight["ph_deltaEmax2_ifLoose"],bc_ph_loose)
    histoPhoLooseTight["ph_emaxs1_ifLoose"] = fillIsEM(bc_ph_emaxs1,histoPhoLooseTight["ph_emaxs1_ifLoose"],bc_ph_loose)
    histoPhoLooseTight["ph_deltaEs_ifLoose"] = fillIsEM(bc_ph_deltaEs,histoPhoLooseTight["ph_deltaEs_ifLoose"],bc_ph_loose)
    histoPhoLooseTight["ph_cl_E_ifLoose"] = fillIsEM(bc_ph_cl_E,histoPhoLooseTight["ph_cl_E_ifLoose"],bc_ph_loose)
    histoPhoLooseTight["ph_f1_ifLoose"] = fillIsEM(bc_ph_f1,histoPhoLooseTight["ph_f1_ifLoose"],bc_ph_loose)
    
    ###########
    ## TIGHT ##
    histoPhoTight["ph_phi_ifTight"] = fillIsEM(bc_ph_phi,histoPhoTight["ph_phi_ifTight"],bc_ph_tight)
    histoPhoTight["ph_pt_ifTight"] = fillIsEM(bc_ph_pt,histoPhoTight["ph_pt_ifTight"],bc_ph_tight)
    histoPhoTight["ph_E_ifTight"] = fillIsEM(bc_ph_E,histoPhoTight["ph_E_ifTight"],bc_ph_tight)
    histoPhoTight["ph_tight_ifTight"] = fillIsEM(bc_ph_tight,histoPhoTight["ph_tight_ifTight"],bc_ph_tight)
    histoPhoTight["ph_tightIso_ifTight"] = fillIsEM(bc_ph_tightIso,histoPhoTight["ph_tightIso_ifTight"],bc_ph_tight)
    histoPhoTight["ph_eta_ifTight"] = fillIsEM(bc_ph_eta,histoPhoTight["ph_eta_ifTight"],bc_ph_tight)
    histoPhoTight["ph_author_ifTight"] = fillIsEM(bc_ph_author,histoPhoTight["ph_author_ifTight"],bc_ph_tight)
    histoPhoTight["ph_loose_ifTight"] = fillIsEM(bc_ph_loose,histoPhoTight["ph_loose_ifTight"],bc_ph_tight)
    histoPhoTight["ph_n_ifTight"] = fillIsEM_N(bc_ph_n,histoPhoTight["ph_n_ifTight"],bc_ph_tight)
    histoPhoTight["ph_isConv_ifTight"] = fillIsEM(bc_ph_isConv,histoPhoTight["ph_isConv_ifTight"],bc_ph_tight)
    histoPhoTight["ph_nConv_ifTight"] = fillIsEM_N(bc_ph_nConv,histoPhoTight["ph_nConv_ifTight"],bc_ph_tight)
    
    ## Loose variables
    histoPhoTightLoose["ph_Es2_ifTight"] = fillIsEM(bc_ph_Es2,histoPhoTightLoose["ph_Es2_ifTight"],bc_ph_tight)
    histoPhoTightLoose["ph_E233_ifTight"] = fillIsEM(bc_ph_E233,histoPhoTightLoose["ph_E233_ifTight"],bc_ph_tight)
    histoPhoTightLoose["ph_E237_ifTight"] = fillIsEM(bc_ph_E237,histoPhoTightLoose["ph_E237_ifTight"],bc_ph_tight)
    histoPhoTightLoose["ph_E277_ifTight"] = fillIsEM(bc_ph_E277,histoPhoTightLoose["ph_E277_ifTight"],bc_ph_tight)
    histoPhoTightLoose["ph_weta2_ifTight"] = fillIsEM(bc_ph_weta2,histoPhoTightLoose["ph_weta2_ifTight"],bc_ph_tight)
    histoPhoTightLoose["ph_Ethad_ifTight"] = fillIsEM(bc_ph_Ethad,histoPhoTightLoose["ph_Ethad_ifTight"],bc_ph_tight)
    histoPhoTightLoose["ph_Ethad1_ifTight"] = fillIsEM(bc_ph_Ethad1,histoPhoTightLoose["ph_Ethad1_ifTight"],bc_ph_tight)
    
    ## Tight variables
    histoPhoTightTight["ph_wstot_ifTight"] = fillIsEM(bc_ph_wstot,histoPhoTightTight["ph_wstot_ifTight"],bc_ph_tight)
    histoPhoTightTight["ph_deltaEmax2_ifTight"] = fillIsEM(bc_ph_deltaEmax2,histoPhoTightTight["ph_deltaEmax2_ifTight"],bc_ph_tight)
    histoPhoTightTight["ph_emaxs1_ifTight"] = fillIsEM(bc_ph_emaxs1,histoPhoTightTight["ph_emaxs1_ifTight"],bc_ph_tight)
    histoPhoTightTight["ph_deltaEs_ifTight"] = fillIsEM(bc_ph_deltaEs,histoPhoTightTight["ph_deltaEs_ifTight"],bc_ph_tight)
    histoPhoTightTight["ph_cl_E_ifTight"] = fillIsEM(bc_ph_cl_E,histoPhoTightTight["ph_cl_E_ifTight"],bc_ph_tight)
    histoPhoTightTight["ph_f1_ifTight"] = fillIsEM(bc_ph_f1,histoPhoTightTight["ph_f1_ifTight"],bc_ph_tight)
    
    ##############
    ## AUTHOR 4 ##
    histoPhoAuthor4["ph_phi_ifAuthor4"] = fillAuthor(bc_ph_phi,histoPhoAuthor4["ph_phi_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_pt_ifAuthor4"] = fillAuthor(bc_ph_pt,histoPhoAuthor4["ph_pt_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_E_ifAuthor4"] = fillAuthor(bc_ph_E,histoPhoAuthor4["ph_E_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_tight_ifAuthor4"] = fillAuthor(bc_ph_tight,histoPhoAuthor4["ph_tight_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_tightIso_ifAuthor4"] = fillAuthor(bc_ph_tightIso,histoPhoAuthor4["ph_tightIso_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_eta_ifAuthor4"] = fillAuthor(bc_ph_eta,histoPhoAuthor4["ph_eta_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_author_ifAuthor4"] = fillAuthor(bc_ph_author,histoPhoAuthor4["ph_author_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_loose_ifAuthor4"] = fillAuthor(bc_ph_loose,histoPhoAuthor4["ph_loose_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_n_ifAuthor4"] = fillAuthor_N(bc_ph_n,histoPhoAuthor4["ph_n_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_isConv_ifAuthor4"] = fillAuthor(bc_ph_isConv,histoPhoAuthor4["ph_isConv_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4["ph_nConv_ifAuthor4"] = fillAuthor_N(bc_ph_nConv,histoPhoAuthor4["ph_nConv_ifAuthor4"],bc_ph_author,4)
    
    ## Loose variables
    histoPhoAuthor4Loose["ph_Es2_ifAuthor4"] = fillAuthor(bc_ph_Es2,histoPhoAuthor4Loose["ph_Es2_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Loose["ph_E233_ifAuthor4"] = fillAuthor(bc_ph_E233,histoPhoAuthor4Loose["ph_E233_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Loose["ph_E237_ifAuthor4"] = fillAuthor(bc_ph_E237,histoPhoAuthor4Loose["ph_E237_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Loose["ph_E277_ifAuthor4"] = fillAuthor(bc_ph_E277,histoPhoAuthor4Loose["ph_E277_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Loose["ph_weta2_ifAuthor4"] = fillAuthor(bc_ph_weta2,histoPhoAuthor4Loose["ph_weta2_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Loose["ph_Ethad_ifAuthor4"] = fillAuthor(bc_ph_Ethad,histoPhoAuthor4Loose["ph_Ethad_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Loose["ph_Ethad1_ifAuthor4"] = fillAuthor(bc_ph_Ethad1,histoPhoAuthor4Loose["ph_Ethad1_ifAuthor4"],bc_ph_author,4)
    
    ## Tight variables
    histoPhoAuthor4Tight["ph_wstot_ifAuthor4"] = fillAuthor(bc_ph_wstot,histoPhoAuthor4Tight["ph_wstot_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Tight["ph_deltaEmax2_ifAuthor4"] = fillAuthor(bc_ph_deltaEmax2,histoPhoAuthor4Tight["ph_deltaEmax2_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Tight["ph_emaxs1_ifAuthor4"] = fillAuthor(bc_ph_emaxs1,histoPhoAuthor4Tight["ph_emaxs1_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Tight["ph_deltaEs_ifAuthor4"] = fillAuthor(bc_ph_deltaEs,histoPhoAuthor4Tight["ph_deltaEs_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Tight["ph_cl_E_ifAuthor4"] = fillAuthor(bc_ph_cl_E,histoPhoAuthor4Tight["ph_cl_E_ifAuthor4"],bc_ph_author,4)
    histoPhoAuthor4Tight["ph_f1_ifAuthor4"] = fillAuthor(bc_ph_f1,histoPhoAuthor4Tight["ph_f1_ifAuthor4"],bc_ph_author,4)
    
    ###############
    ## AUTHOR 16 ##
    histoPhoAuthor16["ph_phi_ifAuthor16"] = fillAuthor(bc_ph_phi,histoPhoAuthor16["ph_phi_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_pt_ifAuthor16"] = fillAuthor(bc_ph_pt,histoPhoAuthor16["ph_pt_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_E_ifAuthor16"] = fillAuthor(bc_ph_E,histoPhoAuthor16["ph_E_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_tight_ifAuthor16"] = fillAuthor(bc_ph_tight,histoPhoAuthor16["ph_tight_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_tightIso_ifAuthor16"] = fillAuthor(bc_ph_tightIso,histoPhoAuthor16["ph_tightIso_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_eta_ifAuthor16"] = fillAuthor(bc_ph_eta,histoPhoAuthor16["ph_eta_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_author_ifAuthor16"] = fillAuthor(bc_ph_author,histoPhoAuthor16["ph_author_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_loose_ifAuthor16"] = fillAuthor(bc_ph_loose,histoPhoAuthor16["ph_loose_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_n_ifAuthor16"] = fillAuthor_N(bc_ph_n,histoPhoAuthor16["ph_n_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_isConv_ifAuthor16"] = fillAuthor(bc_ph_isConv,histoPhoAuthor16["ph_isConv_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16["ph_nConv_ifAuthor16"] = fillAuthor_N(bc_ph_nConv,histoPhoAuthor16["ph_nConv_ifAuthor16"],bc_ph_author,16)
    
    ## Loose variables
    histoPhoAuthor16Loose["ph_Es2_ifAuthor16"] = fillAuthor(bc_ph_Es2,histoPhoAuthor16Loose["ph_Es2_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Loose["ph_E233_ifAuthor16"] = fillAuthor(bc_ph_E233,histoPhoAuthor16Loose["ph_E233_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Loose["ph_E237_ifAuthor16"] = fillAuthor(bc_ph_E237,histoPhoAuthor16Loose["ph_E237_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Loose["ph_E277_ifAuthor16"] = fillAuthor(bc_ph_E277,histoPhoAuthor16Loose["ph_E277_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Loose["ph_weta2_ifAuthor16"] = fillAuthor(bc_ph_weta2,histoPhoAuthor16Loose["ph_weta2_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Loose["ph_Ethad_ifAuthor16"] = fillAuthor(bc_ph_Ethad,histoPhoAuthor16Loose["ph_Ethad_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Loose["ph_Ethad1_ifAuthor16"] = fillAuthor(bc_ph_Ethad1,histoPhoAuthor16Loose["ph_Ethad1_ifAuthor16"],bc_ph_author,16)
    
    ## Tight variables
    histoPhoAuthor16Tight["ph_wstot_ifAuthor16"] = fillAuthor(bc_ph_wstot,histoPhoAuthor16Tight["ph_wstot_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Tight["ph_deltaEmax2_ifAuthor16"] = fillAuthor(bc_ph_deltaEmax2,histoPhoAuthor16Tight["ph_deltaEmax2_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Tight["ph_emaxs1_ifAuthor16"] = fillAuthor(bc_ph_emaxs1,histoPhoAuthor16Tight["ph_emaxs1_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Tight["ph_deltaEs_ifAuthor16"] = fillAuthor(bc_ph_deltaEs,histoPhoAuthor16Tight["ph_deltaEs_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Tight["ph_cl_E_ifAuthor16"] = fillAuthor(bc_ph_cl_E,histoPhoAuthor16Tight["ph_cl_E_ifAuthor16"],bc_ph_author,16)
    histoPhoAuthor16Tight["ph_f1_ifAuthor16"] = fillAuthor(bc_ph_f1,histoPhoAuthor16Tight["ph_f1_ifAuthor16"],bc_ph_author,16)
    
    #############
    ## IS CONV ##
    histoPhoIsConv["ph_phi_ifIsConv"] = fillIsEM(bc_ph_phi,histoPhoIsConv["ph_phi_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_pt_ifIsConv"] = fillIsEM(bc_ph_pt,histoPhoIsConv["ph_pt_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_E_ifIsConv"] = fillIsEM(bc_ph_E,histoPhoIsConv["ph_E_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_tight_ifIsConv"] = fillIsEM(bc_ph_tight,histoPhoIsConv["ph_tight_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_tightIso_ifIsConv"] = fillIsEM(bc_ph_tightIso,histoPhoIsConv["ph_tightIso_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_eta_ifIsConv"] = fillIsEM(bc_ph_eta,histoPhoIsConv["ph_eta_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_author_ifIsConv"] = fillIsEM(bc_ph_author,histoPhoIsConv["ph_author_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_loose_ifIsConv"] = fillIsEM(bc_ph_loose,histoPhoIsConv["ph_loose_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_n_ifIsConv"] = fillIsEM_N(bc_ph_n,histoPhoIsConv["ph_n_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_isConv_ifIsConv"] = fillIsEM(bc_ph_isConv,histoPhoIsConv["ph_isConv_ifIsConv"],bc_ph_isConv)
    histoPhoIsConv["ph_nConv_ifIsConv"] = fillIsEM_N(bc_ph_nConv,histoPhoIsConv["ph_nConv_ifIsConv"],bc_ph_isConv)
    
    ## Loose variables
    histoPhoIsConvLoose["ph_Es2_ifIsConv"] = fillIsEM(bc_ph_Es2,histoPhoIsConvLoose["ph_Es2_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvLoose["ph_E233_ifIsConv"] = fillIsEM(bc_ph_E233,histoPhoIsConvLoose["ph_E233_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvLoose["ph_E237_ifIsConv"] = fillIsEM(bc_ph_E237,histoPhoIsConvLoose["ph_E237_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvLoose["ph_E277_ifIsConv"] = fillIsEM(bc_ph_E277,histoPhoIsConvLoose["ph_E277_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvLoose["ph_weta2_ifIsConv"] = fillIsEM(bc_ph_weta2,histoPhoIsConvLoose["ph_weta2_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvLoose["ph_Ethad_ifIsConv"] = fillIsEM(bc_ph_Ethad,histoPhoIsConvLoose["ph_Ethad_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvLoose["ph_Ethad1_ifIsConv"] = fillIsEM(bc_ph_Ethad1,histoPhoIsConvLoose["ph_Ethad1_ifIsConv"],bc_ph_isConv)
    
    ## Tight variables
    histoPhoIsConvTight["ph_wstot_ifIsConv"] = fillIsEM(bc_ph_wstot,histoPhoIsConvTight["ph_wstot_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvTight["ph_deltaEmax2_ifIsConv"] = fillIsEM(bc_ph_deltaEmax2,histoPhoIsConvTight["ph_deltaEmax2_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvTight["ph_emaxs1_ifIsConv"] = fillIsEM(bc_ph_emaxs1,histoPhoIsConvTight["ph_emaxs1_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvTight["ph_deltaEs_ifIsConv"] = fillIsEM(bc_ph_deltaEs,histoPhoIsConvTight["ph_deltaEs_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvTight["ph_cl_E_ifIsConv"] = fillIsEM(bc_ph_cl_E,histoPhoIsConvTight["ph_cl_E_ifIsConv"],bc_ph_isConv)
    histoPhoIsConvTight["ph_f1_ifIsConv"] = fillIsEM(bc_ph_f1,histoPhoIsConvTight["ph_f1_ifIsConv"],bc_ph_isConv)
    
    #############
    ## UN CONV ##
    histoPhoUnConv["ph_phi_ifUnConv"] = fillNotIsEM(bc_ph_phi,histoPhoUnConv["ph_phi_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_pt_ifUnConv"] = fillNotIsEM(bc_ph_pt,histoPhoUnConv["ph_pt_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_E_ifUnConv"] = fillNotIsEM(bc_ph_E,histoPhoUnConv["ph_E_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_tight_ifUnConv"] = fillNotIsEM(bc_ph_tight,histoPhoUnConv["ph_tight_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_tightIso_ifUnConv"] = fillNotIsEM(bc_ph_tightIso,histoPhoUnConv["ph_tightIso_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_eta_ifUnConv"] = fillNotIsEM(bc_ph_eta,histoPhoUnConv["ph_eta_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_author_ifUnConv"] = fillNotIsEM(bc_ph_author,histoPhoUnConv["ph_author_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_loose_ifUnConv"] = fillNotIsEM(bc_ph_loose,histoPhoUnConv["ph_loose_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_n_ifUnConv"] = fillNotIsEM_N(bc_ph_n,histoPhoUnConv["ph_n_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_isConv_ifUnConv"] = fillNotIsEM(bc_ph_isConv,histoPhoUnConv["ph_isConv_ifUnConv"],bc_ph_isConv)
    histoPhoUnConv["ph_nConv_ifUnConv"] = fillNotIsEM_N(bc_ph_nConv,histoPhoUnConv["ph_nConv_ifUnConv"],bc_ph_isConv)
    
    ## Loose variables
    histoPhoUnConvLoose["ph_Es2_ifUnConv"] = fillNotIsEM(bc_ph_Es2,histoPhoUnConvLoose["ph_Es2_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvLoose["ph_E233_ifUnConv"] = fillNotIsEM(bc_ph_E233,histoPhoUnConvLoose["ph_E233_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvLoose["ph_E237_ifUnConv"] = fillNotIsEM(bc_ph_E237,histoPhoUnConvLoose["ph_E237_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvLoose["ph_E277_ifUnConv"] = fillNotIsEM(bc_ph_E277,histoPhoUnConvLoose["ph_E277_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvLoose["ph_weta2_ifUnConv"] = fillNotIsEM(bc_ph_weta2,histoPhoUnConvLoose["ph_weta2_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvLoose["ph_Ethad_ifUnConv"] = fillNotIsEM(bc_ph_Ethad,histoPhoUnConvLoose["ph_Ethad_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvLoose["ph_Ethad1_ifUnConv"] = fillNotIsEM(bc_ph_Ethad1,histoPhoUnConvLoose["ph_Ethad1_ifUnConv"],bc_ph_isConv)
    
    ## Tight variables
    histoPhoUnConvTight["ph_wstot_ifUnConv"] = fillNotIsEM(bc_ph_wstot,histoPhoUnConvTight["ph_wstot_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvTight["ph_deltaEmax2_ifUnConv"] = fillNotIsEM(bc_ph_deltaEmax2,histoPhoUnConvTight["ph_deltaEmax2_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvTight["ph_emaxs1_ifUnConv"] = fillNotIsEM(bc_ph_emaxs1,histoPhoUnConvTight["ph_emaxs1_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvTight["ph_deltaEs_ifUnConv"] = fillNotIsEM(bc_ph_deltaEs,histoPhoUnConvTight["ph_deltaEs_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvTight["ph_cl_E_ifUnConv"] = fillNotIsEM(bc_ph_cl_E,histoPhoUnConvTight["ph_cl_E_ifUnConv"],bc_ph_isConv)
    histoPhoUnConvTight["ph_f1_ifUnConv"] = fillNotIsEM(bc_ph_f1,histoPhoUnConvTight["ph_f1_ifUnConv"],bc_ph_isConv)
    
    return True

def branchExpert(tree) :
    return True

def branchExpEle(tree,histoIden,histoKin,histoConv,histoEM) :

    bc_el_origin = getattr(tree,'el_origin')
    bc_el_type = getattr(tree,'el_type')
    bc_el_isEM = getattr(tree,'el_isEM')
    bc_el_author = getattr(tree,'el_author')
    bc_el_loose = getattr(tree,'el_loose')
    bc_el_medium = getattr(tree,'el_medium')
    bc_el_tight = getattr(tree,'el_tight')
    bc_el_mediumIso = getattr(tree,'el_mediumIso')
    bc_el_tightIso = getattr(tree,'el_tightIso')
    # bc_el_tightNoIso = getattr(tree,'el_tightNoIso')
    # bc_el_tightTRTNoIso = getattr(tree,'el_tightTRTNoIso')
    # bc_el_mediumNoIso = getattr(tree,'el_mediumNoIso')
    
    bc_el_E = getattr(tree,'el_E')
    # bc_el_EF_E = getattr(tree,'el_EF_E')
    bc_el_Et = getattr(tree,'el_Et')
    # bc_el_EF_Et = getattr(tree,'el_EF_Et')
    bc_el_pt = getattr(tree,'el_pt')
    # bc_el_EF_pt = getattr(tree,'el_EF_pt')
    bc_el_m = getattr(tree,'el_m')
    bc_el_eta = getattr(tree,'el_eta')
    # bc_el_EF_eta = getattr(tree,'el_EF_eta')
    bc_el_phi = getattr(tree,'el_phi')
    # bc_el_EF_phi = getattr(tree,'el_EF_phi')
    bc_el_px = getattr(tree,'el_px')
    bc_el_py = getattr(tree,'el_py')
    bc_el_pz = getattr(tree,'el_pz')
    bc_el_charge = getattr(tree,'el_charge')
    # bc_el_EF_charge = getattr(tree,'el_EF_charge')
    
    bc_el_convFlag = getattr(tree,'el_convFlag')
    bc_el_isConv = getattr(tree,'el_isConv')
    
    bc_el_deltaEmax2 = getattr(tree,'el_deltaEmax2')
    bc_el_Ethad = getattr(tree,'el_Ethad')
    # bc_el_EF_Ethad = getattr(tree,'el_EF_Ethad')
    bc_el_Ethad1 = getattr(tree,'el_Ethad1')
    # bc_el_EF_Ethad1 = getattr(tree,'el_EF_Ethad1')
    bc_el_f1 = getattr(tree,'el_f1')
    # bc_el_EF_f1 = getattr(tree,'el_EF_f1')
    bc_el_f1core = getattr(tree,'el_f1core')
    bc_el_Emins1 = getattr(tree,'el_Emins1')
    # bc_el_EF_Emins1 = getattr(tree,'el_EF_Emins1')
    bc_el_emaxs1 = getattr(tree,'el_emaxs1')
    bc_el_Emax2 = getattr(tree,'el_Emax2')
    # bc_el_EF_Emax2 = getattr(tree,'el_EF_Emax2')
    bc_el_fside = getattr(tree,'el_fside')
    # bc_el_EF_fside = getattr(tree,'el_EF_fside')
    
    ## TDirectory Expert -> Electrons -> Identification
    histoIden["el_origin"] = fill(bc_el_origin,histoIden["el_origin"])
    histoIden["el_type"] = fill(bc_el_type,histoIden["el_type"])
    histoIden["el_isEM"] = fill(bc_el_isEM,histoIden["el_isEM"])
    histoIden["el_author"] = fill(bc_el_author,histoIden["el_author"])
    histoIden["el_loose"] = fill(bc_el_loose,histoIden["el_loose"])
    histoIden["el_medium"] = fill(bc_el_medium,histoIden["el_medium"])
    histoIden["el_tight"] = fill(bc_el_tight,histoIden["el_tight"])
    histoIden["el_mediumIso"] = fill(bc_el_mediumIso,histoIden["el_mediumIso"])
    histoIden["el_tightIso"] = fill(bc_el_tightIso,histoIden["el_tightIso"])
    # histoIden["el_tightNoIso"] = fill(bc_el_tightNoIso,histoIden["el_tightNoIso"])
    # histoIden["el_tightTRTNoIso"] = fill(bc_el_tightTRTNoIso,histoIden["el_tightTRTNoIso"])
    # histoIden["el_mediumNoIso"] = fill(bc_el_mediumNoIso,histoIden["el_mediumNoIso"])
    
    ## TDirectory Expert -> Electrons -> Kinematics
    histoKin["el_E"] = fill(bc_el_E,histoKin["el_E"])
    # histoKin["el_EF_E"] = fill(bc_el_EF_E,histoKin["el_EF_E"])
    histoKin["el_Et"] = fill(bc_el_Et,histoKin["el_Et"])
    # histoKin["el_EF_Et"] = fill(bc_el_EF_Et,histoKin["el_EF_Et"])
    histoKin["el_pt"] = fill(bc_el_pt,histoKin["el_pt"])
    # histoKin["el_EF_pt"] = fill(bc_el_EF_pt,histoKin["el_EF_pt"])
    histoKin["el_m"] = fill(bc_el_m,histoKin["el_m"])
    histoKin["el_eta"] = fill(bc_el_eta,histoKin["el_eta"])
    # histoKin["el_EF_eta"] = fill(bc_el_EF_eta,histoKin["el_EF_eta"])
    histoKin["el_phi"] = fill(bc_el_phi,histoKin["el_phi"])
    # histoKin["el_EF_phi"] = fill(bc_el_EF_phi,histoKin["el_EF_phi"])
    histoKin["el_px"] = fill(bc_el_px,histoKin["el_px"])
    histoKin["el_py"] = fill(bc_el_py,histoKin["el_py"])
    histoKin["el_pz"] = fill(bc_el_pz,histoKin["el_pz"])
    histoKin["el_charge"] = fill(bc_el_charge,histoKin["el_charge"])
    # histoKin["el_EF_charge"] = fill(bc_el_EF_charge,histoKin["el_EF_charge"])
    histoKin["el_author"] = fill(bc_el_author,histoKin["el_author"])
    
    
    ## TDirectory Expert -> Electrons -> Conversions
    histoConv["el_convFlag"] = fill(bc_el_convFlag,histoConv["el_convFlag"])
    histoConv["el_isConv"] = fill(bc_el_isConv,histoConv["el_isConv"])

    ## TDirectory Expert -> Electrons -> EM
    histoEM["el_deltaEmax2"] = fill(bc_el_deltaEmax2,histoEM["el_deltaEmax2"])
    histoEM["el_Ethad"] = fill(bc_el_Ethad,histoEM["el_Ethad"])
    # histoEM["el_EF_Ethad"] = fill(bc_el_EF_Ethad,histoEM["el_EF_Ethad"])
    histoEM["el_Ethad1"] = fill(bc_el_Ethad1,histoEM["el_Ethad1"])
    # histoEM["el_EF_Ethad1"] = fill(bc_el_EF_Ethad1,histoEM["el_EF_Ethad1"])
    histoEM["el_f1"] = fill(bc_el_f1,histoEM["el_f1"])
    # histoEM["el_EF_f1"] = fill(bc_el_EF_f1,histoEM["el_EF_f1"])
    histoEM["el_f1core"] = fill(bc_el_f1core,histoEM["el_f1core"])
    histoEM["el_Emins1"] = fill(bc_el_Emins1,histoEM["el_Emins1"])
    # histoEM["el_EF_Emins1"] = fill(bc_el_EF_Emins1,histoEM["el_EF_Emins1"])
    histoEM["el_emaxs1"] = fill(bc_el_emaxs1,histoEM["el_emaxs1"])
    histoEM["el_Emax2"] = fill(bc_el_Emax2,histoEM["el_Emax2"])
    # histoEM["el_EF_Emax2"] = fill(bc_el_EF_Emax2,histoEM["el_EF_Emax2"])
    histoEM["el_fside"] = fill(bc_el_fside,histoEM["el_fside"])
    # histoEM["el_EF_fside"] = fill(bc_el_EF_fside,histoEM["el_EF_fside"])



        
#####################################
## Print the content of the branch ##
#####################################

def printBr(branch, i):
    for width in branch :
        print i, width
        pass
    return True

########################
## Filling histograms ##
########################

def fill(branch,plot) : ## Fill the histogramms
    u_h1 = plot
    try :
        for u in branch :
            u_h1.Fill(u)
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            u_h1.Fill(branch)
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    return u_h1

def fillIsEM(branch,plot,selec) : ## Fill the histogramms in case isEM selection (loose,medium,tight)
    u_h1 = plot
    try :
        for u in range(len(branch)) :
            if selec[u] == 1 : u_h1.Fill(branch[u])
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            if select == True : u_h1.Fill(branch)
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    return u_h1

def fillIsEM_N(branch,plot,selec) : ## Fill the histogramms el_n in case isEM selection (loose,medium,tight)
    u_h1 = plot
    i = 0
    try :
        for u in range(len(selec)) :
            if selec[u] == 1 : i += 1
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            if select == 1 : i += 1
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    u_h1.Fill(i)
    return u_h1

    
def fillNotIsEM(branch,plot,selec) : ## Fill the histogramms in case not isEM selection (loose,medium,tight)
    u_h1 = plot
    try :
        for u in range(len(branch)) :
            if selec[u] == 0 : u_h1.Fill(branch[u])
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            if select == False : u_h1.Fill(branch)
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    return u_h1

def fillNotIsEM_N(branch,plot,selec) : ## Fill the histogramms el_n in case not isEM selection (loose,medium,tight)
    u_h1 = plot
    i = 0
    try :
        for u in range(len(selec)) :
            if selec[u] == 0 : i += 1
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            if select == 0 : i += 1
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    u_h1.Fill(i)
    return u_h1

    
def fillAuthor(branch,plot,author,n=0) : ## Fill the histogramms in case of author = n
    u_h1 = plot
    try :
        for u in range(len(branch)) :
            if author[u] == n :u_h1.Fill(branch[u])
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            if author == n : u_h1.Fill(branch)
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    return u_h1
    
def fillAuthor_N(branch,plot,author,n=0) : ## Fill the histogramms el_n in case of author = n
    u_h1 = plot
    i = 0
    try :
        for u in range(len(author)) :
            if author[u] == n : i += 1
            pass
        pass
    except :
        # print "pb with filling histo", branch
        try :
            if author == n : i += 1
            pass
        except :
            print "impossible to fill this histo", u_h1
            pass
        pass
    u_h1.Fill(i)
    return u_h1
    

#####################
## Save histograms ##
#####################

def WriteEle(file,histoEleAll,histoEleAllLoose,histoEleAllMedium,histoEleAllTight,histoEleLoose,histoEleLooseLoose,histoEleLooseMedium,histoEleLooseTight,histoEleMedium,histoEleMediumLoose,histoEleMediumMedium,histoEleMediumTight,histoEleTight,histoEleTightLoose,histoEleTightMedium,histoEleTightTight,histoEleAuthor1,histoEleAuthor1Loose,histoEleAuthor1Medium,histoEleAuthor1Tight,histoEleAuthor2,histoEleAuthor2Loose,histoEleAuthor2Medium,histoEleAuthor2Tight,histoEleAuthor3,histoEleAuthor3Loose,histoEleAuthor3Medium,histoEleAuthor3Tight,histoEleAuthor8,histoEleAuthor8Loose,histoEleAuthor8Medium,histoEleAuthor8Tight) :

    ele = file.mkdir("ELECTRONS")
    
    all = ele.mkdir("AllElectrons")
    all_L = all.mkdir("LooseVariables")
    all_M = all.mkdir("MediumVariables")
    all_T = all.mkdir("TightVariables")
    all.cd()
    for i in histoEleAll : histoEleAll[i].Write()
    all_L.cd()
    for i in histoEleAllLoose : histoEleAllLoose[i].Write()
    all_M.cd()
    for i in histoEleAllMedium : histoEleAllMedium[i].Write()
    all_T.cd()
    for i in histoEleAllTight : histoEleAllTight[i].Write()
    
    loose = ele.mkdir("LooseElectrons")
    loose_L = loose.mkdir("LooseVariables")
    loose_M = loose.mkdir("MediumVariables")
    loose_T = loose.mkdir("TightVariables")
    loose.cd()
    for i in histoEleLoose : histoEleLoose[i].Write()
    loose_L.cd()
    for i in histoEleLooseLoose : histoEleLooseLoose[i].Write()
    loose_M.cd()
    for i in histoEleLooseMedium : histoEleLooseMedium[i].Write()
    loose_T.cd()
    for i in histoEleLooseTight : histoEleLooseTight[i].Write()
    
    medium = ele.mkdir("MediumElectrons")
    medium_L = medium.mkdir("LooseVariables")
    medium_M = medium.mkdir("MediumVariables")
    medium_T = medium.mkdir("TightVariables")
    medium.cd()
    for i in histoEleMedium : histoEleMedium[i].Write()
    medium_L.cd()
    for i in histoEleMediumLoose : histoEleMediumLoose[i].Write()
    medium_M.cd()
    for i in histoEleMediumMedium : histoEleMediumMedium[i].Write()
    medium_T.cd()
    for i in histoEleMediumTight : histoEleMediumTight[i].Write()
    
    tight = ele.mkdir("TightElectrons")
    tight_L = tight.mkdir("LooseVariables")
    tight_M = tight.mkdir("MediumVariables")
    tight_T = tight.mkdir("TightVariables")
    tight.cd()
    for i in histoEleTight : histoEleTight[i].Write()
    tight_L.cd()
    for i in histoEleTightLoose : histoEleTightLoose[i].Write()
    tight_M.cd()
    for i in histoEleTightMedium : histoEleTightMedium[i].Write()
    tight_T.cd()
    for i in histoEleTightTight : histoEleTightTight[i].Write()
    
    author1 = ele.mkdir("Author1Electrons")
    author1_L = author1.mkdir("LooseVariables")
    author1_M = author1.mkdir("MediumVariables")
    author1_T = author1.mkdir("TightVariables")
    author1.cd()
    for i in histoEleAuthor1 : histoEleAuthor1[i].Write()
    author1_L.cd()
    for i in histoEleAuthor1Loose : histoEleAuthor1Loose[i].Write()
    author1_M.cd()
    for i in histoEleAuthor1Medium : histoEleAuthor1Medium[i].Write()
    author1_T.cd()
    for i in histoEleAuthor1Tight : histoEleAuthor1Tight[i].Write()

    author2 = ele.mkdir("Author2Electrons")
    author2_L = author2.mkdir("LooseVariables")
    author2_M = author2.mkdir("MediumVariables")
    author2_T = author2.mkdir("TightVariables")
    author2.cd()
    for i in histoEleAuthor2 : histoEleAuthor2[i].Write()
    author2_L.cd()
    for i in histoEleAuthor2Loose : histoEleAuthor2Loose[i].Write()
    author2_M.cd()
    for i in histoEleAuthor2Medium : histoEleAuthor2Medium[i].Write()
    author2_T.cd()
    for i in histoEleAuthor2Tight : histoEleAuthor2Tight[i].Write()

    author3 = ele.mkdir("Author3Electrons")
    author3_L = author3.mkdir("LooseVariables")
    author3_M = author3.mkdir("MediumVariables")
    author3_T = author3.mkdir("TightVariables")
    author3.cd()
    for i in histoEleAuthor3 : histoEleAuthor3[i].Write()
    author3_L.cd()
    for i in histoEleAuthor3Loose : histoEleAuthor3Loose[i].Write()
    author3_M.cd()
    for i in histoEleAuthor3Medium : histoEleAuthor3Medium[i].Write()
    author3_T.cd()
    for i in histoEleAuthor3Tight : histoEleAuthor3Tight[i].Write()

    author8 = ele.mkdir("Author8Electrons")
    author8_L = author8.mkdir("LooseVariables")
    author8_M = author8.mkdir("MediumVariables")
    author8_T = author8.mkdir("TightVariables")
    author8.cd()
    for i in histoEleAuthor8 : histoEleAuthor8[i].Write()
    author8_L.cd()
    for i in histoEleAuthor8Loose : histoEleAuthor8Loose[i].Write()
    author8_M.cd()
    for i in histoEleAuthor8Medium : histoEleAuthor8Medium[i].Write()
    author8_T.cd()
    for i in histoEleAuthor8Tight : histoEleAuthor8Tight[i].Write()

    return True

def WritePho(file,histoPhoAll,histoPhoAllLoose,histoPhoAllTight,histoPhoLoose,histoPhoLooseLoose,histoPhoLooseTight,histoPhoTight,histoPhoTightLoose,histoPhoTightTight,histoPhoAuthor4,histoPhoAuthor4Loose,histoPhoAuthor4Tight,histoPhoAuthor16,histoPhoAuthor16Loose,histoPhoAuthor16Tight,histoPhoIsConv,histoPhoIsConvLoose,histoPhoIsConvTight,histoPhoUnConv,histoPhoUnConvLoose,histoPhoUnConvTight) :

    pho = file.mkdir("PHOTONS")

    all = pho.mkdir("AllPhotons")
    all_L = all.mkdir("LooseVariables")
    all_T = all.mkdir("TightVariables")
    all.cd()
    for i in histoPhoAll : histoPhoAll[i].Write()
    all_L.cd()
    for i in histoPhoAllLoose : histoPhoAllLoose[i].Write()
    all_T.cd()
    for i in histoPhoAllTight : histoPhoAllTight[i].Write()

    loose = pho.mkdir("LoosePhotons")
    loose_L = loose.mkdir("LooseVariables")
    loose_T = loose.mkdir("TightVariables")
    loose.cd()
    for i in histoPhoLoose : histoPhoLoose[i].Write()
    loose_L.cd()
    for i in histoPhoLooseLoose : histoPhoLooseLoose[i].Write()
    loose_T.cd()
    for i in histoPhoLooseTight : histoPhoLooseTight[i].Write()

    tight = pho.mkdir("TightPhotons")
    tight_L = tight.mkdir("LooseVariables")
    tight_T = tight.mkdir("TightVariables")
    tight.cd()
    for i in histoPhoTight : histoPhoTight[i].Write()
    tight_L.cd()
    for i in histoPhoTightLoose : histoPhoTightLoose[i].Write()
    tight_T.cd()
    for i in histoPhoTightTight : histoPhoTightTight[i].Write()
    
    author4 = pho.mkdir("Author4Photons")
    author4_L = author4.mkdir("LooseVariables")
    author4_T = author4.mkdir("TightVariables")
    author4.cd()
    for i in histoPhoAuthor4 : histoPhoAuthor4[i].Write()
    author4_L.cd()
    for i in histoPhoAuthor4Loose : histoPhoAuthor4Loose[i].Write()
    author4_T.cd()
    for i in histoPhoAuthor4Tight : histoPhoAuthor4Tight[i].Write()
    
    author16 = pho.mkdir("Author16Photons")
    author16_L = author16.mkdir("LooseVariables")
    author16_T = author16.mkdir("TightVariables")
    author16.cd()
    for i in histoPhoAuthor16 : histoPhoAuthor16[i].Write()
    author16_L.cd()
    for i in histoPhoAuthor16Loose : histoPhoAuthor16Loose[i].Write()
    author16_T.cd()
    for i in histoPhoAuthor16Tight : histoPhoAuthor16Tight[i].Write()
    
    IsConv = pho.mkdir("IsConvPhotons")
    IsConv_L = IsConv.mkdir("LooseVariables")
    IsConv_T = IsConv.mkdir("TightVariables")
    IsConv.cd()
    for i in histoPhoIsConv : histoPhoIsConv[i].Write()
    IsConv_L.cd()
    for i in histoPhoIsConvLoose : histoPhoIsConvLoose[i].Write()
    IsConv_T.cd()
    for i in histoPhoIsConvTight : histoPhoIsConvTight[i].Write()
    
    UnConv = pho.mkdir("UnConvPhotons")
    UnConv_L = UnConv.mkdir("LooseVariables")
    UnConv_T = UnConv.mkdir("TightVariables")
    UnConv.cd()
    for i in histoPhoUnConv : histoPhoUnConv[i].Write()
    UnConv_L.cd()
    for i in histoPhoUnConvLoose : histoPhoUnConvLoose[i].Write()
    UnConv_T.cd()
    for i in histoPhoUnConvTight : histoPhoUnConvTight[i].Write()
    
    return True
    
def WriteExpert(file) :
    Exp = file.mkdir("Expert")
    return Exp

def WriteExpEle(file, Exp, histoIden, histoKin, histoConv, histoEM) :
    
    ele = Exp.mkdir("Electrons")

    iden   = ele.mkdir("Identification")
    kin    = ele.mkdir("Kinematics")
    conv   = ele.mkdir("Conversion")
    em     = ele.mkdir("EMVariables")
    iso    = ele.mkdir("Isolation")
    dis    = ele.mkdir("DiscriminantsVariables")
    point  = ele.mkdir("Pointing")
    clK    = ele.mkdir("ClusterKinematics")
    l1     = ele.mkdir("L1Variables")
    l2     = ele.mkdir("L2Variables")
    ef     = ele.mkdir("EFVariables")
    jet    = ele.mkdir("Jet")
    trk    = ele.mkdir("Track")
    raw    = ele.mkdir("RawCl")
    vtx    = ele.mkdir("Vertex")
    bre    = ele.mkdir("Brem")
    mc     = ele.mkdir("MC")
    trkm   = ele.mkdir("TrackMatch")
    emTrk  = ele.mkdir("EMTrackFit")
    forw   = ele.mkdir("ForwardVariables")

    iden.cd()
    for i in histoIden : histoIden[i].Write()
    kin.cd()
    for i in histoKin : histoKin[i].Write()
    conv.cd()
    for i in histoConv : histoConv[i].Write()
    em.cd()
    for i in histoEM : histoEM[i].Write()
    
    return True

def WriteExpPho(file, Exp) :
    
    pho = Exp.mkdir("Photons")

    iden  = pho.mkdir("Identification")
    kin   = pho.mkdir("Kinematics")
    conv  = pho.mkdir("Conversion")
    em    = pho.mkdir("EMVariables")
    iso   = pho.mkdir("Isolation")
    dis   = pho.mkdir("DiscriminantsVariables")
    point = pho.mkdir("Pointing")
    clK   = pho.mkdir("ClusterKinematics")
    l1    = pho.mkdir("L1Variables")
    l2    = pho.mkdir("L2Variables")
    ef    = pho.mkdir("EFVariables")
    jet   = pho.mkdir("Jet")
    trk   = pho.mkdir("TrackVariables")
    raw   = pho.mkdir("RawCl")
    topo  = pho.mkdir("Topo")
    mc    = pho.mkdir("MC")
    
    return True

def WriteExpMu(file, Exp) :
    
    mu = Exp.mkdir("Muons")

    kin     = mu.mkdir("Kinematics")
    alg     = mu.mkdir("Algorithms")
    eneCalo = mu.mkdir("EnergyLossCalo")
    hits    = mu.mkdir("MuonSpectrometerHitsSummary")
    iso     = mu.mkdir("Isolation")
    trkPa   = mu.mkdir("TrackPattern")
    trk     = mu.mkdir("TrackParameters")
    trkQ    = mu.mkdir("TrackQuality")
    l1      = mu.mkdir("L1Variables")
    l2      = mu.mkdir("L2Variables")
    ef      = mu.mkdir("EFVariables")
    mc      = mu.mkdir("MC")
    
    return True


def WriteExpOther(file, Exp) :
    
    oth = Exp.mkdir("Others")

    mc = oth.mkdir("McEvent")
    glob = oth.mkdir("GlobalEvent")
    jet = oth.mkdir("JetContent")
    met = oth.mkdir("METContent")
    mb = oth.mkdir("MinimumBias")
    trig = oth.mkdir("TriggerBit")

    return True

