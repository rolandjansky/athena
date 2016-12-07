/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFMtAllTE.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   Arantxa Ruiz Martinez
 * CREATED:  2 June 2015
 *
 ********************************************************************/

#include "TrigEgammaHypo/TrigEFMtAllTE.h"
#include "TrigNavigation/Navigation.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTrigger/TrigPassBits.h"
using namespace std;

struct DescendingEt:std::binary_function<const xAOD::Electron*,
                                         const xAOD::Electron*,
					 bool> {
  bool operator()(const xAOD::Electron* l, const xAOD::Electron* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};

TrigEFMtAllTE::TrigEFMtAllTE(const std::string& name, ISvcLocator* svcloc) : 
  HLT::AllTEAlgo(name, svcloc)
{
  declareProperty("MinMtCut",       m_MinMtCut       = 20*CLHEP::GeV);
  declareProperty("MaxNbElectrons", m_MaxNbElectrons = 10);
  declareProperty("MinElectronEt",  m_MinElectronEt  = 10*CLHEP::GeV);

  declareMonitoredVariable("mt_electron1",      m_mt_electron1);
  declareMonitoredVariable("mt_electron1_pass", m_mt_electron1_pass);
}

TrigEFMtAllTE::~TrigEFMtAllTE() {
}

HLT::ErrorCode TrigEFMtAllTE::hltInitialize() {
  return HLT::OK;
}

HLT::ErrorCode TrigEFMtAllTE::hltExecute(std::vector<HLT::TEVec>& inputs, unsigned int type_out) {

  HLT::TriggerElement* outputTE = makeOutputTE(inputs, type_out);
  if (outputTE && inputs.size() != 2) {
    msg() << MSG::WARNING << "OutputTE is null or input TE size not two"
	  << endmsg;
    return HLT::OK;
  }

  outputTE->setActiveState(false);

  HLT::TEVec& tes1 = inputs[0]; // 1st TE must be MET
  HLT::TEVec& tes2 = inputs[1]; // 2nd TE must be electrons

  bool pass(false);
  int electron_counter = 0;
  m_mt_electron1 = -999;
  m_mt_electron1_pass = -999;

  if(tes1.size() == 0) {
    msg() << MSG::WARNING << "No MET TE found" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  if(tes2.size() == 0) {
    msg() << MSG::WARNING << "No jet TE found" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // MET

  std::vector<const xAOD::TrigMissingET*> v_met;
  HLT::ErrorCode statMET = getFeatures(tes1.front(), v_met);
  if(statMET != HLT::OK) {
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endmsg;
    return HLT::OK;
  }
  if(v_met.size() == 0){
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endmsg;
    return HLT::MISSING_FEATURE;
  }

  const xAOD::TrigMissingET* efmet = v_met.front();
  float met = sqrt(efmet->ex()*efmet->ex()+efmet->ey()*efmet->ey());
  float metphi = atan2f(efmet->ey(),efmet->ex());

  // Electron

  const xAOD::ElectronContainer* outEle=0;
  HLT::ErrorCode statEles = getFeature(tes2.front(), outEle);
  if(statEles != HLT::OK) {
    msg() << MSG::WARNING << " Failed to get Electrons " << endmsg;
    return HLT::OK;
  }
  if(outEle == 0){
    msg() << MSG::DEBUG << " Got no Electrons associated to the TE! " << endmsg;
    return HLT::MISSING_FEATURE;
  }
  const xAOD::TrigPassBits* bits(0);
  HLT::ErrorCode status = getFeature(tes2.front(), bits, "passbits");
  if (status != HLT::OK) {
    msg() << MSG::WARNING << " Failed to get TrigPassBits " << endmsg;
    return HLT::MISSING_FEATURE;
  }

  std::vector<const xAOD::Electron*> theElectrons(outEle->begin(), outEle->end());
  std::sort (theElectrons.begin(), theElectrons.end(), DescendingEt());

  for (const xAOD::Electron* aEle : theElectrons) {

    if(!bits->isPassing( aEle, outEle )) {
        if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Electron found not passing Hypo object" << endmsg;
      }
      continue;
    }

    float elephi = aEle->p4().Phi();
    float elept = aEle->p4().Pt();

    double delta_phi = fabs(elephi - metphi);
    if (delta_phi>M_PI) delta_phi = 2*M_PI - delta_phi;

    float mt = sqrt(2*elept*met*(1-cos(delta_phi)));

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Electron pt " << elept << " MeV, MT: " << mt << " MeV, MET: " << met << " MeV" << endmsg;
      msg() << MSG::DEBUG << "Electron pt cut " << m_MinElectronEt*CLHEP::GeV << " MeV, MT cut: " << m_MinMtCut*CLHEP::GeV << " MeV" << endmsg;
    }

    if(electron_counter == 0) {
      m_mt_electron1 = mt;
    }

    if(electron_counter < m_MaxNbElectrons) {
      if(elept > m_MinElectronEt*CLHEP::GeV && mt > m_MinMtCut*CLHEP::GeV) {
	pass = true;
	if(electron_counter == 0) {
	  m_mt_electron1_pass = mt;
	}
      }
    }

    electron_counter++;
    if (electron_counter >= m_MaxNbElectrons) break;

  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Decision: " << pass << endmsg;
  }
  if (pass) outputTE->setActiveState(true);

  return HLT::OK;
}

HLT::ErrorCode TrigEFMtAllTE::hltFinalize() {
  return HLT::OK;
}

HLT::TriggerElement* TrigEFMtAllTE::makeOutputTE(std::vector<HLT::TEVec>& inputs, unsigned int type_out) {

  HLT::TEVec allTEs;
  if (inputs.size() == 2) {
    HLT::TEVec& tes1 = inputs[0];
    HLT::TEVec& tes2 = inputs[1];
    copy(tes1.begin(), tes1.end(), back_inserter(allTEs));
    copy(tes2.begin(), tes2.end(), back_inserter(allTEs));
  }

  HLT::Navigation* navigation = config()->getNavigation();
  HLT::TriggerElement* outputTE = navigation->addNode(allTEs, type_out);

  return outputTE;
}
