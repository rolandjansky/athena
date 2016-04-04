/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/TrigTauORLTool.h"
#include "TrigTauEmulation/Utils.h"
#include "TrigTauEmulation/MsgStream.h"

const double DELTA_OVERLAP_J = round(100*((0.1)*(0.1)));
const double DELTA_OVERLAP_EMTAU = round(100*((0.0)*(0.0)));

TrigTauORLTool::TrigTauORLTool(const std::string& name) : asg::AsgTool(name)
{

}

TrigTauORLTool::TrigTauORLTool(const TrigTauORLTool& other) : asg::AsgTool(other.name() + "_copy")
{

}


StatusCode TrigTauORLTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigTauORLTool::execute(const xAOD::EmTauRoIContainer* c1,
    const xAOD::EmTauRoIContainer* c2,
    const std::string & sel1,
    const std::string & sel2)

{
  std::string decor = sel1 + "-" + sel2;
  m_orl_decisions[decor] = false;
  MY_MSG_DEBUG("TrigTauORLTool::execute(): considering overlap between " << c1 << " and " << c2 << " using sel1=" << sel1 << " sel2=" << sel2);
  for(const auto p1 : *c1){
    p1->auxdecor<bool>(decor) = false;
    // Only consider selected objects
    if (sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
      for(const auto p2 : *c2){
        p2->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if (sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true) ) {
          double delta_r = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p2->eta(), p2->phi());
          if (delta_r > DELTA_OVERLAP_EMTAU) {
            // if (p1->roiWord() != p2->roiWord()){
            m_orl_decisions[decor] = true;
            p1->auxdecor<bool>(decor) = true;
            p2->auxdecor<bool>(decor) = true;
          }
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TrigTauORLTool::execute(const xAOD::EmTauRoIContainer* c1,
      const xAOD::JetRoIContainer* c2,
      const std::string & sel1,
      const std::string & sel2)

  {
    std::string decor = sel1 + "-" + sel2;
    m_orl_decisions[decor] = false;

    for(const auto p1 : *c1){
      p1->auxdecor<bool>(decor) = false;
      // Only consider selected objects
      if (sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
        for(const auto p2 : *c2){
          p2->auxdecor<bool>(decor) = false;
          // Only consider selected objects
          if (sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true) ) {
            double delta_r = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p2->eta(), p2->phi());
            if (delta_r > DELTA_OVERLAP_J) {
              // if (p1->roiWord() != p2->roiWord()){
              m_orl_decisions[decor] = true;
              p1->auxdecor<bool>(decor) = true;
              p2->auxdecor<bool>(decor) = true;
            }
            }
          }
        }
      }
      return StatusCode::SUCCESS;
    }

    StatusCode TrigTauORLTool::execute(const xAOD::EmTauRoIContainer* c1,
        const xAOD::EmTauRoIContainer* c2,
        const xAOD::JetRoIContainer* c3,
        const std::string & sel1,
        const std::string & sel2,
        const std::string & sel3)

    {
      std::string decor = sel1 + sel2 + "-" + sel3;
      m_orl_decisions[decor] = false;

      for(const auto p1 : *c1){
        p1->auxdecor<bool>(decor) = false;
        // Only consider selected objects
        if (sel1 == "" || (sel1 != "" && p1->auxdataConst<bool>(sel1) == true) ) {
          for(const auto p2 : *c2){
            p2->auxdecor<bool>(decor) = false;
            // Only consider selected objects
            if (sel2 == "" || (sel2 != "" && p2->auxdataConst<bool>(sel2) == true) ) {
              for (const auto p3: *c3) {
                p3->auxdecor<bool>(decor) = false;
                if (sel3 == "" || (sel3 != "" && p3->auxdataConst<bool>(sel3) == true) ) {
                  double delta_r_p1_p2 = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p2->eta(), p2->phi()); 
                  double delta_r_p1_p3 = Utils::L1_DeltaR_Square(p1->eta(), p1->phi(), p3->eta(), p3->phi());
                  double delta_r_p2_p3 = Utils::L1_DeltaR_Square(p2->eta(), p2->phi(), p3->eta(), p3->phi());
                  if (delta_r_p1_p2 > DELTA_OVERLAP_EMTAU &&delta_r_p1_p3 > DELTA_OVERLAP_J && delta_r_p2_p3 > DELTA_OVERLAP_J) {

                    // if (p1->roiWord() != p3->roiWord() && p2->roiWord() != p3->roiWord()){
                    m_orl_decisions[decor] = true;
                    p1->auxdecor<bool>(decor) = true;
                    p2->auxdecor<bool>(decor) = true;
                    p3->auxdecor<bool>(decor) = true;
                  }
                  }
                }
              }
            }
          }
        }
        return StatusCode::SUCCESS;
      }


      bool TrigTauORLTool::decision(const std::string & item)
      {
        if (m_orl_decisions.find(item) != m_orl_decisions.end()) {
          return m_orl_decisions[item];
        } else {
          MY_MSG_ERROR(item<<" is not in the list of known ORL requirements");
          return false;
        }
      }   

