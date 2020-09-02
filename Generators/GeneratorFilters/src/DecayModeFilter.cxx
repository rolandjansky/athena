/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DecayModeFilter.h"
#include <cmath>
#include <sstream>

using namespace std;


inline bool isIn(const std::vector<unsigned int>& pdgs, int pdg) {
  return std::find(pdgs.begin(), pdgs.end(), static_cast<unsigned int>(std::abs(pdg))) != pdgs.end();
}


DecayModeFilter::DecayModeFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator),
    m_nDirectModeEvents(0), m_nBosonicModeEvents(0), m_nLeptonicModeEvents(0),
    m_nDirect3bodyModeEvents(0), m_nDoubleBosonicMode(0), m_nDoubleLeptonicMode(0), m_nBosonicLeptonicMode(0),
    m_nDirectDecays(0), m_nBosonicDecays(0),
    m_nLeptonicDecays(0), m_nDirect3bodyDecays(0)
{
  declareProperty("modeID", m_modeID ="-" );
  declareProperty("submode", m_submode = "-" );
  declareProperty("nChargedLeptons", m_nChargedLeptons = -1 );
  //
  declareProperty("producedParticlesPDG", m_producedParticles_PDG);  //only set these if you want to change default values
  declareProperty("chi2sPDG", m_chi2s_PDG);
  //declareProperty("sleptonsPDG", m_sleptons_PDG);
  declareProperty("LSPsPDG", m_LSPs_PDG);

  m_selected = false;
}


StatusCode DecayModeFilter::filterInitialize() {
  if (m_modeID == "-") {
    ATH_MSG_INFO("No decay mode m_selected: Events will only be analyzed, not cut.");
  }
  if (m_submode == "C" && m_nChargedLeptons == -1) {
    ATH_MSG_ERROR("When selecting a specific leptonic submode, also nChargedLeptons has to be set!");
  }

  m_nDirectModeEvents = 0;
  m_nBosonicModeEvents = 0;
  m_nLeptonicModeEvents = 0;
  m_nDirect3bodyModeEvents = 0;

  m_nDirectDecays = 0;
  m_nBosonicDecays = 0;
  m_nLeptonicDecays = 0;
  m_nDirect3bodyDecays = 0;

  m_nDoubleBosonicMode = 0;
  m_nDoubleLeptonicMode = 0;
  m_nBosonicLeptonicMode = 0;

  if (m_producedParticles_PDG.empty()) {
    m_producedParticles_PDG.push_back(1000001); //d_L
    m_producedParticles_PDG.push_back(1000002); //u_L
    m_producedParticles_PDG.push_back(1000003); //s_L
    m_producedParticles_PDG.push_back(1000004); //c_L
    m_producedParticles_PDG.push_back(1000005); //b_1
    m_producedParticles_PDG.push_back(1000006); //t_1
    m_producedParticles_PDG.push_back(2000001); //d_R
    m_producedParticles_PDG.push_back(2000002); //u_R
    m_producedParticles_PDG.push_back(2000003); //s_R
    m_producedParticles_PDG.push_back(2000004); //c_R
    m_producedParticles_PDG.push_back(2000005); //b_2
    m_producedParticles_PDG.push_back(2000006); //t_2
    m_producedParticles_PDG.push_back(1000021); //gluino
  }

  if (m_chi2s_PDG.empty()) {
    m_chi2s_PDG.push_back(1000023); //chi^0_2
    m_chi2s_PDG.push_back(1000024); //chi^+_1
    m_chi2s_PDG.push_back(1000025); //chi^0_3
    m_chi2s_PDG.push_back(1000035); //chi^0_4
    m_chi2s_PDG.push_back(1000037); //chi^+_2
  }

  if (m_LSPs_PDG.empty()) {
    m_LSPs_PDG.push_back(1000022); //chi^0_1
    //m_LSPs_PDG.push_back(1000039); the gravitino
  }
  return StatusCode::SUCCESS;
}


