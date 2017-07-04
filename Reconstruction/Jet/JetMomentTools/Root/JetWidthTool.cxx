/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetWidthTool.cxx

#include "JetMomentTools/JetWidthTool.h"

#include "xAODJet/JetConstituentVector.h"
#include "JetUtils/JetDistances.h"
#include "PFlowUtils/IWeightPFOTool.h"

//**********************************************************************

JetWidthTool::JetWidthTool(std::string myname)
  : JetModifierBase(myname)
{
}

//**********************************************************************

int JetWidthTool::modifyJet(xAOD::Jet& jet) const {

  double widthEta = 0, widthPhi = 0;
  //jet.setAttribute("Width", width(jet));
  jet.setAttribute("Width", width(jet,widthEta,widthPhi));
  //jet.setAttribute("WidthEta",widthEta);
  jet.setAttribute("WidthPhi",widthPhi);
  return 0;
}

//**********************************************************************

//double JetWidthTool::width(const xAOD::Jet& jet) const {
//
//  // Get the constituents of the jet
//  const xAOD::JetConstituentVector constituents = jet.getConstituents();
//  xAOD::JetConstituentVector::iterator iter = constituents.begin();
//  xAOD::JetConstituentVector::iterator iEnd = constituents.end();
//
//  // TODO: Switch to using helper function once JetUtils has been updated
//  // Set the width
//  // jetWidth = JetKinematics::ptWeightedWidth(iter,iEnd,&jet);
//    
//  // Calculate the pt weighted width
//  const double jetEta = jet.eta();
//  const double jetPhi = jet.phi();
//  double weightedWidth = 0;
//  double ptSum = 0;
//
//  for ( ; iter != iEnd; ++iter) {
//    const double dR = jet::JetDistances::deltaR(jetEta, jetPhi, iter->eta(),  iter->phi() );
//    const double pt = iter->pt();
//    weightedWidth += dR * pt;
//    ptSum += pt;
//  }
//
//  return ptSum > 0 ? weightedWidth/ptSum : -1;
//}

double JetWidthTool::width(const xAOD::Jet& jet, double& widthEta, double& widthPhi) const {

  // Get the constituents of the jet
  // TODO: Switch to using helper function once JetUtils has been updated
  // Set the width
  // jetWidth = JetKinematics::ptWeightedWidth(iter,iEnd,&jet);
    
  // Calculate the pt weighted width
  const double jetEta = jet.eta();
  const double jetPhi = jet.phi();
  double weightedWidth = 0;
  double weightedWidthEta = 0;
  double weightedWidthPhi = 0;
  double ptSum = 0;

  const xAOD::JetInput::Type jetinput = jet.getInputType();
  const bool isPFlowEM = (jetinput == xAOD::JetInput::PFlow || jetinput == xAOD::JetInput::EMPFlow);
  const bool isPFlowLC = (jetinput == xAOD::JetInput::LCPFlow || jetinput == xAOD::JetInput::EMCPFlow);

  const xAOD::JetConstituentVector constituents = jet.getConstituents();
  for (const auto& constituent : constituents) {
    const double dR   = jet::JetDistances::deltaR(jetEta, jetPhi, constituent->eta(),  constituent->phi() );
    const double dEta = fabs(jet::JetDistances::deltaEta(jetEta, constituent->eta() ));
    const double dPhi = fabs(jet::JetDistances::deltaPhi(jetPhi, constituent->phi() ));
    const double pt   = constituent->pt();

    weightedWidth += dR * pt;
    weightedWidthEta += dEta * pt;
    weightedWidthPhi += dPhi * pt;
    
    ptSum += pt * weight;
  }

  widthEta = ptSum > 0 ? weightedWidthEta/ptSum : -1;
  widthPhi = ptSum > 0 ? weightedWidthPhi/ptSum : -1;

  return ptSum > 0 ? weightedWidth/ptSum : -1;
}

//**********************************************************************
