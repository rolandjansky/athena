/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIERDEFS_H
#define MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIERDEFS_H
/********************************************************************

NAME:     MCTruthClassifierDefs.h
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier

AUTHORS:  O. Fedin
CREATED:  Oct 2007

PURPOSE:

Updated:

For more information, please see the twiki page:
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCTruthClassifier
********************************************************************/
#include <vector>
#include <string>
//
#define PARTICLETYPES   40
#define PARTICLEORIGIN  48
#define PARTICLEOUTCOME 12

namespace MCTruthPartClassifier {

  enum ParticleType : unsigned int {
    Unknown           =  0,
    UnknownElectron   =  1,
    IsoElectron       =  2,
    NonIsoElectron    =  3,
    BkgElectron       =  4,
    UnknownMuon       =  5,
    IsoMuon           =  6,
    NonIsoMuon        =  7,
    BkgMuon           =  8,
    UnknownTau        =  9,
    IsoTau            =  10,
    NonIsoTau         =  11,
    BkgTau            =  12,
    UnknownPhoton     =  13,
    IsoPhoton         =  14,
    NonIsoPhoton      =  15,
    BkgPhoton         =  16,
    Hadron            =  17,
    Neutrino          =  18,
    NuclFrag          =  19,
    NonPrimary        =  20,
    GenParticle       =  21,
    SUSYParticle      =  22,
    OtherBSMParticle  =  39,
    BBbarMesonPart    =  23,
    BottomMesonPart   =  24,
    CCbarMesonPart    =  25,
    CharmedMesonPart  =  26,
    BottomBaryonPart  =  27,
    CharmedBaryonPart =  28,
    StrangeBaryonPart =  29,
    LightBaryonPart   =  30,
    StrangeMesonPart  =  31,
    LightMesonPart    =  32,
    BJet              =  33,
    CJet              =  34,
    LJet              =  35,
    GJet              =  36,
    TauJet            =  37,
    UnknownJet        =  38
  };

  enum ParticleOrigin : unsigned int {
    NonDefined    = 0,

    SingleElec    = 1,
    SingleMuon    = 2,
    SinglePhot    = 3,
    SingleTau     = 4,

    PhotonConv    = 5,
    DalitzDec     = 6,
    ElMagProc     = 7,
    Mu            = 8,
    TauLep        = 9,
    top           = 10,
    QuarkWeakDec  = 11,
    WBoson        = 12,
    ZBoson        = 13,
    Higgs         = 14,
    HiggsMSSM     = 15,
    HeavyBoson    = 16,
    WBosonLRSM    = 17,
    NuREle        = 18,
    NuRMu         = 19,
    NuRTau        = 20,
    LQ            = 21,

    SUSY          = 22,
    OtherBSM      = 46,
    LightMeson    = 23,
    StrangeMeson  = 24,
    CharmedMeson  = 25,
    BottomMeson   = 26,
    CCbarMeson    = 27,
    JPsi          = 28,
    BBbarMeson    = 29,
    LightBaryon   = 30,
    StrangeBaryon = 31,
    CharmedBaryon = 32,
    BottomBaryon  = 33,
    PionDecay     = 34,
    KaonDecay     = 35,
    BremPhot      = 36,
    PromptPhot    = 37,
    UndrPhot      = 38,
    ISRPhot       = 39,
    FSRPhot       = 40,
    NucReact      = 41,
    PiZero        = 42,
    DiBoson       = 43,
    ZorHeavyBoson = 44,
    MultiBoson    = 47,
    QCD           = 45
  };

  enum ParticleOutCome : unsigned int {
    NonDefinedOutCome  =  0,
    UnknownOutCome     =  1,
    UnConverted        =  2,
    Converted          =  3,
    NonInteract        =  4,
    NuclInteraction    =  5,
    ElectrMagInter     =  6,
    DecaytoElectron    =  7,
    DecaytoMuon        =  8,
    OneProng           =  9,
    ThreeProng         = 10,
    FiveProng          = 11
  };



  struct ParticleDef {

    /** the vector of particle types */
    std::vector<std::string> sParticleType;
    /** the vector of particle origin */
    std::vector<std::string> sParticleOrigin;
    /** the vector of particle out come */
    std::vector<std::string> sParticleOutCome;

