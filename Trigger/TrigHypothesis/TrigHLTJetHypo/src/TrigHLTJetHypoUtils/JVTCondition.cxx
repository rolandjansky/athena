/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JVTCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NonexistantVariable.h" //error handling
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>


// Constructor
JVTCondition::JVTCondition(double etThreshold, double workingPoint){
  m_etThreshold  = etThreshold;
  m_workingPoint = workingPoint;
}


// isSatisfied()
bool JVTCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 1){
    std::stringstream ss;
    ss << "JVT::isSatisfied must see exactly 1 jet, but received "
       << ips.size()
       << '\n';
    throw std::runtime_error(ss.str());
  }
  return passCuts(ips[0]);
}


// passCuts()
bool JVTCondition::passCuts(pHypoJet jet) const {
  // The conditions for each jet are: ET>etmin, 0<|eta|<320 and (JVT>JVTwp or |eta|>2.5 or pT>60)
  auto et        = m_MeVtoGeV * jet->et();
  auto pt        = m_MeVtoGeV * jet->pt();
  auto absEta    = std::abs(jet->eta());
  float detEta   = 0;
  if(!(jet->getAttribute("DetectorEta",detEta))){
    throw NonexistantVariable("JVT condition cannot retrieve variable 'DetectorEta', 'DetectorEta' does not exist");
  }
  auto absdetEta = std::abs(detEta);
  bool result         = false;
  bool jvtApplicable  = (absdetEta<2.5 and pt<60) ? true : false;
  bool passKinematics = (et>m_etThreshold and absEta<3.2) ? true : false; 
  if(!jvtApplicable){ // jvt not applicable
    return passKinematics;
  } else { // jvt applicable
    if(!passKinematics) return passKinematics; // kinematic selection not passed
    else { // kinematic selection passed
      float jvt;
      if(!(jet->getAttribute("Jvt",jvt))){
        throw NonexistantVariable("JVT condition cannot retrieve variable 'Jvt', 'Jvt' does not exist");
      }
      bool passJVT = (jvt>m_workingPoint) ? true : false;
      return passKinematics*passJVT;
    }
  }

  return result;
}


// toString()
std::string JVTCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "JVTCondition: "
     << "jet cuts: "
     << "etThreshold "
     << m_etThreshold
     << " absEtaMin 0"
     << " absEtaMax 320" 
     << " working point "
     << m_workingPoint
     <<'\n';
  return ss.str();
}


// orderingParameter()
double JVTCondition::orderingParameter() const noexcept {
  return m_etThreshold;
}
