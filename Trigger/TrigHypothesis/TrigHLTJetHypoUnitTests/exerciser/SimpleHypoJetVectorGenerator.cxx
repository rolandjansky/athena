/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypoUnitTests/SimpleHypoJetVectorGenerator.h"
#include "../src/ITLorentzVectorFactory.h"
#include "../src/TLorentzVectorFactoryEtaE.h"
#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"

#include <vector>
#include <random>
#include <sstream>

class TLorentzVectorAsIJet;

SimpleHypoJetVectorGenerator::SimpleHypoJetVectorGenerator(const std::string& type,
							   const std::string& name,
							   const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode
SimpleHypoJetVectorGenerator::initialize() {
  if(m_es.size() != m_etas.size()){
    ATH_MSG_ERROR("Config error: no of Et values != no of Eta values"
		  << m_es.size() << " " << m_etas.size());
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
  
}

std::string SimpleHypoJetVectorGenerator::toString() const {
  std::stringstream ss;
  ss << name() <<":: e: ";
  for(const auto& e : m_es){ss << e <<  ", ";}
  ss << " eta:  ";
  for(const auto& e : m_etas){ss << e << ", ";}
  ss<<" bkgd: "<< m_nbkgd << " max E" << m_bkgd_etmax << " max abs eta: "
    << m_bkgd_etamax << '\n';

  return ss.str();
}


HypoJetVector
SimpleHypoJetVectorGenerator::get() {

  auto factory = std::unique_ptr<ITLorentzVectorFactory>(nullptr);
  if (m_useEtaEtNotEtaE){
    factory.reset(new TLorentzVectorFactory());
  } else {
    factory.reset(new TLorentzVectorFactoryEtaE());
  }
 
  HypoJetVector result;
  
  for(std::size_t i = 0; i < m_es.size(); ++i){
    const auto& eta = m_etas[i];
    const auto& et = m_es[i];
    auto tlv = factory->make(eta, et);
    result.push_back(std::shared_ptr<const HypoJet::IJet>(new TLorentzVectorAsIJet(tlv)));
  }
  
  if(m_nbkgd){
    std::default_random_engine generator;
    std::uniform_real_distribution<double> eta_distribution(-m_bkgd_etamax,
							    m_bkgd_etamax);
    
    std::uniform_real_distribution<double> en_distribution(5.,
							   m_bkgd_etmax);

    
    
    for(int i = 0; i < m_nbkgd; ++i){
      const auto& eta = eta_distribution(generator);
      const auto& en = en_distribution(generator);
      // depending on the factory, en is e or et
      auto tlv = factory->make(eta, en);
      result.push_back(std::shared_ptr<const HypoJet::IJet>(new TLorentzVectorAsIJet(tlv)));
    }
  }

  std::shuffle(result.begin(), result.end(), m_shuffle_re);
  return result;
}
