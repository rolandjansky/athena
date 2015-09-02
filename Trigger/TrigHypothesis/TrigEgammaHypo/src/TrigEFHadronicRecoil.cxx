/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigEFHadronicRecoil.cxx
*/
#include "TrigEgammaHypo/TrigEFHadronicRecoil.h"
#include "TrigNavigation/Navigation.h"
#include "xAODEgamma/ElectronContainer.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigSteeringEvent/TrigPassBits.h"

using namespace std;


TrigEFHadronicRecoil::TrigEFHadronicRecoil(const std::string& name, 
						   ISvcLocator* svcloc) : 
  HLT::AllTEAlgo(name, svcloc) {
  declareProperty("RecoilCut", mRecoilCut=25.0E+3, 
		  "Maximum value of the hadronic recoil (MeV)");
}

TrigEFHadronicRecoil::~TrigEFHadronicRecoil() {
}

HLT::ErrorCode TrigEFHadronicRecoil::hltInitialize() {
  return HLT::OK;
}

HLT::ErrorCode 
TrigEFHadronicRecoil::hltExecute(std::vector<HLT::TEVec>& inputs, 
				     unsigned int type_out) {
  HLT::TriggerElement* outputTE = makeOutputTE(inputs, type_out);
  if (outputTE && inputs.size() != 2) {
    // Cannot calculate the hadronic recoil and attach it
    msg() << MSG::WARNING << "OutputTE is null or input TE size not two"
	  << endreq;
    return HLT::OK;
  }

  outputTE->setActiveState(false);

  HLT::TEVec& tes1 = inputs[0]; // 1st TE must be MET
  HLT::TEVec& tes2 = inputs[1]; // 2nd TE must be electrons

  const TrigMissingET* met(0);
  std::vector<float> v_recoil;
  bool pass(false);

  if (tes1.size() == 1) {
    HLT::TriggerElement* te_met = tes1[0];
    std::vector<const TrigMissingET*> v_met;
    HLT::ErrorCode status = getFeatures(te_met, v_met);
    if (status == HLT::OK) {
      if (v_met.size() > 0) met = v_met[0];
    }
  } else {
    // There should be exactly one MET TE
  }
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "MET: " << met << endreq;
  }
  if (tes2.size() > 0) {
    const TrigPassBits* bits(0);
    HLT::TEVec::const_iterator p;
    const xAOD::ElectronContainer* cont=0;
    xAOD::ElectronContainer::const_iterator pe;
    for (p=tes2.begin(); p!=tes2.end(); ++p) {
      HLT::ErrorCode status = getFeature(*p, cont);
      if (status != HLT::OK || cont == 0) continue;
      status = getFeature(*p, bits, "passbits");
      if (status != HLT::OK || cont == 0) continue;

      //Need to fix for xADO!!!!!!!!!!!!!!!!!!!!!!
     /* unsigned int i=0;
      for (pe=cont->begin(); pe!=cont->end(); ++pe, ++i) {
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Checking hadronic recoil with electron" << endreq;
	}
	if ( (*pe) != 0 && 
	     (bits == 0 || (i<bits->size() && bits->isPassing(i) ) ) ) {
	  float px = (*pe)->px();
	  float py = (*pe)->py();
	  float recoil_px = px + met->ex();
	  float recoil_py = py + met->ey();
	  float recoil = sqrt(recoil_px*recoil_px + recoil_py*recoil_py);
	  if (msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Electron passed the selection, recoil=" << recoil << endreq;
	  }
	  if (recoil < mRecoilCut) pass = true;
	  //	  v_recoil.push_back(recoil);
	}
      }*/
    }
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Decision: " << pass << endreq;
  }
  if (pass) outputTE->setActiveState(true);

  return HLT::OK;
}

HLT::ErrorCode TrigEFHadronicRecoil::hltFinalize() {
  return HLT::OK;
}

HLT::TriggerElement* 
TrigEFHadronicRecoil::makeOutputTE(std::vector<HLT::TEVec>& inputs, 
				       unsigned int type_out) {
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

