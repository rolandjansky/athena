/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
using namespace std;
#include "TrigTauPerformNtuple/TrigTauTrueParticle.h"

ClassImp(TrigTauTrueParticle)  


TrigTauTrueParticle::TrigTauTrueParticle() :
  TLorentzVector(0,0,0,0),
  m_nDaughters(0), m_pdgId(0), m_status(0),  r_mother(), r_daughters()
{}

TrigTauTrueParticle::TrigTauTrueParticle(const TrigTauTrueParticle & other ) :
  TLorentzVector(other),
  m_nDaughters(other.m_nDaughters),
  m_pdgId(other.m_pdgId),
  m_status(other.m_status),
  r_mother(other.r_mother),
  r_daughters(other.r_daughters)
{}

TrigTauTrueParticle::~TrigTauTrueParticle() {}
TrigTauTrueParticle & TrigTauTrueParticle::operator = (const TrigTauTrueParticle & other )
{
  TLorentzVector::operator = (other);
  m_nDaughters = other.m_nDaughters;
  r_mother = r_mother;
  m_pdgId = other.m_pdgId;
  m_status = other.m_status;

  // Copy this by hand because the assigment operator
  // of TCollection is private
  unsigned int nDau =  other.nSavedDaughters();
  for(unsigned int idau = 0; idau < nDau ; ++ idau)
    {
      TrigTauTrueParticle * newDau = const_cast<TrigTauTrueParticle*> (other.daughter(idau) );
      addDaughter(*newDau);
    }
  return *this;
}


void  TrigTauTrueParticle::print(int shift, bool completeTree) const
{
  for(int i = 0; i< shift; ++i )
    cout << "   ";
  cout << "PDG = "<< m_pdgId << ", pT="<< Pt() << ", eta="<< Eta()
       << ", phi="<< Phi()<< " stat="<< m_status ;
  if( mother() ) cout << ", moth=" << mother()->pdgId();
  cout << " Ndau="<< nDaughters() 
       << "(saved nDau=" << nSavedDaughters() << ")" << endl;

  if( completeTree )
    {
      unsigned int nDau =  nSavedDaughters();
      for(unsigned int idau = 0; idau < nDau ; ++ idau)
	{
	  cout << "|";
	  if(daughter(idau) == 0 ) {cout <<" ZERO DAUGHTER!!"; continue; }
	  daughter(idau)->print(shift+1,completeTree); 
	}
    }
  return;
} 


int TrigTauTrueParticle::charge() const  // determined from pdgId
{

  // Very brute-force and stupid way to do this.
  // Check for things we know from tau decays...
  switch (pdgId()) {
  case 22: // Photon
    return 0;

  case +12: // Neutrinos
  case -12:
  case +14:
  case -14:
  case +16:
  case -16:
    return 0;

  case +11: // electron
  case +13: // mu-
  case +15:
    return -1;

  case -11: // electron
  case -13: // mu-
  case -15:
    return +1;

  case 111: // Pi0
  case 130: // KL
  case 310: // Ks
  case 311: // K0
    return 0;

  case 211: // pi+
  case 321: // K+
    return +1;

  case -211: // pi-
  case -321: // K-
    return -1;

  default:
    cout << "Charge for PDG ID " << pdgId() << " not known!" << endl;
  }

  return 100;
}
const TrigTauTrueParticle * TrigTauTrueParticle::leadingParticle() const
{
  const TrigTauTrueParticle * pTrack(0);
  float maxPt = 0;
  
  unsigned int nDau = nSavedDaughters();
  for(unsigned int idau = 0; idau < nDau; ++idau  )
    { 
      const TrigTauTrueParticle * itr = daughter(idau);      
      float pt = itr->Pt();
      if( pt > maxPt ) { maxPt = pt; pTrack = &(*itr);}
    }
  return pTrack;
}

