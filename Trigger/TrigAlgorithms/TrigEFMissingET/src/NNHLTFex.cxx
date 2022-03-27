/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file NNHLTFex.cxx
 *
 * Implementation of the HLT NN combination fex class
 * @author Zhelun Li
 *****************************************************************************/

#include "NNHLTFex.h"
#include "TrigEFMissingET/METComponent.h"
#include <array>
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <istream>
#include <fstream>

namespace HLT::MET {

  NNHLTFex::NNHLTFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_graphNN(nullptr)
  {
  }

StatusCode NNHLTFex::initialize()
  {
    std::ifstream inputNN(m_filename);
    m_configNN = lwt::parse_json_graph(inputNN);
    if (m_configNN.inputs.size() != 1) return StatusCode::FAILURE;//only one input node allowed
    m_graphNN = std::make_unique<lwt::LightweightGraph>(m_configNN); 
    CHECK(m_triggerMETs.initialize());
    return initializeBase({"HLTMET"});
  }

  StatusCode NNHLTFex::fillMET(
      xAOD::TrigMissingET& met,
      const EventContext& context,
      MonGroupBuilder&) const
  { 
    //for lwtnn:
    std::map<std::string, std::map<std::string, double> > inputs;
    for (const SG::ReadHandleKey<xAOD::TrigMissingETContainer> &key : m_triggerMETs)
    {
      std::string name = key.key();
      SG::ReadHandle<xAOD::TrigMissingETContainer> met = SG::makeHandle(key,context);
      if (!met.isValid())
        {
          ATH_MSG_ERROR("Failed to retrieve " << key);
          return StatusCode::FAILURE;
        }

      if (met->size() == 0)
      {
        inputs["node_0"].insert(std::pair<std::string, double>(name+".met",-1) );
        inputs["node_0"].insert(std::pair<std::string, double>(name+".sumet",-1) );
      }
      else
      {
        const xAOD::TrigMissingET &metObj = *(met->at(0));
        float mpx = metObj.ex();
        float mpy = metObj.ey();
        inputs["node_0"].insert(std::pair<std::string, double>(name+".met",sqrt(mpx * mpx + mpy * mpy)/Gaudi::Units::GeV) );
        inputs["node_0"].insert(std::pair<std::string, double>(name+".sumet",metObj.sumEt()/Gaudi::Units::GeV) );
      }
    }
    
    std::map<std::string, double> outputNN = m_graphNN->compute(inputs); 
    auto NN_MET=outputNN["out_0"]*Gaudi::Units::GeV;
    //The NN only estimates the MET. Setting to mpx. But this means that only the total MET makes sense. mpx,mpy,mpz,sumEt 
    // are no available. Be careful!
    met.setEx(NN_MET);
    
    return StatusCode::SUCCESS;
  }
}  //> end namespace HLT::MET