/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypoUnitTests/RandomSignalHypoJetVectorGenerator.h"
#include "../src/ITLorentzVectorFactory.h"
#include "../src/TLorentzVectorFactoryEtaE.h"
#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"

#include <vector>
#include <sstream>

class TLorentzVectorAsIJet;

RandomSignalHypoJetVectorGenerator::RandomSignalHypoJetVectorGenerator(const std::string& type,
								       const std::string& name,
								       const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode
RandomSignalHypoJetVectorGenerator::initialize() {
  if(m_n_sig_lo >= m_n_sig_hi ||
     m_e_sig_lo >= m_e_sig_hi
     ){
    ATH_MSG_ERROR("Config error: lower signal limit exceeds uper limit");
    return StatusCode::FAILURE;
  }

  m_randNSig = RandUint(m_n_sig_lo, m_n_sig_hi);
  m_flip = RandUint(0, 1);

  m_randEtaSig = RandDouble(-m_eta_sig_hi, m_eta_sig_hi);
  m_randEnSig = RandDouble(m_e_sig_lo, m_e_sig_hi);

  m_randEtaBkd =  RandDouble(-m_bkgd_etamax, m_bkgd_etamax);
  m_randEnBkd = RandDouble(5., m_bkgd_etmax);
 
  return StatusCode::SUCCESS;  
}

std::string RandomSignalHypoJetVectorGenerator::toString() const {
  std::stringstream ss;
  ss << name() <<":: "
     << m_n_sig_lo << " "
     << m_n_sig_hi << " "
     << m_e_sig_lo << " "
     << m_e_sig_hi << " "
     << m_eta_sig_hi << " ";
    
  ss << m_nbkgd << " "
     << m_bkgd_etmax << " "
     << m_bkgd_etamax << '\n';

  return ss.str();
}


HypoJetVector
RandomSignalHypoJetVectorGenerator::get() {

  auto factory = std::unique_ptr<ITLorentzVectorFactory>(nullptr);
  if (m_useEtaEtNotEtaE){
    factory.reset(new TLorentzVectorFactory());
  } else {
    factory.reset(new TLorentzVectorFactoryEtaE());
  }
 
  HypoJetVector result;

  auto nj =  m_randNSig();
  for(std::size_t ij = 0; ij  < nj;  ++ij){
    const auto& eta = m_randEtaSig();
    const auto& en = m_randEnSig();
    // depending on the factory, en is e or et
    auto tlv = factory->make(eta, en);
    if(m_flip()){
      tlv.SetZ(-tlv.Z()); // randomly flip the sign of the +vly generated tlv.
    }
    result.push_back(std::shared_ptr<const HypoJet::IJet>(new TLorentzVectorAsIJet(tlv)));
  }

  if(m_nbkgd){
    for(int i = 0; i < m_nbkgd; ++i){
      const auto& eta = m_randEtaBkd();
      const auto& en = m_randEnBkd();
      // depending on the factory, en is e or et
      auto tlv = factory->make(eta, en);
      if(m_flip()){
	tlv.SetZ(-tlv.Z()); // randomly flip the sign of the +vly generated tlv.
      }  
      result.push_back(std::shared_ptr<const HypoJet::IJet>(new TLorentzVectorAsIJet(tlv)));
    }
  }

  std::shuffle(result.begin(), result.end(), m_shuffle_re);
  return result;
}