StatusCode DecayModeFilter::filterFinalize() {
  int nEvents = m_nPass + m_nFail;
  ATH_MSG_INFO("Single sparticle decays::  % direct decays: " << float(m_nDirectDecays)/(2*nEvents)*100
               << " % bosonic decays: " << float(m_nBosonicDecays)/(2*nEvents)*100
               << " % leptonic decays: " << float(m_nLeptonicDecays)/(2*nEvents)*100 << " %"
	       << " % dirct 3-body decay: " << float(m_nDirect3bodyDecays)/(2*nEvents)*100 << " %");
  ATH_MSG_INFO("DecayMode summary:   Direct = " << float(m_nDirectModeEvents)/nEvents*100 << "%, "
               << " Bosonic = " << float(m_nBosonicModeEvents)/nEvents*100 << "%, "
               << " Leptonic = " << float(m_nLeptonicModeEvents)/nEvents*100 << "%"
	       << " Direct3body = " << float(m_nDirect3bodyModeEvents)/nEvents*100 << "%");

  int totalFound = m_nDirectModeEvents + m_nBosonicModeEvents + m_nLeptonicModeEvents;
  ATH_MSG_INFO("Total found = " << float(totalFound)/nEvents*100 << "%");
  ATH_MSG_INFO("Other Modes:  "
               << " Double-Bosonic=" << float(m_nDoubleBosonicMode)/nEvents*100 << "%,"
               << " Double-Leptonic=" <<  float(m_nDoubleLeptonicMode)/nEvents*100 << "%,"
               << " Mixed-Bosonic-Leptonic=" << float(m_nBosonicLeptonicMode)/nEvents*100 << "%");
  ATH_MSG_INFO("Longer decay modes = " << (1 - float(totalFound + m_nDoubleBosonicMode + m_nDoubleLeptonicMode + m_nBosonicLeptonicMode)/nEvents)*100 << "%" << endmsg);

  return StatusCode::SUCCESS;
}


