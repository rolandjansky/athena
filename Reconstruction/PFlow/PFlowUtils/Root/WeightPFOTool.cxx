/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFlowUtils/WeightPFOTool.h"

namespace CP {

  WeightPFOTool::WeightPFOTool(const std::string& name) : asg::AsgTool( name ) { }

  StatusCode WeightPFOTool::fillWeightAndP4( const xAOD::PFO& pcpf, float& weight, TLorentzVector& cpfo_p4 ) {
    weight = 0.;

    const xAOD::TrackParticle* ptrk = pcpf.track(0);
    if ( ptrk == 0 ) {
      ATH_MSG_WARNING("Skipping charged PFO with null track pointer.");
      return StatusCode::SUCCESS;
    }

    //This weight allows us to linearly de-weight higher pt tracks as we move towards the calo only regime
    if (ptrk->pt() < 30000) {
      weight = 1.0;
    } else if (ptrk->pt() >= 30000 && ptrk->pt() <= 60000) {
      weight = (1.0 - (ptrk->pt()-30000)/30000);
    } else {
      ATH_MSG_VERBOSE("PFlow track with pt " << ptrk->pt() << " outside accepted range");
      return StatusCode::SUCCESS;
    }

    int isInDenseEnvironment = false;
    bool gotVariable = pcpf.attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment);
    if (false == gotVariable) ATH_MSG_WARNING("This charged PFO did not have eflowRec_isInDenseEnvironment set");
    ATH_MSG_VERBOSE("Track in dense environment");
	      
    float expectedEnergy = 0.0;
    gotVariable = pcpf.attribute(xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit,expectedEnergy);

    cpfo_p4 = pcpf.p4();
    if (gotVariable == false) {
      ATH_MSG_WARNING("This charged PFO did not have eflowRec_tracksExpectedEnergyDeposit set");
      weight = 1.0;
    } else{
      ATH_MSG_VERBOSE("Track expected energy: " << expectedEnergy);
      if (isInDenseEnvironment == true) {
	cpfo_p4.SetPtEtaPhiM((ptrk->e()-expectedEnergy)/cosh(pcpf.eta()),pcpf.eta(),pcpf.phi(),pcpf.m());
	ATH_MSG_VERBOSE("CPFO in dense environment: modify pt from "<< ptrk->pt() << " to " << cpfo_p4.Pt());
      } else{
	float expectedPt = expectedEnergy/cosh(pcpf.eta());

	if( abs(weight-1.0)>FLT_EPSILON ){ // check against float precision
	  weight = (expectedPt + weight*(ptrk->pt()-expectedPt))/ptrk->pt();
	}
      } // dense environment
    } // expected energy
    
    ATH_MSG_VERBOSE("CPFO received weight: " << weight);

    return StatusCode::SUCCESS;
  }

}
