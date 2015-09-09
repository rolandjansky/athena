/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstitFourMomTool.cxx

#include "JetMomentTools/JetConstitFourMomTool.h"

#include "xAODJet/JetConstituentVector.h"
#include "xAODJet/JetTypes.h"
//**********************************************************************

JetConstitFourMomTool::JetConstitFourMomTool(std::string myname)
: JetModifierBase(myname) {
  declareProperty("JetScaleName", m_jetScaleName="JetLCScaleMomentum");
  declareProperty("ConstitScale", m_constitScale=xAOD::CalibratedJetConstituent);
}

//**********************************************************************

int JetConstitFourMomTool::modifyJet(xAOD::Jet& jet) const {
  //  jet.setAttribute("", width(jet));

  xAOD::JetFourMom_t constitFourVec;
  calcConstitFourVec(jet,constitFourVec);

  jet.setJetP4(m_jetScaleName, constitFourVec);

  return 0;
}

//**********************************************************************
void JetConstitFourMomTool::calcConstitFourVec(const xAOD::Jet& jet, xAOD::JetFourMom_t& constitFourVec) const {

  // Get the constituents of the jet
  const xAOD::JetConstituentVector constituents = jet.getConstituents();

  //gets the iter at the LC scale

  xAOD::JetConstitScale constscale = (xAOD::JetConstitScale) m_constitScale;
  for (auto citer = constituents.begin(constscale); //gets the iter at the LC scale
       citer != constituents.end(constscale); ++citer) {
    constitFourVec += **citer;
  }
}

//**********************************************************************
