/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetWidthTool.cxx

#include "JetMomentTools/JetWidthTool.h"

#include "xAODJet/JetConstituentVector.h"
#include "JetUtils/JetDistances.h"

//**********************************************************************

JetWidthTool::JetWidthTool(std::string myname)
: JetModifierBase(myname) { }

//**********************************************************************

int JetWidthTool::modifyJet(xAOD::Jet& jet) const {
  jet.setAttribute("Width", width(jet));
  return 0;
}

//**********************************************************************

double JetWidthTool::width(const xAOD::Jet& jet) const {

  // Get the constituents of the jet
  const xAOD::JetConstituentVector constituents = jet.getConstituents();
  xAOD::JetConstituentVector::iterator iter = constituents.begin();
  xAOD::JetConstituentVector::iterator iEnd = constituents.end();

  // TODO: Switch to using helper function once JetUtils has been updated
  // Set the width
  // jetWidth = JetKinematics::ptWeightedWidth(iter,iEnd,&jet);
    
  // Calculate the pt weighted width
  const double jetEta = jet.eta();
  const double jetPhi = jet.phi();
  double weightedWidth = 0;
  double ptSum = 0;

  for ( ; iter != iEnd; ++iter) {
    const double dR = jet::JetDistances::deltaR(jetEta, jetPhi, iter->eta(),  iter->phi() );
    const double pt = iter->pt();
    weightedWidth += dR * pt;
    ptSum += pt;
  }

  return ptSum > 0 ? weightedWidth/ptSum : -1;
}

//**********************************************************************
