/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrigHLTJetRec/IParticleSelectorEtaPt.h"
#include "xAODBase/IParticle.h"

using xAOD::IParticle;

IParticleSelectorEtaPt::IParticleSelectorEtaPt(const std::string& s,
                                                 const std::string& name,
                                                 const IInterface* i):
  AthAlgTool(s, name, i){
  declareInterface<IIParticleSelector>(this);
  declareProperty("etaMax", m_etaMax, "max abs eta for acceptance");
  declareProperty("ptMin", m_ptMin, "min pt for acceptance");
}

StatusCode IParticleSelectorEtaPt::initialize() {

  ATH_MSG_INFO("etaMax: " << m_etaMax << " ptMin: " << m_ptMin << endreq);  
  return StatusCode::SUCCESS;
}


class IParticleRemover{
public:
  
  IParticleRemover(double etaMax, double ptMin):
    m_etaMax(etaMax), m_ptMin(ptMin){}

  bool operator()(const IParticle* pIp) const{

    if (std::abs(pIp->eta()) > m_etaMax){
      return true;
    }
    if (pIp->pt() < m_ptMin) {
      return true;
    }

    return false;}

private:
  double m_etaMax;
  double m_ptMin;
};

StatusCode
IParticleSelectorEtaPt::select(IParticleVector& iparticles) const {

  iparticles.erase(std::remove_if(iparticles.begin(),
                                  iparticles.end(),
                                  IParticleRemover(m_etaMax, m_ptMin)),
                   iparticles.end());
  
  return StatusCode::SUCCESS;}
  


