/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ApplyJetCalibrationAthena.h"
#include "AthenaKernel/errorcheck.h"
#include "PathResolver/PathResolver.h"
#include "xAODTracking/VertexContainer.h"

#include "EventInfo/EventInfo.h"

ApplyJetCalibrationAthena::ApplyJetCalibrationAthena( const std::string& n ) :   asg::AsgTool(n)
{
  declareProperty("AlgoType", m_algoType);
  declareProperty("ConfigFile", m_configfile);
  declareProperty("RhoKey", m_rhoKey="ClusterRhoKt4EM");
  declareProperty("IsData", m_isData);
  declareProperty("OnlyOffsetSubstraction", m_onlyOffset=false);
  declareProperty("VertexContainer", m_vtxContName="PrimaryVertices");

}

StatusCode ApplyJetCalibrationAthena::initialize() {
  
  std::string fullpath = PathResolverFindXMLFile(m_configfile);

  ATH_MSG_INFO(" config file is "<< fullpath);
  
  m_applyJES = new JetAnalysisCalib::JetCalibrationTool(m_algoType.c_str(),fullpath.c_str(), m_isData);

  return StatusCode::SUCCESS;
}

int ApplyJetCalibrationAthena::modify(xAOD::JetContainer& jets) const {

  double rho = 0;
  int npv=0;
  double mu=0;

  StatusCode sc = userStore()->retrieve( m_rhoKey, rho);
  if (sc.isFailure() ) ATH_MSG_WARNING(" Rho is 0 for "<< m_rhoKey);

  const xAOD::VertexContainer * vtxCont = 0;
  sc = evtStore()->retrieve(vtxCont, m_vtxContName) ;
  if (sc.isFailure() ) ATH_MSG_WARNING(" no Primary Vertex ");
  else {
    npv = 0;
    for (auto vtxIter = vtxCont->begin(); vtxIter != vtxCont->end(); ++vtxIter) { 
      if((*vtxIter)->nTrackParticles() >= 2)  npv++;
    }
  }
  ATH_MSG_DEBUG( " initializeEvent : rho="<<rho <<"  npv="<<npv);
  const EventInfo* eventInfo;
  sc = evtStore()->retrieve( eventInfo ) ;
  if(sc.isFailure() ) {ATH_MSG_ERROR("Cant retrieve EventInfo"); return 0;}
  mu = eventInfo->averageInteractionsPerCrossing();


  // Iterate over jets :
  for ( xAOD::Jet* pjet : jets ) {
    xAOD::JetFourMom_t fv = pjet->jetP4(xAOD::JetConstitScaleMomentum);
    double eraw = fv.E();
    double eta  = fv.Eta();
    double phi  = fv.Phi();
    double m    = fv.M();
    xAOD::JetFourMom_t varea = pjet->getAttribute<xAOD::JetFourMom_t>("ActiveArea4vec");
    double Ax   = varea.Px();
    double Ay   = varea.Py();
    double Az   = varea.Pz();
    double Ae   = varea.E();
    TLorentzVector jetfv;  
    if ( m_onlyOffset ) {
      jetfv = m_applyJES->ApplyJetAreaOffset(eraw,eta,phi,m,Ax,Ay,Az,Ae,rho,mu,npv);
    } else {
      // intermediary step to compute the jes factor
      jetfv = m_applyJES->ApplyJetAreaOffsetOrigin(eraw,eta,phi,eta,phi,m,Ax,Ay,Az,Ae,rho,mu,npv);
      double jes = m_applyJES->GetJES(jetfv.E(), eta);
      pjet->setAttribute<float>("JES", jes);
      jetfv = m_applyJES->ApplyJetAreaOffsetEtaJES(eraw,eta,phi,m,Ax,Ay,Az,Ae,rho,mu,npv);
    }
    ATH_MSG_DEBUG("rho,mu,npv" << rho<< "," << mu<< "," <<npv << " | jet at ("
                  << eta << "," << phi << ") " << eraw << " Ax=" << Ax << " Ae=" << Ae
                  << " | cal E=" << jetfv.E() << "  Eta=" << jetfv.Eta());
    pjet->setJetP4(xAOD::JetFourMom_t(jetfv.Pt(), jetfv.Eta(), jetfv.Phi(), jetfv.M() ) );
  }

  return 1;
}