    /**Default constructor*/
    ParticleDef()
    {
      sParticleType.reserve(PARTICLETYPES);
      sParticleType.emplace_back("Unknown");
      sParticleType.emplace_back("UnknownElectron");
      sParticleType.emplace_back("IsoElectron");
      sParticleType.emplace_back("NonIsoElectron");
      sParticleType.emplace_back("BkgElectron");
      sParticleType.emplace_back("UnknownMuon");
      sParticleType.emplace_back("IsoMuon");
      sParticleType.emplace_back("NonIsoMuon");
      sParticleType.emplace_back("BkgMuon");
      sParticleType.emplace_back("UnknownTau");
      sParticleType.emplace_back("IsoTau");
      sParticleType.emplace_back("NonIsoTau");
      sParticleType.emplace_back("BkgTau");
      sParticleType.emplace_back("UnknownPhoton");
      sParticleType.emplace_back("IsoPhoton");
      sParticleType.emplace_back("NonIsoPhoton");
      sParticleType.emplace_back("BkgPhoton");
      sParticleType.emplace_back("Hadron");
      sParticleType.emplace_back("Neutrino");
      sParticleType.emplace_back("NuclFrag");
      sParticleType.emplace_back("NonPrimary");
      sParticleType.emplace_back("GenParticle");
      sParticleType.emplace_back("SUSYParticle");
      sParticleType.emplace_back("BBbarMesonPart");
      sParticleType.emplace_back("BottomMesonPart");
      sParticleType.emplace_back("CCbarMesonPart");
      sParticleType.emplace_back("CharmedMesonPart");
      sParticleType.emplace_back("BottomBaryonPart");
      sParticleType.emplace_back("CharmedBaryonPart");
      sParticleType.emplace_back("StrangeBaryonPart");
      sParticleType.emplace_back("LightBaryonPart");
      sParticleType.emplace_back("StrangeMesonPart");
      sParticleType.emplace_back("LightMesonPart");
      sParticleType.emplace_back("BJet");
      sParticleType.emplace_back("CJet");
      sParticleType.emplace_back("LJet");
      sParticleType.emplace_back("GJet");
      sParticleType.emplace_back("TauJet");
      sParticleType.emplace_back("UnknownJet");
      sParticleType.emplace_back("OtherBSMParticle");

      sParticleOrigin.reserve(PARTICLEORIGIN);
      sParticleOrigin.emplace_back("NonDefined");
      sParticleOrigin.emplace_back("SingleElec");
      sParticleOrigin.emplace_back("SingleMuon");
      sParticleOrigin.emplace_back("SinglePhot");
      sParticleOrigin.emplace_back("SingleTau");
      sParticleOrigin.emplace_back("PhotonConv");
      sParticleOrigin.emplace_back("DalitzDec");
      sParticleOrigin.emplace_back("ElMagProc");
      sParticleOrigin.emplace_back("Mu");
      sParticleOrigin.emplace_back("TauLep");
      sParticleOrigin.emplace_back("top");
      sParticleOrigin.emplace_back("QuarkWeakDec");
      sParticleOrigin.emplace_back("WBoson");
      sParticleOrigin.emplace_back("ZBoson");
      sParticleOrigin.emplace_back("Higgs");
      sParticleOrigin.emplace_back("HiggsMSSM");
      sParticleOrigin.emplace_back("WZMSSM");
      sParticleOrigin.emplace_back("WBosonLRSM");
      sParticleOrigin.emplace_back("NuREle");
      sParticleOrigin.emplace_back("NuRMu ");
      sParticleOrigin.emplace_back("NuRTau");
      sParticleOrigin.emplace_back("LQ");
      sParticleOrigin.emplace_back("SUSY");
      sParticleOrigin.emplace_back("LightMeson");
      sParticleOrigin.emplace_back("StrangeMeson");
      sParticleOrigin.emplace_back("CharmedMeson");
      sParticleOrigin.emplace_back("BottomMeson");
      sParticleOrigin.emplace_back("CCbarMeson");
      sParticleOrigin.emplace_back("JPsi");
      sParticleOrigin.emplace_back("BBbarMeson");
      sParticleOrigin.emplace_back("LightBaryon");
      sParticleOrigin.emplace_back("StrangeBaryon");
      sParticleOrigin.emplace_back("CharmedBaryon");
      sParticleOrigin.emplace_back("BottomBaryon");
      sParticleOrigin.emplace_back("PionDecay");
      sParticleOrigin.emplace_back("KaonDecay");
      sParticleOrigin.emplace_back("BremPhot");
      sParticleOrigin.emplace_back("PromptPhot");
      sParticleOrigin.emplace_back("UndrPhot");
      sParticleOrigin.emplace_back("ISRPhot");
      sParticleOrigin.emplace_back("FSRPhot");
      sParticleOrigin.emplace_back("NucReact");
      sParticleOrigin.emplace_back("PiZero");
      sParticleOrigin.emplace_back("DiBoson");
      sParticleOrigin.emplace_back("ZorHeavyBoson");
      sParticleOrigin.emplace_back("QCD");
      sParticleOrigin.emplace_back("OtherBSM");
      sParticleOrigin.emplace_back("MultiBoson");

      sParticleOutCome.reserve(PARTICLEOUTCOME);
      sParticleOutCome.emplace_back("NonDefinedOutCome");
      sParticleOutCome.emplace_back("UnknownOutCome");
      sParticleOutCome.emplace_back("UnConverted");
      sParticleOutCome.emplace_back("Converted");
      sParticleOutCome.emplace_back("NonInteract");
      sParticleOutCome.emplace_back("NuclInteraction");
      sParticleOutCome.emplace_back("ElectrMagInter");
      sParticleOutCome.emplace_back("DecaytoElectron");
      sParticleOutCome.emplace_back("DecaytoMuon");
      sParticleOutCome.emplace_back("OneProng");
      sParticleOutCome.emplace_back("ThreeProng");
      sParticleOutCome.emplace_back("FiveProng");

    }

  };

  // Ensure that the enums are available from ROOT
  struct ROOT6_NamespaceAutoloadHook{};
}

#endif // MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIERDEFS_H
