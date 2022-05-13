/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./DipsCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

DipsCondition::DipsCondition(double workingPoint,
                             const float &cfrac,
                             const std::string &decName_pb,
                             const std::string &decName_pc,
                             const std::string &decName_pu,
                             const std::string &decName_isValid) :
  m_workingPoint(workingPoint),
  m_cfrac(cfrac),
  m_decName_pb(decName_pb),
  m_decName_pc(decName_pc),
  m_decName_pu(decName_pu),
  m_decName_isValid(decName_isValid)
{

}

float DipsCondition::getDipsDecValue(const pHypoJet &ip,
                                     const std::unique_ptr<ITrigJetHypoInfoCollector> &collector,
                                     const std::string &decName) const
{

  float momentValue = -1;
  if (!(ip->getAttribute(decName, momentValue)))
  {
    if (collector)
    {
      auto j_addr = static_cast<const void *>(ip.get());

      std::stringstream ss0;
      ss0 << "DipsCondition: "
          << " unable to retrieve " << decName << '\n';
      std::stringstream ss1;
      ss1 << "     jet : (" << j_addr << ")";
      collector->collect(ss0.str(), ss1.str());
    }

    throw std::runtime_error("Impossible to retrieve decorator \'" + decName + "\' for jet hypo");
  }

  return momentValue;
}

float DipsCondition::evaluateDips(const float &dips_pb,
                                  const float &dips_pc,
                                  const float &dips_pu) const {
  return log(dips_pb / (m_cfrac * dips_pc + (1 - m_cfrac) * dips_pu));
}

bool DipsCondition::isSatisfied(const pHypoJet &ip,
                                const std::unique_ptr<ITrigJetHypoInfoCollector> &collector) const
{
  if (!m_decName_isValid.empty()) {
    // short circuit if dips is invalid and we ask for a check
    //
    // we have to dynamic cast to xAOD jets here because there's no char
    // accessor on IJet and it's not clear if we need one generally.
    const auto* jet = dynamic_cast<const HypoJet::xAODJetAsIJet*>(ip.get());
    if (!jet) throw std::runtime_error("Fast dips has to run on xAOD::Jet");
    char valid = (*jet->xAODJet())->getAttribute<char>(m_decName_isValid);
    if (valid == 0) return false;
  }

  // if we got this far check the dips hypo
  float dips_pb = getDipsDecValue(ip, collector, m_decName_pb);
  float dips_pc = getDipsDecValue(ip, collector, m_decName_pc);
  float dips_pu = getDipsDecValue(ip, collector, m_decName_pu);
  float dips_output = evaluateDips(dips_pb, dips_pc, dips_pu);

  bool pass = (dips_output >= m_workingPoint);

  if (collector)
  {
    const void *address = static_cast<const void *>(this);

    std::stringstream ss0;
    ss0 << "DipsCondition: (" << address
        << ")"
        << " pass: " << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void *>(ip.get());
    std::stringstream ss1;
    ss1 << "     jet : (" << j_addr << ") "
                                       "moment "
        << m_decName_pb << " value: " << dips_pb << '\n';

    collector->collect(ss0.str(), ss1.str());
  }

  return pass;
}

bool 
DipsCondition::isSatisfied(const HypoJetVector& ips,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string DipsCondition::toString() const {
  std::stringstream ss;
  ss << "DipsCondition (" << this << ") "
     << " Cleaning decs: " 
     << m_decName_pb << ", "
     << m_decName_pu << ", "
     << m_decName_pc << ", "
     <<'\n';

  return ss.str();
}
