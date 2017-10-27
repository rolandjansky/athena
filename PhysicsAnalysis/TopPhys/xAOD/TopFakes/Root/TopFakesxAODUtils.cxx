/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// 
// Author   : 
//              Timothée Theveneaux-Pelzer
// Creation : 
// Purpose  : helper to be used in analyses
//
// Updated  : 
//           2016-02-12 : fixes for muon trigger splitting
//
//////////////////////////////////////////////////////////////////////
#include "TopFakes/TopFakesxAODUtils.h"

#include "TopEvent/EventTools.h"
#include "TopFakes/FakesWeights.h"

void top::fakes::GetClosestJet (const xAOD::Electron& el, 
				const xAOD::JetContainer& jets,
				float& dRClosetJet, 
				float& PtClosetJet, 
				float& dROverPtClosetJet,
				float& dRClosetCentralJet, 
				float& PtClosetCentralJet, 
				float& dROverPtClosetCentralJet)
{
  //the dRs are initialised to a very high positive value
  dRClosetJet = 999.;
  dRClosetCentralJet = 999.;
  //the pTs are initialised to a negative value
  PtClosetJet = -999.;
  PtClosetCentralJet = -999.;
  // these variables are not used in fact! 
  dROverPtClosetJet = -999.;
  dROverPtClosetCentralJet = -999.;

  for (const auto* const jetPtr : jets) {
    if (jetPtr->pt()<25000.) continue;
    double newdR = top::deltaR(*jetPtr,el);
    if (jetPtr->eta()<2.5) {
      if (newdR<dRClosetCentralJet) {
	dRClosetCentralJet = newdR;
	PtClosetCentralJet = jetPtr->pt()/1000.; // tool needs GeV
      }
    }
    if (newdR<dRClosetJet) {
      dRClosetJet = newdR;
      PtClosetJet = jetPtr->pt()/1000.; // tool needs GeV
    }
  }

  // these variables are not used !
  if ((dRClosetJet!=999.) && (PtClosetJet!=-999.)) dROverPtClosetJet = dRClosetJet/PtClosetJet;
  if ((dRClosetCentralJet!=999.) && (PtClosetCentralJet!=-999.)) dROverPtClosetCentralJet = dRClosetCentralJet/PtClosetCentralJet;
 
  //if the dRs are at their default value, give them a negative value
  if (dRClosetJet == 999.) dRClosetJet = -999.;
  if (dRClosetCentralJet == 999.) dRClosetCentralJet = -999.;
}

// ============================================================
void top::fakes::GetClosestJet (const xAOD::Muon& mu, 
				const xAOD::JetContainer& jets,
				float& dRClosetJet, 
				float& PtClosetJet, 
				float& dROverPtClosetJet,
				float& dRClosetCentralJet, 
				float& PtClosetCentralJet, 
				float& dROverPtClosetCentralJet)
{
  //the dRs are initialised to a very high positive value
  dRClosetJet = 999.;
  dRClosetCentralJet = 999.;
  //the pTs are initialised to a negative value
  PtClosetJet = -999.;
  PtClosetCentralJet = -999.;
  // these variables are not used
  dROverPtClosetJet = -999.;
  dROverPtClosetCentralJet = -999.;

  for (const auto* const jetPtr : jets) {
    if (jetPtr->pt()<25000.) continue;
    double newdR = top::deltaR(*jetPtr,mu);
    double newdRCentral = newdR;
    if (jetPtr->eta()<2.5) {
      if (newdRCentral<dRClosetCentralJet) {
	dRClosetCentralJet = newdRCentral;
	PtClosetCentralJet = jetPtr->pt()/1000.; // tool needs GeV
      }
    }
    if (newdR<dRClosetJet) {
      dRClosetJet = newdR;
      PtClosetJet = jetPtr->pt()/1000.; // tool needs GeV
    }
  }

  // these variables are not used
  if ((dRClosetJet!=999.) && (PtClosetJet!=-999.)) dROverPtClosetJet = dRClosetJet/PtClosetJet;
  if ((dRClosetCentralJet!=999.) && (PtClosetCentralJet!=-999.)) dROverPtClosetCentralJet = dRClosetCentralJet/PtClosetCentralJet;

  //if the dRs are at their default value, give them a negative value
  if (dRClosetJet == 999.) dRClosetJet = -999.;
  if (dRClosetCentralJet == 999.) dRClosetCentralJet = -999.;
}
 
// =================================================
void top::fakes::GetdPhiLepMET (const xAOD::Electron& el, const xAOD::MissingET& met, float& dPhi)
{
  dPhi = -999.;
  if (met.mpx()!=0. && met.mpy()!=0.) {
    TLorentzVector met_4vect;
    met_4vect.SetPxPyPzE(met.mpx(),met.mpy(),0.,0.);
    dPhi = el.p4().DeltaPhi(met_4vect);
  }
}

