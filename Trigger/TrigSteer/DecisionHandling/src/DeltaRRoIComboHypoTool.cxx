/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DeltaRRoIComboHypoTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <algorithm>
#include <cmath>

using namespace TrigCompositeUtils;

DeltaRRoIComboHypoTool::DeltaRRoIComboHypoTool(const std::string& type,
					       const std::string& name,
					       const IInterface* parent)
  : ComboHypoToolBase(type, name, parent)
{ }


StatusCode DeltaRRoIComboHypoTool::initialize() {
  ATH_MSG_DEBUG("DR threshold set to " << m_DRcut );
  return StatusCode::SUCCESS;
}



bool DeltaRRoIComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>& combination) const
{
  //retrieve the rois 
  std::vector<ElementLink<TrigRoiDescriptorCollection>> selected_rois;

  // Expecting to only run over chains with two legs and one Decision Object required on each leg
  // So should always have two objects from which to check DeltaR
  if(combination.size() != 2){
    ATH_MSG_ERROR("Expecting to combine exactly two Decision Objects, but instead found " << combination.size() << ". Will throw a runtime error");
    throw std::runtime_error("Expecting to combine exactly two Decision Objects, but instead found "+combination.size());
  }

  for (const auto el: combination){
    const auto EL= el.second;    
    const auto dec= (*EL);
    const auto roiLink = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( dec, initialRoIString() ).link;
    selected_rois.push_back(roiLink);
  }

  auto roiLink1=selected_rois[0];
  auto roiLink2=selected_rois[1];
  // calucalte DeltaR
  float Dr = deltaR((*roiLink1)->eta(), (*roiLink2)->eta(), (*roiLink1)->phi(), (*roiLink2)->phi());
  ATH_MSG_DEBUG("Found two RoIs with eta/phi " << (*roiLink1)->eta() <<"/"<<(*roiLink1)->phi() <<" and "<< (*roiLink2)->eta()<<"/"<<(*roiLink2)->phi() <<" with Dr="<<Dr);

  // apply the cut
  bool pass=true;
  if (Dr > m_DRcut) pass=false;

  if (pass)
    ATH_MSG_DEBUG( "  RoIs within DR<"<<m_DRcut<<". This seleciton passed! ");
  
  return pass;
}


double DeltaRRoIComboHypoTool::deltaR(double eta1, double eta2, double phi1, double phi2) const {
  double dPhi=std::remainder( phi1 - phi2, 2*M_PI );
  double dEta=std::fabs(eta1-eta2);
  return std::sqrt(dEta*dEta + dPhi*dPhi);
}