const char * TrigTauTrueParticle::pdgName(int pdgId)
{
  switch (pdgId ){
    case 1: return  "d" ;
  case -1: return  "anti_d" ;
  case 2: return  "u" ;
  case -2: return  "anti_u" ;
  case 3: return  "s" ;
  case -3: return  "anti_s" ;
  case 4: return  "c" ;
  case -4: return  "anti_c" ;
  case 5: return  "b" ;
  case -5: return  "anti_b" ;
  case 6: return  "t" ;
  case -6: return  "anti_t" ;
  case 7: return  "l" ;
  case -7: return  "anti_l" ;
  case 8: return  "h" ;
  case -8: return  "anti_h" ;
  case 21: return  "g" ;
  case 11: return  "e_minus" ;
  case -11: return  "e_plus" ;
  case 12: return  "nu_e" ;
  case -12: return  "anti_nu_e" ;
  case 13: return  "mu_minus" ;
  case -13: return  "mu_plus" ;
  case 14: return  "nu_mu" ;
  case -14: return  "anti_nu_mu" ;
  case 15: return  "tau_minus" ;
  case -15: return  "tau_plus" ;
  case 16: return  "nu_tau" ;
  case -16: return  "anti_nu_tau" ;
  case 17: return  "L_minus" ;
  case -17: return  "L_plus" ;
  case 18: return  "nu_L" ;
  case -18: return  "anti_nu_L" ;
  case 22: return  "gamma" ;
  case 23: return  "Z0" ;
  case 24: return  "W_plus" ;
  case -24: return  "W_minus" ;
  case 25: return  "Higgs0" ;
  case 28: return  "reggeon" ;
  case 29: return  "pomeron" ;
  case 32: return  "Z_prime0" ;
  case 33: return  "Z_prime_prime0" ;
  case 34: return  "W_prime_plus" ;
  case -34: return  "W_prime_minus" ;
  case 35: return  "Higgs_prime0" ;
  case 36: return  "A0" ;
  case 37: return  "Higgs_plus" ;
  case -37: return  "Higgs_minus" ;
  case 40: return  "R0" ;
  case -40: return  "anti_R0" ;
  case 81: return  "specflav" ;
  case 82: return  "rndmflav" ;
  case -82: return  "anti_rndmflav" ;
  case 83: return  "phasespa" ;
  case 84: return  "c_minushadron" ;
  case -84: return  "anti_c_minushadron" ;
  case 85: return  "b_minushadron" ;
  case -85: return  "anti_b_minushadron" ;
  case 86: return  "t_minushadron" ;
  case -86: return  "anti_t_minushadron" ;
  case 89: return  "Wvirt_plus" ;
  case -89: return  "Wvirt_minus" ;
  case 90: return  "diquark" ;
  case -90: return  "anti_diquark" ;
  case 91: return  "cluster" ;
  case 92: return  "string" ;
  case 93: return  "indep" ;
  case 94: return  "CMshower" ;
  case 95: return  "SPHEaxis" ;
  case 96: return  "THRUaxis" ;
  case 97: return  "CLUSjet" ;
  case 98: return  "CELLjet" ;
  case 99: return  "table" ;
  case 111: return  "pi0" ;
  case 211: return  "pi_plus" ;
  case -211: return  "pi_minus" ;
  case 210: return  "pi_diffr_plus" ;
  case -210: return  "pi_diffr_minus" ;
  case 20111: return  "pi_2S0" ;
  case 20211: return  "pi_2S_plus" ;
  case -20211: return  "pi_2S_minus" ;
  case 221: return  "eta" ;
  case 20221: return  "eta_2S" ;
  case 331: return  "eta_prime" ;
  case 113: return  "rho0" ;
  case 213: return  "rho_plus" ;
  case -213: return  "rho_minus" ;
  case 30113: return  "rho_2S0" ;
  case 30213: return  "rho_2S_plus" ;
  case -30213: return  "rho_2S_minus" ;
  case 40113: return  "rho_3S0" ;
  case 40213: return  "rho_3S_plus" ;
  case -40213: return  "rho_3S_minus" ;
  case 223: return  "omega" ;
  case 30223: return  "omega_2S" ;
  case 333: return  "phi" ;
  case 10111: return  "a_00" ;
  case 10211: return  "a_0_plus" ;
  case -10211: return  "a_0_minus" ;
  case 10221: return  "f_0" ;
  case 10331: return  "f_prime_0" ;
  case 10113: return  "b_10" ;
  case 10213: return  "b_1_plus" ;
  case -10213: return  "b_1_minus" ;
  case 10223: return  "h_1" ;
  case 10333: return  "h_prime_1" ;
  case 20113: return  "a_10" ;
  case 20213: return  "a_1_plus" ;
  case -20213: return  "a_1_minus" ;
  case 20223: return  "f_1" ;
  case 20333: return  "f_prime_1" ;
  case 115: return  "a_20" ;
  case 215: return  "a_2_plus" ;
  case -215: return  "a_2_minus" ;
  case 225: return  "f_2" ;
  case 335: return  "f_prime_2" ;
  case 311: return  "K0" ;
  case -311: return  "anti_K0" ;
  case 310: return  "K_S0" ;
  case 130: return  "K_L0" ;
  case 321: return  "K_plus" ;
  case -321: return  "K_minus" ;
  case 313: return  "K_star0" ;
  case -313: return  "anti_K_star0" ;
  case 323: return  "K_star_plus" ;
  case -323: return  "K_star_minus" ;
  case 10311: return  "K_0_star0" ;
  case -10311: return  "anti_K_0_star0" ;
  case 10321: return  "K_0_star_plus" ;
  case -10321: return  "K_0_star_minus" ;
  case 10313: return  "K_10" ;
  case -10313: return  "anti_K_10" ;
  case 10323: return  "K_1_plus" ;
  case -10323: return  "K_1_minus" ;
  case 315: return  "K_2_star0" ;
  case -315: return  "anti_K_2_star0" ;
  case 325: return  "K_2_star_plus" ;
  case -325: return  "K_2_star_minus" ;
  case 20313: return  "K_prime_10" ;
  case -20313: return  "anti_K_prime_10" ;
  case 20323: return  "K_prime_1_plus" ;
  case -20323: return  "K_prime_1_minus" ;
  case 411: return  "D_plus" ;
  case -411: return  "D_minus" ;
  case 421: return  "D0" ;
  case -421: return  "anti_D0" ;
  case 413: return  "D_star_plus" ;
  case -413: return  "D_star_minus" ;
  case 423: return  "D_star0" ;
  case -423: return  "anti_D_star0" ;
  case 10411: return  "D_0_star_plus" ;
  case -10411: return  "D_0_star_minus" ;
  case 10421: return  "D_0_star0" ;
  case -10421: return  "anti_D_0_star0" ;
  case 10413: return  "D_1_plus" ;
  case -10413: return  "D_1_minus" ;
  case 10423: return  "D_10" ;
  case -10423: return  "anti_D_10" ;
  case 415: return  "D_2_star_plus" ;
  case -415: return  "D_2_star_minus" ;
  case 425: return  "D_2_star0" ;
  case -425: return  "anti_D_2_star0" ;
  case 20413: return  "D_prime_1_plus" ;
  case -20413: return  "D_prime_1_minus" ;
  case 20423: return  "D_prime_10" ;
  case -20423: return  "anti_D_prime_10" ;
  case 431: return  "D_s_plus" ;
  case -431: return  "D_s_minus" ;
  case 433: return  "D_s_star_plus" ;
  case -433: return  "D_s_star_minus" ;
  case 10431: return  "D_s0_star_plus" ;
  case -10431: return  "D_s0_star_minus" ;
  case 10433: return  "D_s1_plus" ;
  case -10433: return  "D_s1_minus" ;
  case 435: return  "D_s2_star_plus" ;
  case -435: return  "D_s2_star_minus" ;
  case 20433: return  "D_prime_s1_plus" ;
  case -20433: return  "D_prime_s1_minus" ;
  case 511: return  "B0" ;
  case -511: return  "anti_B0" ;
  case 521: return  "B_plus" ;
  case -521: return  "B_minus" ;
  case 513: return  "B_star0" ;
  case -513: return  "anti_B_star0" ;
  case 523: return  "B_star_plus" ;
  case -523: return  "B_star_minus" ;
  case 10511: return  "B_0_star0" ;
  case -10511: return  "anti_B_0_star0" ;
  case 10521: return  "B_0_star_plus" ;
  case -10521: return  "B_0_star_minus" ;
  case 10513: return  "B_10" ;
  case -10513: return  "anti_B_10" ;
  case 10523: return  "B_1_plus" ;
  case -10523: return  "B_1_minus" ;
  case 515: return  "B_2_star0" ;
  case -515: return  "anti_B_2_star0" ;
  case 525: return  "B_2_star_plus" ;
  case -525: return  "B_2_star_minus" ;
  case 20513: return  "B_prime_10" ;
  case -20513: return  "anti_B_prime_10" ;
  case 20523: return  "B_prime_1_plus" ;
  case -20523: return  "B_prime_1_minus" ;
  case 531: return  "B_s0" ;
  case -531: return  "anti_B_s0" ;
  case 533: return  "B_s_star0" ;
  case -533: return  "anti_B_s_star0" ;
  case 10531: return  "B_s0_star0" ;
  case -10531: return  "anti_B_s0_star0" ;
  case 10533: return  "B_s10" ;
  case -10533: return  "anti_B_s10" ;
  case 535: return  "B_s2_star0" ;
  case -535: return  "anti_B_s2_star0" ;
  case 20533: return  "B_prime_s10" ;
  case -20533: return  "anti_B_prime_s10" ;
  case 541: return  "B_c_plus" ;
  case -541: return  "B_c_minus" ;
  case 543: return  "B_c_star_plus" ;
  case -543: return  "B_c_star_minus" ;
  case 10541: return  "B_c0_star_plus" ;
  case -10541: return  "B_c0_star_minus" ;
  case 10543: return  "B_c1_plus" ;
  case -10543: return  "B_c1_minus" ;
  case 545: return  "B_c2_star_plus" ;
  case -545: return  "B_c2_star_minus" ;
  case 20543: return  "B_prime_c1_plus" ;
  case -20543: return  "B_prime_c1_minus" ;
  case 441: return  "eta_c" ;
  case 20441: return  "eta_c_2S" ;
  case 443: return  "J_psi" ;
  case 20443: return  "psi_2S" ;
  case 10441: return  "chi_c0" ;
  case 10443: return  "chi_c1" ;
  case 445: return  "chi_c2" ;
  case 20551: return  "eta_b_2S" ;
  case 40551: return  "eta_b_3S" ;
  case 553: return  "Upsilon" ;
  case 100553: return  "Upsilon_2S" ;
  case 60553: return  "Upsilon_3S" ;
  case 70553: return  "Upsilon_4S" ;
  case 80553: return  "Upsilon_5S" ;
    //case 10553: return  "h_b" ;
  case 40553: return  "h_b_2P" ;
    //case 10553: return  "h_b_3P" ;
  case 551: return  "chi_b0" ;
  case 20553: return  "chi_b1" ;
  case 555: return  "chi_b2" ;
  case 30551: return  "chi_b0_2P" ;
  case 50553: return  "chi_b1_2P" ;
  case 10555: return  "chi_b2_2P" ;
  case 50551: return  "chi_b0_3P" ;
  case 110553: return  "chi_b1_3P" ;
  case 20555: return  "chi_b2_3P" ;
  case 40555: return  "eta_b2_1D" ;
  case 60555: return  "eta_b2_2D" ;
  case 120553: return  "Upsilon_1_1D" ;
  case 30555: return  "Upsilon_2_1D" ;
  case 557: return  "Upsilon_3_1D" ;
  case 130553: return  "Upsilon_1_2D" ;
  case 50555: return  "Upsilon_2_2D" ;
  case 10557: return  "Upsilon_3_2D" ;
  case 1114: return  "Delta_minus" ;
  case -1114: return  "anti_Delta_plus" ;
  case 2110: return  "n_diffr" ;
  case -2110: return  "anti_n_diffr" ;
  case 2112: return  "n0" ;
  case -2112: return  "anti_n0" ;
  case 2114: return  "Delta0" ;
  case -2114: return  "anti_Delta0" ;
  case 2210: return  "p_diffr_plus" ;
  case -2210: return  "anti_p_diffr_minus" ;
  case 2212: return  "p_plus" ;
  case -2212: return  "anti_p_minus" ;
  case 2214: return  "Delta_plus" ;
  case -2214: return  "anti_Delta_minus" ;
  case 2224: return  "Delta_plus_plus" ;
  case -2224: return  "anti_Delta_minus_minus" ;
  case 3112: return  "Sigma_minus" ;
  case -3112: return  "anti_Sigma_plus" ;
  case 3114: return  "Sigma_star_minus" ;
  case -3114: return  "anti_Sigma_star_plus" ;
  case 3122: return  "Lambda0" ;
  case -3122: return  "anti_Lambda0" ;
  case 3212: return  "Sigma0" ;
  case -3212: return  "anti_Sigma0" ;
  case 3214: return  "Sigma_star0" ;
  case -3214: return  "anti_Sigma_star0" ;
  case 3222: return  "Sigma_plus" ;
  case -3222: return  "anti_Sigma_minus" ;
  case 3224: return  "Sigma_star_plus" ;
  case -3224: return  "anti_Sigma_star_minus" ;
  case 3312: return  "Xi_minus" ;
  case -3312: return  "anti_Xi_plus" ;
  case 3314: return  "Xi_star_minus" ;
  case -3314: return  "anti_Xi_star_plus" ;
  case 3322: return  "Xi0" ;
  case -3322: return  "anti_Xi0" ;
  case 3324: return  "Xi_star0" ;
  case -3324: return  "anti_Xi_star0" ;
  case 3334: return  "Omega_minus" ;
  case -3334: return  "anti_Omega_plus" ;
  case 4112: return  "Sigma_c0" ;
  case -4112: return  "anti_Sigma_c0" ;
  case 4114: return  "Sigma_c_star0" ;
  case -4114: return  "anti_Sigma_c_star0" ;
  case 4122: return  "Lambda_c_plus" ;
  case -4122: return  "anti_Lambda_c_minus" ;
  case 4132: return  "Xi_c0" ;
  case -4132: return  "anti_Xi_c0" ;
  case 4212: return  "Sigma_c_plus" ;
  case -4212: return  "anti_Sigma_c_minus" ;
  case 4214: return  "Sigma_c_star_plus" ;
  case -4214: return  "anti_Sigma_c_star_minus" ;
  case 4222: return  "Sigma_c_plus_plus" ;
  case -4222: return  "anti_Sigma_c_minus_minus" ;
  case 4224: return  "Sigma_c_star_plus_plus" ;
  case -4224: return  "anti_Sigma_c_star_minus_minus" ;
  case 4322: return  "Xi_c_plus" ;
  case -4322: return  "anti_Xi_c_minus" ;
  case 4312: return  "Xi_prime_c0" ;
  case -4312: return  "Xi_primeanti__c0" ;
  case 4314: return  "Xi_c_star0" ;
  case -4314: return  "anti_Xi_c_star0" ;
  case 4232: return  "Xi_prime_c_plus" ;
  case -4232: return  "Xi_primeanti__c_minus" ;
  case 4324: return  "Xi_c_star_plus" ;
  case -4324: return  "anti_Xi_c_star_minus" ;
  case 4332: return  "Omega_c0" ;
  case -4332: return  "anti_Omega_c0" ;
  case 4334: return  "Omega_c_star0" ;
  case -4334: return  "anti_Omega_c_star0" ;
  case 5112: return  "Sigma_b_minus" ;
  case -5112: return  "anti_Sigma_b_plus" ;
  case 5114: return  "Sigma_b_star_minus" ;
  case -5114: return  "anti_Sigma_b_star_plus" ;
  case 5122: return  "Lambda_b0" ;
  case -5122: return  "anti_Lambda_b0" ;
  case 5132: return  "Xi_b_minus" ;
  case -5132: return  "anti_Xi_b_plus" ;
  case 5212: return  "Sigma_b0" ;
  case -5212: return  "anti_Sigma_b0" ;
  case 5214: return  "Sigma_b_star0" ;
  case -5214: return  "anti_Sigma_b_star0" ;
  case 5222: return  "Sigma_b_plus" ;
  case -5222: return  "anti_Sigma_b_minus" ;
  case 5224: return  "Sigma_star_" ;
  case -5224: return  "anti_Sigma_b_star_minus" ;
  case 5232: return  "Xi_b0" ;
  case -5232: return  "anti_Xi_b0" ;
  case 5312: return  "Xi_prime_b_minus" ;
  case -5312: return  "anti_Xi_prime_b_plus" ;
  case 5314: return  "Xi_b_star_minus" ;
  case -5314: return  "anti_Xi_b_star_plus" ;
  case 5322: return  "Xi_prime_b0" ;
  case -5322: return  "anti_Xi_prime_b0" ;
  case 5324: return  "Xi_b_star0" ;
  case -5324: return  "anti_Xi_b_star0" ;
  case 5332: return  "Omega_b_minus" ;
  case -5332: return  "anti_Omega_b_plus" ;
  case 5334: return  "Omega_b_star_minus" ;
  case -5334: return  "anti_Omega_b_star_plus" ;
  case 1101: return  "dd_0" ;
  case -1101: return  "anti_dd_0" ;
  case 2101: return  "ud_0" ;
  case -2101: return  "anti_ud_0" ;
  case 2201: return  "uu_0" ;
  case -2201: return  "anti_uu_0" ;
  case 3101: return  "sd_0" ;
  case -3101: return  "anti_sd_0" ;
  case 3201: return  "su_0" ;
  case -3201: return  "anti_su_0" ;
  case 3301: return  "ss_0" ;
  case -3301: return  "anti_ss_0" ;
  case 4101: return  "cd_0" ;
  case -4101: return  "anti_cd_0" ;
  case 4201: return  "cu_0" ;
  case -4201: return  "anti_cu_0" ;
  case 4301: return  "cs_0" ;
  case -4301: return  "anti_cs_0" ;
  case 4401: return  "cc_0" ;
  case -4401: return  "anti_cc_0" ;
  case 5101: return  "bd_0" ;
  case -5101: return  "anti_bd_0" ;
  case 5201: return  "bu_0" ;
  case -5201: return  "anti_bu_0" ;
  case 5301: return  "bs_0" ;
  case -5301: return  "anti_bs_0" ;
  case 5401: return  "bc_0" ;
  case -5401: return  "anti_bc_0" ;
  case 5501: return  "bb_0" ;
  case -5501: return  "anti_bb_0" ;
  case 1103: return  "dd_1" ;
  case -1103: return  "anti_dd_1" ;
  case 2103: return  "ud_1" ;
  case -2103: return  "anti_ud_1" ;
  case 2203: return  "uu_1" ;
  case -2203: return  "anti_uu_1" ;
  case 3103: return  "sd_1" ;
  case -3103: return  "anti_sd_1" ;
  case 3203: return  "su_1" ;
  case -3203: return  "anti_su_1" ;
  case 3303: return  "ss_1" ;
  case -3303: return  "anti_ss_1" ;
  case 4103: return  "cd_1" ;
  case -4103: return  "anti_cd_1" ;
  case 4203: return  "cu_1" ;
  case -4203: return  "anti_cu_1" ;
  case 4303: return  "cs_1" ;
  case -4303: return  "anti_cs_1" ;
  case 4403: return  "cc_1" ;
  case -4403: return  "anti_cc_1" ;
  case 5103: return  "bd_1" ;
  case -5103: return  "anti_bd_1" ;
  case 5203: return  "bu_1" ;
  case -5203: return  "anti_bu_1" ;
  case 5303: return  "bs_1" ;
  case -5303: return  "anti_bs_1" ;
  case 5403: return  "bc_1" ;
  case -5403: return  "anti_bc_1" ;
  case 5503: return  "bb_1" ;
  case -5503: return  "anti_bb_1" ;
  
  // Supersymmetric particles
  case 1000001: return  "sdown_L";
  case -1000001: return  "anti_sdown_L";
  case 1000002: return  "sup_L";
  case -1000002: return  "anti_sup_L";
  case 1000003: return  "sstrange_L";
  case -1000003: return  "anti_sstrange_L";
  case 1000004: return  "scharm_L";
  case -1000004: return  "anti_scharm_L";
  case 1000005: return  "sbottom_1";
  case -1000005: return  "anti_sbottom_1";
  case 1000006: return  "stop_1";
  case -1000006: return  "anti_stop_1";
  
  case 1000011: return  "selectron_L";
  case -1000011: return  "anti_selectron_L";
  case 1000012: return  "snu_electron_L";
  case -1000012: return  "anti_snu_electron_L";
  case 1000013: return  "smuon_L";
  case -1000013: return  "anti_smuon_L";
  case 1000014: return  "snu_muon_L";
  case -1000014: return  "anti_snu_muon_L";
  case 1000015: return  "stau_L";
  case -1000015: return  "anti_stau_L";
  case 1000016: return  "snu_tau_L";
  case -1000016: return  "anti_snu_tau_L";
  
  case 2000001: return  "sdown_R";
  case -2000001: return  "anti_sdown_R";
  case 2000002: return  "sup_R";
  case -2000002: return  "anti_sup_R";
  case 2000003: return  "sstrange_R";
  case -2000003: return  "anti_sstrange_R";
  case 2000004: return  "scharm_R";
  case -2000004: return  "anti_scharm_R";
  case 2000005: return  "sbottom_2";
  case -2000005: return  "anti_sbottom_2";
  case 2000006: return  "stop_2";
  case -2000006: return  "anti_stop_2";

  case 2000011: return  "selectron_R";
  case -2000011: return  "anti_selectron_R";
  case 2000013: return  "smuon_R";
  case -2000013: return  "anti_smuon_R";
  case 2000015: return  "stau_R";
  case -2000015: return  "anti_stau_R";
   case 1000021: return  "gluino";
  case -1000021: return  "anti_gluino";
  
  case 1000022: return  "neutralino_1";
  case -1000022: return  "anti_neutralino_1";
  case 1000023: return  "neutralino_2";
  case -1000023: return  "anti_neutralino_2";
  case 1000024: return  "chargino_1";
  case -1000024: return  "anti_chargino_1";
  case 1000025: return  "neutralino_3";
  case -1000025: return  "anti_neutralino_3";
  case 1000035: return  "neutralino_4";
  case -1000035: return  "anti_neutralino_4";
  case 1000037: return  "chargino_2";
  case -1000037: return  "anti_chargino_2";
  case 1000039: return  "gravitino";
  case -1000039: return  "anti_gravitino";
  default : break;
  }
  return "unknown";
}