// ==================================================
void top::fakes::GetdPhiLepMET (const xAOD::Muon& mu, const xAOD::MissingET& met, float& dPhi)
{
  dPhi = -999.;
  if (met.mpx()!=0. && met.mpy()!=0.) {
    TLorentzVector met_4vect;
    met_4vect.SetPxPyPzE(met.mpx(),met.mpy(),0.,0.);
    dPhi = mu.p4().DeltaPhi(met_4vect);
  }
}

// =========================================================
void top::fakes::GetTrigMatchVariable(const xAOD::Electron& el, int& trigmatch, const unsigned int runNumber)
{
  std::string release_series = "";
  const char* rel_temp = std::getenv("ROOTCORE_RELEASE_SERIES");
  if (rel_temp) {
    release_series = std::string(rel_temp);
  } else {
    std::cerr << "Can not access ROOTCORE_RELEASE_SERIES - exiting..." << std::endl;
    exit(1);
  }
  
  trigmatch = 0;
  bool trig_lowpTIso,trig_hightpTNoIso,trig_veryhightpTNoIso,trig_lowpTNoIso;

  if (release_series == "23") {// for 20.1 samples
    try {//not really used - trig_lowpTIso is overriden below
      trig_lowpTIso = el.auxdataConst<char>("TRIGMATCH_HLT_e24_lhmedium_iloose_L1EM20VH");
    } catch (std::exception& e) {
      trig_lowpTIso = false;
    }
    try {
      trig_hightpTNoIso = el.auxdataConst<char>("TRIGMATCH_HLT_e60_lhmedium");
    } catch (std::exception& e) {
      trig_hightpTNoIso = false;
    }
    try {
      trig_veryhightpTNoIso = el.auxdataConst<char>("TRIGMATCH_HLT_e120_lhloose");
    } catch (std::exception& e) {
      trig_veryhightpTNoIso = false;
    }
    try {
      trig_lowpTIso = trig_lowpTNoIso = ( el.auxdataConst<char>("TRIGMATCH_HLT_e24_lhmedium_L1EM18VH") || el.auxdataConst<char>("TRIGMATCH_HLT_e24_lhmedium_L1EM20VH") );
    } catch (std::exception& e) {
      trig_lowpTIso = trig_lowpTNoIso = false;
    }
  }
  else if (release_series == "24") {// for 20.7 samples
    if (runNumber>=276262 && runNumber<=284484) {
      try {//not really used - trig_lowpTIso is overriden below
        trig_lowpTIso = el.auxdataConst<char>("TRIGMATCH_HLT_e24_lhmedium_iloose_L1EM20VH");
      } catch (std::exception& e) {
        trig_lowpTIso = false;
      }
      try {
        trig_hightpTNoIso = el.auxdataConst<char>("TRIGMATCH_HLT_e60_lhmedium");
      } catch (std::exception& e) {
        trig_hightpTNoIso = false;
      }
      try {
        trig_veryhightpTNoIso = el.auxdataConst<char>("TRIGMATCH_HLT_e120_lhloose");
      } catch (std::exception& e) {
        trig_veryhightpTNoIso = false;
      }
      try {
        trig_lowpTNoIso = trig_lowpTIso = el.auxdataConst<char>("TRIGMATCH_HLT_e24_lhmedium_L1EM20VH");
      } catch (std::exception& e) {
        trig_lowpTNoIso = false;
      }
    }
    else if (runNumber>=297730) {
      try {
        trig_lowpTIso = el.auxdataConst<char>("TRIGMATCH_HLT_HLT_e24_lhtight_nod0_ivarloose");
      } catch (std::exception& e) {
        trig_lowpTIso = false;
      }
      try {
        trig_hightpTNoIso = el.auxdataConst<char>("TRIGMATCH_HLT_HLT_e60_lhmedium_nod0");
      } catch (std::exception& e) {
        trig_hightpTNoIso = false;
      }
      try {
        trig_veryhightpTNoIso = el.auxdataConst<char>("TRIGMATCH_HLT_e140_lhloose_nod0");
      } catch (std::exception& e) {
        trig_veryhightpTNoIso = false;
      }
      try {
        trig_lowpTNoIso = ( el.auxdataConst<char>("TRIGMATCH_HLT_e24_lhmedium_L1EM20VH") );
      } catch (std::exception& e) {
        trig_lowpTNoIso = false;
      }
    }
    else {
      std::cerr<<"Nothing is well-defined for runNumber "<<runNumber<<" - exiting..." << std::endl;
      exit(1);
    }
  }
  else {
    std::cerr<<"Unknown release series - exiting..."<<std::endl;
    exit(1);
  }

  //if (trig_lowpTIso)                              trigmatch |= 0x1 << 0;
  //if (trig_hightpTNoIso || trig_veryhightpTNoIso) trigmatch |= 0x1 << 1;

  // NB note used for Run2 2015 as there is no trigger splitting
  if (trig_lowpTIso)         trigmatch |= 0x1 << 0;
  if (trig_hightpTNoIso || trig_veryhightpTNoIso)  trigmatch |= 0x1 << 1;
  if (trig_lowpTNoIso)       trigmatch |= 0x1 << 2;
}

