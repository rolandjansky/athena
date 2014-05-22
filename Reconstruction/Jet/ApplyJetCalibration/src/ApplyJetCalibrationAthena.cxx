/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUtils/JetSignalStateHelper.h"
#include "ApplyJetCalibration/ApplyJetCalibrationAthena.h"
#include "AthenaKernel/errorcheck.h"
#include "VxVertex/VxContainer.h"
#include "PathResolver/PathResolver.h"

#include "EventInfo/EventInfo.h"

ApplyJetCalibrationAthena::ApplyJetCalibrationAthena( const std::string& s1, const std::string& s2, const IInterface* i ) :
  JetCalibratorAthena(s1,s2,i)
{
  declareProperty("AlgoType", m_algoType);
  declareProperty("ConfigFile", m_configfile);
  declareProperty("RhoKey", m_rhoKey="ClusterRhoKt4EM");
  declareProperty("IsData", m_isData);
  declareProperty("OnlyOffsetSubstraction", m_onlyOffset=false);

}

StatusCode ApplyJetCalibrationAthena::initialize() {
  
  std::string fullpath = PathResolverFindXMLFile(m_configfile);

  ATH_MSG_INFO(" config file is "<< fullpath);
  
  m_applyJES = new JetAnalysisCalib::JetCalibrationTool(m_algoType.c_str(),fullpath.c_str(), m_isData);

  return StatusCode::SUCCESS;
}

StatusCode ApplyJetCalibrationAthena::initializeEvent()  {
  m_rho = 0;
  StatusCode sc = userStore()->retrieve( m_rhoKey, m_rho);
  if (sc.isFailure() ) ATH_MSG_WARNING(" Rho is 0 for "<< m_rhoKey);

  const VxContainer * vtxCont = 0;
  sc = evtStore()->retrieve(vtxCont, "VxPrimaryCandidate") ;
  if (sc.isFailure() ) ATH_MSG_WARNING(" no Primary Vertex ");
  else {
    m_npv = 0;
    for (VxContainer::const_iterator vtxIter(vtxCont->begin()); 
         vtxIter != vtxCont->end(); ++vtxIter) { 
      if((*vtxIter)->vxTrackAtVertex()->size() >= 2)  m_npv++;
    }
  }
  ATH_MSG_DEBUG( " initializeEvent : rho="<<m_rho <<"  npv="<<m_npv);
  const EventInfo* eventInfo;
  CHECK( evtStore()->retrieve( eventInfo ) );
  m_mu = eventInfo->averageInteractionsPerCrossing();

  return StatusCode::SUCCESS;
}


bool ApplyJetCalibrationAthena::compute_corrections(const Jet* jet_in ){


  JetSignalStateHelper sh(jet_in, P4SignalState::JETCONSTITUENTSCALE);
  double Eraw    = jet_in->e();
  double eta     = jet_in->eta();
  double phi     = jet_in->phi();
  double m       = jet_in->m();
  double Ax      = jet_in->getMoment("ActiveAreaPx");
  double Ay      = jet_in->getMoment("ActiveAreaPy");
  double Az      = jet_in->getMoment("ActiveAreaPz");
  double Ae      = jet_in->getMoment("ActiveAreaE");


  
  TLorentzVector jet;  

  if(m_onlyOffset) jet= m_applyJES->ApplyJetAreaOffset(Eraw,eta,phi,m,Ax,Ay,Az,Ae,m_rho,m_mu,m_npv);
  else{
    // intermediary step to compute the jes factor
    jet= m_applyJES->ApplyJetAreaOffsetOrigin(Eraw,eta,phi,eta,phi,m,Ax,Ay,Az,Ae,m_rho,m_mu,m_npv);
    double jes =  m_applyJES->GetJES(jet.E(),eta);
    jet_in->setMoment("JES",jes);
    jet= m_applyJES->ApplyJetAreaOffsetEtaJES(Eraw,eta,phi,m,Ax,Ay,Az,Ae,m_rho,m_mu,m_npv);
  }

  ATH_MSG_DEBUG("rho,mu,npv"<< m_rho<< ","<< m_mu<< ","<<m_npv<< " | jet at ("<< eta << ","<<phi<<") "<<Eraw << " Ax="<<Ax<< " Ae="<<Ae << " | cal E="<< jet.E()<<"  Eta="<<jet.Eta());
  m_corrections.clear();
  m_corrections.push_back(jet.Px());
  m_corrections.push_back(jet.Py());
  m_corrections.push_back(jet.Pz());
  m_corrections.push_back(jet.E());

  return true;

}

bool ApplyJetCalibrationAthena::correct_4mom( Jet* jet_in ){
  
  Jet::hlv_t hlv(m_corrections[0],m_corrections[1],m_corrections[2],m_corrections[3]);
  jet_in->set4Mom(hlv);
  return true;
}
