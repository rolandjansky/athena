/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Based on DerivationFramework::SkimmingToolExample

#include "DerivationFrameworkExotics/SkimmingToolEXOT20.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"

// Constructor
DerivationFramework::SkimmingToolEXOT20::SkimmingToolEXOT20( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_trigDecisionTool ("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_extrapolator ("Trk::Extrapolator/AtlasExtrapolator")
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("MSTPContainer",       m_MSTPSGKey = "MuonSpectrometerTrackParticles");
    declareProperty("Triggers",            m_triggers = std::vector<std::string>());
    declareProperty("MinPtMSTP",           m_MinPtMSTP = 3.0);
    declareProperty("MinNumMSTP",          m_MinNumMSTP = 2);
    declareProperty("StartDirection",      m_direction = -1);
    declareProperty("ParticleType",        m_particleType = 2);
  }
  
// Destructor
DerivationFramework::SkimmingToolEXOT20::~SkimmingToolEXOT20() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolEXOT20::initialize(){

  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }

  if (m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_extrapolator);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("initialize() ...");
  return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::SkimmingToolEXOT20::finalize()
{
     ATH_MSG_INFO("finalize() ...");
     ATH_MSG_INFO("Processed " << n_tot << " events, " << n_pass << " events passed filter ");

     ATH_MSG_INFO("Trigger   :: " << n_passTrigger);
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolEXOT20::eventPassesFilter() const
{

  n_tot++;

  if (!SubcutTrigger()) return false;
  if (!SubcutSelect()) return false;
 
  n_pass++;
  return true;

}

bool DerivationFramework::SkimmingToolEXOT20::SubcutTrigger() const {

  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo));

  e_passTrigger = false;

  for (unsigned int i=0; i < m_triggers.size(); i++) {
    bool thisTrig = m_trigDecisionTool->isPassed(m_triggers.at(i));
    eventInfo->auxdecor< bool >(TriggerVarName(m_triggers.at(i))) = thisTrig;
    e_passTrigger |= thisTrig;
  }

  if (e_passTrigger) n_passTrigger++;
  return e_passTrigger;

}

bool DerivationFramework::SkimmingToolEXOT20::SubcutSelect() const {

  const xAOD::TrackParticleContainer* MSTP(0);
  ATH_CHECK(evtStore()->retrieve(MSTP,m_MSTPSGKey));
  xAOD::TrackParticleContainer::const_iterator mu_itr(MSTP->begin());
  xAOD::TrackParticleContainer::const_iterator mu_end(MSTP->end());

  int goodMSTPs(0);
  for (; mu_itr != mu_end; ++mu_itr) {    
    if ((*mu_itr)->pt()*0.001 > m_MinPtMSTP) goodMSTPs++;
  }
  if (goodMSTPs < m_MinNumMSTP) return false;

  mu_itr = MSTP->begin();
  for (; mu_itr != mu_end; ++mu_itr) {

    Amg::Vector3D refPos(0.1,0.1,0.1);
    Trk::PerigeeSurface persurf(refPos);

    Trk::PropDirection propagationDirection = m_direction > 0 ? Trk::alongMomentum : Trk::oppositeMomentum;

    const Trk::TrackParameters* entryPars = m_extrapolator->extrapolate((*mu_itr)->perigeeParameters(),
                                                                persurf,
                                                                propagationDirection,
                                                                false,
                                                                (Trk::ParticleHypothesis)m_particleType);

    if (entryPars) {
      const AmgVector(5)& tmpParam = entryPars->parameters();
      SG::AuxElement::Decorator< double > d0Dec("d0IP");
      d0Dec(**mu_itr) = tmpParam(Trk::d0);
      SG::AuxElement::Decorator< double > z0Dec("z0IP");
      z0Dec(**mu_itr) = tmpParam(Trk::z0);
      SG::AuxElement::Decorator< double > phiDec("phiIP");
      phiDec(**mu_itr) = tmpParam(Trk::phi);
      SG::AuxElement::Decorator< double > thetaDec("thetaIP");
      thetaDec(**mu_itr) = tmpParam(Trk::theta);
      SG::AuxElement::Decorator< double > qOverPDec("qOverPIP");
      qOverPDec(**mu_itr) = tmpParam(Trk::qOverP);

      const AmgSymMatrix(5) * covP = entryPars->covariance();
      if (covP){
        const AmgSymMatrix(5) cov = *covP;
        SG::AuxElement::Decorator< double > d0CovDec("d0CovIP");
        d0CovDec(**mu_itr) = cov(Trk::d0,Trk::d0);
        SG::AuxElement::Decorator< double > z0CovDec("z0CovIP");
        z0CovDec(**mu_itr) = cov(Trk::z0,Trk::z0);
        SG::AuxElement::Decorator< double > phiCovDec("phiCovIP");
        phiCovDec(**mu_itr) = cov(Trk::phi,Trk::phi);
        SG::AuxElement::Decorator< double > thetaCovDec("thetaCovIP");
        thetaCovDec(**mu_itr) = cov(Trk::theta,Trk::theta);
        SG::AuxElement::Decorator< double > qOverPCovDec("qOverPCovIP");
        qOverPCovDec(**mu_itr) = cov(Trk::qOverP,Trk::qOverP);
      }
    }//entryPars exist
  }//muonLoop

  return true;  
}

std::string DerivationFramework::SkimmingToolEXOT20::TriggerVarName(std::string s) const {
  std::replace(s.begin(), s.end(), '-', '_'); return s;
}