// ===========================================================
void top::fakes::GetTrigMatchVariable(const xAOD::Muon& mu, int& trigmatch, const unsigned int runNumber)
{
  std::string release_series = "";
  const char* rel_temp = std::getenv("ROOTCORE_RELEASE_SERIES");
  if (rel_temp) {
    release_series = std::string(rel_temp);
  } else {
    std::cerr<<"Can not access ROOTCORE_RELEASE_SERIES - exiting..."<<std::endl;
    exit(1);
  }

  trigmatch = 0;
  bool trig_lowpTIso,trig_hightpTNoIso,trig_lowpTNoIso;

  if (release_series == "23") {// for 20.1 samples
    try {
      trig_lowpTIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu20_iloose_L1MU15");
    } catch (std::exception& e) {
      trig_lowpTIso = false;
    }
    try {
      trig_hightpTNoIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu50");
    } catch (std::exception& e) {
      trig_hightpTNoIso = false;
    }
    try {
      trig_lowpTNoIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu20_L1MU15");
    } catch (std::exception& e) {
      trig_lowpTNoIso = false;
    }
  }
  else if (release_series == "24") {// for 20.7 samples
    if (runNumber>=276262 && runNumber<=284484) {
      try {
        trig_lowpTIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu20_iloose_L1MU15");
      } catch (std::exception& e) {
        trig_lowpTIso = false;
      }
      try {
        trig_hightpTNoIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu50");
      } catch (std::exception& e) {
        trig_hightpTNoIso = false;
      }
      try {
        trig_lowpTNoIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu20_L1MU15");
      } catch (std::exception& e) {
        trig_lowpTNoIso = false;
      }
    }
    else if (runNumber>=297730) {
      try {
        trig_lowpTIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu24_ivarmedium");
      } catch (std::exception& e) {
        trig_lowpTIso = false;
      }
      try {
        trig_hightpTNoIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu50");
      } catch (std::exception& e) {
        trig_hightpTNoIso = false;
      }
      try {
        trig_lowpTNoIso = mu.auxdataConst<char>("TRIGMATCH_HLT_mu24");
      } catch (std::exception& e) {
        trig_lowpTNoIso = false;
      }
    }
    else {
      std::cerr<<"Nothing is well-defined for runNumber "<<runNumber<<" - exiting..."<<std::endl;
      exit(1);
    }
  }
  else {
    std::cerr<<"Unknown release series - exiting..."<<std::endl;
    exit(1);
  }

  //if (trig_lowpTIso)     trigmatch |= 0x1 << 0;
  //if (trig_hightpTNoIso) trigmatch |= 0x1 << 1;

  if (trig_lowpTIso)         trigmatch |= 0x1 << 0; // mu20i
  if (trig_hightpTNoIso)     trigmatch |= 0x1 << 1; // mu50
  if (trig_lowpTNoIso)       trigmatch |= 0x1 << 2; // mu20

}

// =============================================================
std::vector<unsigned int> top::fakes::GetGoodHighPtLeptons(const xAOD::ElectronContainer& els)
{
  unsigned int i(0);
  std::vector<unsigned int> good_els_index;
  for (const auto* const elPtr : els) {//loop on electrons
    if ( elPtr->isAvailable<char>("passPreORSelectionLoose") )  {//count only leptons wigth pT>25GeV that pass selection
      if ( elPtr->pt()> 25000. && (elPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) ) good_els_index.push_back(i);
    }
    i++;
  }//loop on electrons
  return good_els_index;
}

// =============================================================
std::vector<unsigned int> top::fakes::GetGoodHighPtLeptons(const xAOD::MuonContainer& mus)
{
  unsigned int i(0);
  std::vector<unsigned int> good_mus_index;
  for (const auto* const muPtr : mus) {//loop on muons
    if ( muPtr->isAvailable<char>("passPreORSelectionLoose") )  {//count only leptons wigth pT>25GeV that pass selection
      if ( muPtr->pt()> 25000. && (muPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) ) good_mus_index.push_back(i);
    }
    i++;
  }//loop on muons
  return good_mus_index;
}

// ============================================================
int top::fakes::GetChannel(const xAOD::ElectronContainer& els, 
			   const xAOD::MuonContainer& mus)
{
  //
  //
  //

  unsigned int iels = GetGoodHighPtLeptons(els).size();
  unsigned int imus = GetGoodHighPtLeptons(mus).size();
  if (iels==1 && imus==0) return FakesWeights::EJETS;
  else if (iels==0 && imus==1) return FakesWeights::MUJETS;
  else if (iels==2 && imus==0) return FakesWeights::EE;
  else if (iels==0 && imus==2) return FakesWeights::MUMU;
  else if (iels==1 && imus==1) return FakesWeights::EMU;
  else return -1;
}