const char * TrigTauTrueParticle::statusName(int status)
{
  switch (status ){
  
  case 1   : return  "final state particle" ;
  case 2   : return  "parton before hadronization" ;
  case 3   : return  "documentation line" ;
  case 100 : return  "cone limiting jet evolution" ;
  case 101 : return  "'beam' (beam 1)" ;
  case 102 : return  "'target' (beam 2)" ;
  case 103 : return  "overall centre of mass" ;
  case 110 : return  "unprocessed hard process c.m." ;
  case 111 : return  "unprocessed beam parton" ;
  case 112 : return  "unprocessed target parton" ;
  case 113 : return  "unproc. first outgoing parton" ;
  case 114 : return  "unproc. other outgoing parton" ;
  case 115 : return  "unprocessed spectator parton" ;
  case 120 : return  "unprocessed hard process c.m. after processing" ;
  case 121 : return  "unprocessed beam parton after processing" ;
  case 122 : return  "unprocessed target parton after processing" ;
  case 123 : return  "unproc. first outgoing parton after processing" ;
  case 124 : return  "unproc. other outgoing parton after processing" ;
  case 125 : return  "unprocessed spectator parton after processing" ;
  case 130 : return  "lepton in jet (unboosted)" ;
  case 131 : return  "jet from unprocessed beam parton unboosted to c.m." ;
  case 132 : return  "jet from unprocessed target parton unboosted to c.m." ;
  case 133 : return  "jet from unproc. first outgoing parton unboosted to c.m." ;
  case 134 : return  "jet from unproc. other outgoing parton unboosted to c.m." ;
  case 135 : return  "spacelike parton (beam, unboosted)" ;
  case 136 : return  "spacelike parton (target,unboosted)" ;
  case 137 : return  "spectator (beam, unboosted)" ;
  case 138 : return  "spectator (target, unboosted)" ;
  case 139 : return  "parton from branching (unboosted)" ;
  case 140 : return  "parton from gluon splitting (unboosted)" ;
  case 141 : return  "jet from unprocessed beam parton" ;
  case 142 : return  "jet from unprocessed target parton" ;
  case 143 : return  "jet from unproc. first outgoing parton" ;
  case 144 : return  "jet from unproc. other outgoing parton" ;
  case 145 : return  "spacelike parton (beam, boosted, unclustered)" ;
  case 146 : return  "spacelike parton (target,boosted, unclustered)" ;
  case 147 : return  "spectator (beam, boosted, unclustered)" ;
  case 148 : return  "spectator (target, boosted, unclustered)" ;
  case 149 : return  "parton from branching (boosted, unclustered)" ;
  case 150 : return  "parton from gluon splitting (boosted, unclustered)" ;
  case 151 : return  "parton from cluster splitting, not yet clustered" ;
  case 152 : return  "spectator after heavy decay, not yet clustered" ;
  case 153 : return  "spectator from beam" ;
  case 154 : return  "spectator from target" ;
  case 155 : return  "heavy quark before decay" ;
  case 156 : return  "spectator before heavy decay" ;
  case 157 : return  "parton from QCD branching" ;
  case 158 : return  "parton from gluon splitting" ;
  case 159 : return  "parton from cluster splitting" ;
  case 160 : return  "spectator after heavy decay" ;
  case 161 : return  "beam spectator after gluon splitting" ;
  case 162 : return  "target spectator after gluon splitting" ;
  case 163 : return  "other cluster before soft process" ;
  case 164 : return  "beam cluster before soft process" ;
  case 165 : return  "target cluster before soft process" ;
  case 167 : return  "unhadronized beam cluster" ;
  case 168 : return  "unhadronized target cluster" ;
  case 170 : return  "soft process centre of mass" ;
  case 171 : return  "soft cluster (beam, unhadronized)" ;
  case 172 : return  "soft cluster (target, unhadronized)" ;
  case 173 : return  "soft cluster (other, unhadronized)" ;
  case 181 : return  "beam cluster (no soft process)" ;
  case 182 : return  "target cluster (no soft process)" ;
  case 183 : return  "hard process cluster (hadronized)" ;
  case 184 : return  "soft cluster (beam, hadronized)" ;
  case 185 : return  "soft cluster (target, hadronized)" ;
  case 186 : return  "soft cluster (other, hadronized)" ;
  case 190 : return  "direct unstable non-hadron, before decays" ;
  case 191 : return  "direct unstable hadron (1-body clus.), before decays" ;
  case 192 : return  "direct unstable hadron (2-body clus.), before decays" ;
  case 193 : return  "indirect unstable hadron or lepton, before decays" ;
  case 195 : return  "direct unstable non-hadron" ;
  case 196 : return  "direct unstable hadron (1-body clus.)" ;
  case 197 : return  "direct unstable hadron (2-body clus.)" ;
  case 198 : return  "indirect unstable hadron or lepton" ;
  case 199 : return  "decayed heavy flavour hadron" ;
  case 200 : return  "neutral B meson, flavour at prod'n" ;
  default : break;
  }

  return "unknown status";
}

