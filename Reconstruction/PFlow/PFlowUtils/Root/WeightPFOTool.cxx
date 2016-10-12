/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFlowUtils/WeightPFOTool.h"

namespace CP {

  WeightPFOTool::WeightPFOTool(const std::string& name) : asg::AsgTool( name )
  {
    declareProperty("DoEoverPWeight", m_doEoverPweight=true);
    declareProperty("NeutralPFOScale",m_theNeutralPFOScaleString="EM");
  }

  // The intended result:
  //
  //CP::EM Case
  // Subtraction applied (inDenseEnvironment==false)
  // pt < 30 GeV: Ptrk [weight = 1]
  // 30 <= pt < 60 GeV: Ptrk ( E/P + (1-E/P)(1 - (pt - 30)/30 ) ) [weight = E/P + (1-E/P)(1 - (pt - 30)/30 )]
  // pt >= 60 GeV: Ptrk * E/P [weight = E/P]
  // 
  // Subtraction not applied (inDenseEnvironment==true)
  // pt < 30 GeV: Ptrk (1 - E/P) + Ecal [weight = (1-E/P]
  // 30 <= pt < 60 GeV: Ptrk ( (1-E/P)(1 - (pt - 30)/30 ) ) + Ecal [weight = (1-E/P)(1 - (pt - 30)/30 )]
  // pt >= 60 GeV: Ecal [weight = 0]
  //
  //CP::LC Case - this follows when you assuem E/p = 1 for the LC scale in the above formulae
  //Subtraction applied (inDenseEnvironment==false)
  // pt < 30 GeV: Ptrk weight = 1
  // 30 <= pt < 60 GeV: weight = 0
  // pt >= 60 GeV: weight = 1;
  //
  // Subtraction not applied (inDenseEnvironment==true)
  // All Pt ranges: weight = 0
  
  StatusCode WeightPFOTool::fillWeight( const xAOD::PFO& cpfo, float& weight ) {

    //we need to convert the string scale back to the enum
    PFO_JetMETConfig_inputScale theNeutralPFOScale = CP::EM;
    CP::inputScaleMapper inputScaleMapper;
    bool answer = inputScaleMapper.getValue(m_theNeutralPFOScaleString,theNeutralPFOScale);
    if (false == answer) ATH_MSG_FATAL("Invalid neutral PFO Scale has been specified in PFlowUtils::PFOWeightTool");

    // Compute the weights internally
    weight = 0.;
    if(cpfo.pt()>100e3) {
      ATH_MSG_WARNING("PFO with invalid pt " << cpfo.pt() << ", quitting.");
      return StatusCode::FAILURE;
    }
	
    int isInDenseEnvironment = false;
    float expectedEnergy = 0.0;
    bool gotVariable = cpfo.attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment);
    gotVariable &= cpfo.attribute(xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit,expectedEnergy);
    if (!gotVariable) {
      ATH_MSG_WARNING("This charged PFO did not have eflowRec_isInDenseEnvironment or eflowRec_tracksExpectedEnergyDeposit set");
      return StatusCode::FAILURE;
    } else {
      //EM case first
      if (CP::EM == theNeutralPFOScale){
	// Start by computing the correction as though we subtracted the calo energy
	// This interpolates between the full track P and the expected calo E
	float EoverP = expectedEnergy/cpfo.e(); // divide once only
	if(m_doEoverPweight) {
	  if(cpfo.pt()<30e3) {        // take full track
	    weight = 1.;
	  } else if(cpfo.pt()<60e3) { // linearly interpolate between 1 and E/P
	    float interpolf = (1.0 - (cpfo.pt()-30000)/30000);
	    weight = EoverP + interpolf * (1-EoverP);
	  } else {                    // take the expected energy
	    weight = EoverP;
	  }
	}

	ATH_MSG_VERBOSE("cpfo in dense environment? " << isInDenseEnvironment);
	ATH_MSG_VERBOSE("cpfo pt: " << cpfo.pt() << ", E/P: " << EoverP << ", weight: " << weight);

	if(isInDenseEnvironment) {
	  // In this case we further remove the expected deposited energy from the track
	  weight -= EoverP;
	}
      }//EM Scale
      else if (CP::LC == theNeutralPFOScale){
	if(!isInDenseEnvironment){
	  if(cpfo.pt()<30e3 || cpfo.pt() >= 60e03) weight = 1;
	}
      }
    }
	
    ATH_MSG_VERBOSE("Weight before zero check: " << weight);
    // If the weight went to 0, set it to the ghost scale, so that the cPFOs
    // are always added to the track.
    if (weight<1e-9) {weight = 1e-20;}
    ATH_MSG_VERBOSE("Final weight: " << weight);
    return StatusCode::SUCCESS;
  }

}
