/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypoUnitTests/SimpleHypoJetVectorGenerator.h"
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
  if(m_ets.size() != m_etas.size()){
    ATH_MSG_ERROR("Config error: no of Et values != no of Eta values"
		  << m_ets.size() << " " << m_etas.size());
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
  
}

std::string SimpleHypoJetVectorGenerator::toString() const {
  std::stringstream ss;
  ss << name() <<":: et: ";
  for(const auto& e : m_ets){ss << e <<  ", ";}
  ss << " eta:  ";
  for(const auto& e : m_etas){ss << e << ", ";}
  ss<<" bkgd: "<< m_nbkgd << " max E" << m_bkgd_etmax << " max abs eta: "
    << m_bkgd_etamax << '\n';

  return ss.str();
}


HypoJetVector
SimpleHypoJetVectorGenerator::get() const{


  HypoJetVector result;
  // result.resize(m_ets.size() + m_nbkgd);
  auto factory = TLorentzVectorFactory();
  
  for(std::size_t i = 0; i < m_ets.size(); ++i){
    const auto& eta = m_etas[i];
    const auto& et = m_ets[i];
    auto tlv = factory.make(eta, et);
    result.push_back(new TLorentzVectorAsIJet(tlv));
  }

  if(m_nbkgd){
    std::default_random_engine generator;
    std::uniform_real_distribution<double> eta_distribution(-m_bkgd_etamax,
							    m_bkgd_etamax);
    
    std::uniform_real_distribution<double> et_distribution(0.,
							   m_bkgd_etmax);

    for(int i = 0; i < m_nbkgd; ++i){
      const auto& eta = eta_distribution(generator);
      const auto& et = et_distribution(generator);
      auto tlv = factory.make(eta, et);
      result.push_back(new TLorentzVectorAsIJet(tlv));
    }
  }

  std::random_shuffle(result.begin(), result.end());
  return result;
}
