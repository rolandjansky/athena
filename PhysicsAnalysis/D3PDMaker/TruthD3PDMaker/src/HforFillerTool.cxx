/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @author George Lewis
/// @date Nov, 2010
/// @brief Fill in Heavy Flavor type for MC from HforTool
/// Modified for use with TruthD3PDMaker

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "HforFillerTool.h"
#include "HepMC/SimpleVector.h"

namespace D3PD{

  HforFillerTool::HforFillerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : BlockFillerTool<void> (type, name, parent),
      m_hfor_tool("HforTool/hforTool")
  {
    declareProperty("hforTool", m_hfor_tool);

    book().ignore(); // Avoid coverity warnings.
  }


  StatusCode HforFillerTool::initialize() {
    CHECK( BlockFillerTool<void>::initialize() );
    CHECK( m_hfor_tool.retrieve() );
    return StatusCode::SUCCESS;
  }


  StatusCode HforFillerTool::book() {
    CHECK( addVariable ("hfor_type", m_hfor_type) );
    CHECK( addVariable ("hfor_event_flavour", hfor_event_flavour) );
    CHECK( addVariable ("hfor_Quarks_px", hfor_Quarks_px) );
    CHECK( addVariable ("hfor_Quarks_py", hfor_Quarks_py) );
    CHECK( addVariable ("hfor_Quarks_pz", hfor_Quarks_pz) );
    CHECK( addVariable ("hfor_Quarks_e", hfor_Quarks_e) );

    return StatusCode::SUCCESS;
  }


  StatusCode HforFillerTool::fill() {

    // Get the result for the method chosen in the config-file

    hfor_Quarks_px->clear();
    hfor_Quarks_py->clear();
    hfor_Quarks_pz->clear();
    hfor_Quarks_e->clear();

    hfor_event_flavour->clear();
    *m_hfor_type = -1;

    std::string hfor_decision;
    int m_event_flavour;

    std::vector<HepMC::FourVector> GS_bQuarks;
    std::vector<HepMC::FourVector> ME_bQuarks;

    std::vector<HepMC::FourVector> GS_cQuarks;
    std::vector<HepMC::FourVector> ME_cQuarks;

    // execute() throws a StatusCode::FAILURE if heavy flavor overlap is not required
    StatusCode sc = m_hfor_tool->execute();
    if (sc != StatusCode::FAILURE) {
      hfor_decision = m_hfor_tool->getDecision();

      GS_bQuarks = m_hfor_tool->get_bQuarks_GS();
      ME_bQuarks = m_hfor_tool->get_bQuarks_ME();

      ME_cQuarks = m_hfor_tool->get_cQuarks_GS();
      GS_cQuarks = m_hfor_tool->get_cQuarks_ME();

      if      (hfor_decision == "")              *m_hfor_type = -1;
      else if (hfor_decision == "isBB")          *m_hfor_type =  0;
      else if (hfor_decision == "isCC")          *m_hfor_type =  1;
      else if (hfor_decision == "isC")           *m_hfor_type =  2;
      else if (hfor_decision == "isLightFlavor") *m_hfor_type =  3;
      else if (hfor_decision == "kill_bb")       *m_hfor_type =  4;
      else if (hfor_decision == "kill_cc")       *m_hfor_type =  4;
      else if (hfor_decision == "kill")          *m_hfor_type =  4;

      for (size_t i=0; i<GS_bQuarks.size(); i++){
        hfor_Quarks_px->push_back( static_cast<float> (GS_bQuarks.at(i).px()) );
        hfor_Quarks_py->push_back( static_cast<float> (GS_bQuarks.at(i).py()) );
        hfor_Quarks_pz->push_back( static_cast<float> (GS_bQuarks.at(i).pz()) );
        hfor_Quarks_e->push_back( static_cast<float> (GS_bQuarks.at(i).e()) );
        m_event_flavour = 0;
        hfor_event_flavour->push_back(m_event_flavour);
      }
      for (size_t i=0; i<ME_bQuarks.size(); i++){
        hfor_Quarks_px->push_back( static_cast<float> (ME_bQuarks.at(i).px()) );
        hfor_Quarks_py->push_back( static_cast<float> (ME_bQuarks.at(i).py()) );
        hfor_Quarks_pz->push_back( static_cast<float> (ME_bQuarks.at(i).pz()) );
        hfor_Quarks_e->push_back( static_cast<float> (ME_bQuarks.at(i).e()) );
        m_event_flavour = 1;
        hfor_event_flavour->push_back(m_event_flavour);
      }
      for (size_t i=0; i<GS_cQuarks.size(); i++){
        hfor_Quarks_px->push_back( static_cast<float> (GS_cQuarks.at(i).px()) );
        hfor_Quarks_py->push_back( static_cast<float> (GS_cQuarks.at(i).py()) );
        hfor_Quarks_pz->push_back( static_cast<float> (GS_cQuarks.at(i).pz()) );
        hfor_Quarks_e->push_back( static_cast<float> (GS_cQuarks.at(i).e()) );
        m_event_flavour = 2;
        hfor_event_flavour->push_back(m_event_flavour);
      }
      for (size_t i=0; i<ME_cQuarks.size(); i++){
        hfor_Quarks_px->push_back( static_cast<float> (ME_cQuarks.at(i).px()) );
        hfor_Quarks_py->push_back( static_cast<float> (ME_cQuarks.at(i).py()) );
        hfor_Quarks_pz->push_back( static_cast<float> (ME_cQuarks.at(i).pz()) );
        hfor_Quarks_e->push_back( static_cast<float> (ME_cQuarks.at(i).e()) );
        m_event_flavour = 3;
        hfor_event_flavour->push_back(m_event_flavour);
      }

    }

    return StatusCode::SUCCESS;
  }

} // namespace D3PD