StatusCode DecayModeFilter::filterEvent() {
  setFilterPassed(false);
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* theGenEvent = *(events()->begin());
#ifdef HEPMC3
    auto  vtx_begin = theGenEvent->vertices().begin();
    auto  vtx_end = theGenEvent->vertices().end();
#else
    HepMC::GenEvent::vertex_const_iterator vtx_begin = theGenEvent->vertices_begin();
    HepMC::GenEvent::vertex_const_iterator vtx_end = theGenEvent->vertices_end();
#endif
    for (auto vtx_iter=vtx_begin; vtx_iter!=vtx_end; ++vtx_iter) {
      // Look for initial vertex
#ifdef HEPMC3
      if ((*vtx_iter)->particles_in().size() != 2) continue;
      if ((*vtx_iter)->particles_out().size() < 2) continue;
      auto outParticle = (*vtx_iter)->particles_out().begin();
#else
      if ((*vtx_iter)->particles_in_size() != 2) continue;
      if ((*vtx_iter)->particles_out_size() < 2) continue;
      auto outParticle = (*vtx_iter)->particles_out_const_begin();
#endif
      auto parent1 = *outParticle;
      auto parent2 = *(++outParticle);
      ATH_MSG_DEBUG("Two in, two out: "  << parent1->pdg_id() << " " << parent2->pdg_id());

      bool parent1ok(false), parent2ok(false);
      for (size_t i = 0; i < m_producedParticles_PDG.size(); ++i) {
        if (std::abs(parent1->pdg_id()) == (int) m_producedParticles_PDG[i]) parent1ok = true;
        if (std::abs(parent2->pdg_id()) == (int) m_producedParticles_PDG[i]) parent2ok = true;
      }

      if (!(parent1ok && parent2ok)) continue;
      ATH_MSG_DEBUG("Both parents ok ");
      //printChain(parent1); printChain(parent2);
      m_selected = false;

      bool isDirect1(false), isBosonic1(false), isLeptonic1(false), isDirect3body1(false);
      analyzeChain(parent1, isDirect1, isBosonic1, isLeptonic1, isDirect3body1);

      bool isDirect2(false), isBosonic2(false), isLeptonic2(false), isDirect3body2(false);
      analyzeChain(parent2, isDirect2, isBosonic2, isLeptonic2, isDirect3body2);

      bool isDirectMode( isDirect1 && isDirect2 );
      bool isBosonicMode( (isDirect1 && isBosonic2) || (isDirect2 && isBosonic1) );
      bool isLeptonicMode( (isDirect1 && isLeptonic2) || (isDirect2 && isLeptonic1) );
      bool isDirect3bodyMode( (!isDirect3body1 && isDirect3body2) || (!isDirect3body2 && isDirect3body1) );
      bool isAsymmDirectMode( (!isDirect1 && isDirect2) || (!isDirect2 && isDirect1) );

      // Sanity checks
      if ((isDirect1 && isBosonic1) || (isDirect1 && isLeptonic1) || (isBosonic1 && isLeptonic1))
        ATH_MSG_ERROR("ERROR: overlaping decay modes found!!");
      if ((isDirect2 && isBosonic2) || (isDirect2 && isLeptonic2) || (isBosonic2 && isLeptonic2))
        ATH_MSG_ERROR("ERROR: overlaping decay modes found!!");
      if (isDirectMode && isBosonicMode) ATH_MSG_ERROR("ERROR: direct and bosonic mode found.");
      if (isDirectMode && isLeptonicMode) ATH_MSG_ERROR("ERROR: direct and leptonic mode found.");
      if (isBosonicMode && isLeptonicMode) ATH_MSG_ERROR("ERROR: bosonic and leptonic mode found.");

      // Eigenmode counters
      if (isDirectMode) m_nDirectModeEvents++;
      if (isBosonicMode) m_nBosonicModeEvents++;
      if (isLeptonicMode) m_nLeptonicModeEvents++;
      if (isDirect3bodyMode) m_nDirect3bodyModeEvents++;

      // Filters
      if (m_modeID == "A" && isDirectMode) setFilterPassed(true);
      if (m_modeID == "B" && isBosonicMode && m_selected) setFilterPassed(true);
      if (m_modeID == "C" && isLeptonicMode && m_selected) setFilterPassed(true);
      if (m_modeID == "D" && isDirect3bodyMode) setFilterPassed(true);
      if (m_modeID == "E" && isAsymmDirectMode) setFilterPassed(true);

      // Other decay-modes
      if (isBosonic1 && isBosonic2) m_nDoubleBosonicMode++;
      if (isLeptonic1 && isLeptonic2) m_nDoubleLeptonicMode++;
      if ((isBosonic1 && isLeptonic2) || (isBosonic2 && isLeptonic1)) m_nBosonicLeptonicMode++;

      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::SUCCESS;
}


string DecayModeFilter::printChain(HepMC::ConstGenParticlePtr parent) const {
  if (!parent) return std::string("");
  std::stringstream ss;
  ss << " " << std::abs(parent->pdg_id()) << " ->  ";
  HepMC::ConstGenParticlePtr foundChild=nullptr;
  int SMchild_PDG(0);
  if (!parent->end_vertex()) return ss.str();
  for ( auto  child: *(parent->end_vertex())) {
    if (std::abs(child->pdg_id()) < 1000) SMchild_PDG = std::abs(child->pdg_id());
    else foundChild = child;
  }
  ss << (foundChild ? std::abs(foundChild->pdg_id()) : -9999) << " (" << SMchild_PDG << ") ";
  if (foundChild && abs(foundChild->pdg_id()) != 1000022) ss << printChain(foundChild);
  return ss.str();
}

 
void DecayModeFilter::analyzeChain(HepMC::ConstGenParticlePtr parent, bool& isDirect, bool& isBosonic, bool& isLeptonic, bool& isDirect3body) {
  int length(0), Nchi2(0), NW(0), NZ(0), NH(0), Nse(0), Nsmu(0), Nstau(0), nChargedLeptons(0), nSMParticles(0);
  countChain(parent, length, Nchi2, NW, NZ, NH, Nse, Nsmu, Nstau, nChargedLeptons, nSMParticles);

  if (length==0) ATH_MSG_WARNING("ERROR in countChain, length==0 !");

  if (length==1 && Nchi2==0) {
    isDirect=true;
    m_nDirectDecays++;
    ATH_MSG_DEBUG(" isDirect.");
    if( nSMParticles==2 ){
      isDirect3body=true;
      m_nDirect3bodyDecays++;
      ATH_MSG_DEBUG(" isDirect3body.");
    }      
  }
  if (length==2 && Nchi2==1 && Nse+Nsmu+Nstau==0) {
    isBosonic=true;
    m_nBosonicDecays++;
    ATH_MSG_DEBUG(" isBosonic.");
    if (m_submode=="W" && NW==1) m_selected = true;
    if (m_submode=="Z" && NZ==1) m_selected = true;
    if (m_submode=="H" && NH==1) m_selected = true;
    ATH_MSG_DEBUG(" m_selected: " << m_selected);
  }
  if (length==3 && Nchi2==1 && Nse+Nsmu+Nstau==1) {
    isLeptonic=true;
    m_nLeptonicDecays++;
    ATH_MSG_DEBUG(" isLeptonic.");
    if (m_submode=="se" && Nse==1 && m_nChargedLeptons==nChargedLeptons) m_selected=true;
    if (m_submode=="smu" && Nsmu==1 && m_nChargedLeptons==nChargedLeptons) m_selected=true;
    if (m_submode=="stau" && Nstau==1 && m_nChargedLeptons==nChargedLeptons) m_selected=true;
    ATH_MSG_DEBUG(" m_selected: " << m_selected);
  }
}


void DecayModeFilter::countChain(HepMC::ConstGenParticlePtr parent, int& length,
                                 int& Nchi2, int& NW,int& NZ,int& NH, int& Nse, int& Nsmu, int& Nstau, int& nChargedLeptons, int& nSMParticles) const {
  HepMC::ConstGenParticlePtr foundChild=nullptr;
  int SMchild_PDG(0);
  if (!parent) return;
  if (parent->end_vertex())
  {
  for (auto child: *(parent->end_vertex())) {
    if (std::abs(child->pdg_id()) < 1000){
      SMchild_PDG = std::abs(child->pdg_id());
      nSMParticles++;
    }
    if (std::abs(child->pdg_id()) > 1000000) foundChild = child;
  }
  }
  if (!foundChild) {
    length = 0;
    return;
  }
  countPIDs(std::abs(foundChild->pdg_id()),SMchild_PDG, Nchi2, NW,NZ,NH, Nse,Nsmu,Nstau,nChargedLeptons);
  if (!isIn(m_producedParticles_PDG,abs(foundChild->pdg_id()))) length += 1; //allow serveral strong-sparticle steps
  if (!isIn( m_LSPs_PDG, std::abs(foundChild->pdg_id()))) {
    countChain(foundChild,length, Nchi2, NW,NZ,NH, Nse,Nsmu,Nstau,nChargedLeptons,nSMParticles);
  }
}


void DecayModeFilter::countPIDs(int sparticle_PDG, int SM_PDG, int& Nchi2, int& NW, int& NZ, int& NH,
                                int& Nse, int& Nsmu, int& Nstau, int& nChargedLeptons) const {
  if (isIn(m_chi2s_PDG,sparticle_PDG)) Nchi2+=1;
  //
  if (SM_PDG==24) NW+=1;
  if (SM_PDG==23) NZ+=1;
  if (SM_PDG==25 || SM_PDG==35 || SM_PDG==36 || SM_PDG==37) NH+=1;
  /////
  if (sparticle_PDG==1000011 || sparticle_PDG==2000011 || sparticle_PDG==1000012) Nse+=1; //e_L, e_R, nu_e
  if (sparticle_PDG==1000013 || sparticle_PDG==2000013 || sparticle_PDG==1000014) Nsmu+=1; //mu_L, mu_R, nu_mu
  if (sparticle_PDG==1000015 || sparticle_PDG==2000015 || sparticle_PDG==1000016) Nstau+=1; //tau_L, tau_R, nu_tau
  //
  if (SM_PDG==11 || SM_PDG==13 || SM_PDG==15) nChargedLeptons += 1;
}