void  TrigTauTrueParticle::prettyPrintDecayChain(unsigned int indent) const
{
 unsigned int nDau = nSavedDaughters();
  if( nDau == 1 && daughter( 0)->pdgId() == pdgId())
  {
    daughter(0)->prettyPrintDecayChain(  indent);
    return;
  }

  for( unsigned int i=0; i < indent; ++i)
      std::cout << " ";

  print( 0, false);

  for( unsigned int i=0; i < nDau; ++i)
    daughter(i)->prettyPrintDecayChain(  indent+2);
  
}



bool  TrigTauTrueParticle::isSusyParticle() const
{
  unsigned int susyType = std::abs( pdgId()) / 1000000;
  if( susyType == 1 || susyType == 2)
      return true;
  else
      return false;
}

bool  TrigTauTrueParticle::isStartOfSusyChain() const
{
  if( isSusyParticle())
      return true;

  for( unsigned int i=0; i < nSavedDaughters(); ++i)
  {
    if( daughter(i)->isSusyParticle())
        return true;
  }

  for( unsigned int i=0; i < nSavedDaughters(); ++i)
  {
    if( daughter(i)->isStartOfSusyChain())
        return true;
  }

  return false;
}

const TrigTauTrueParticle*  TrigTauTrueParticle::findMotherOfFirstSusyDecay() const
{
  if( isSusyParticle())
  {
    if( mother())
        return mother();
    else
        return this;
  }

  for( unsigned int i=0; i < nSavedDaughters(); ++i)
  {
    if( daughter(i)->isSusyParticle())
        return this;
  }

  for( unsigned int i=0; i < nSavedDaughters(); ++i)
  {
    const TrigTauTrueParticle* ret = 0;
    ret = daughter(i)->findMotherOfFirstSusyDecay();
    if( ret)
        return ret;
  }

  return 0;
}
