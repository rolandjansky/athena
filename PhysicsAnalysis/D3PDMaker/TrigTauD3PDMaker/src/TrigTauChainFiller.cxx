/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauChainFiller.h"
#include "AthenaKernel/errorcheck.h"

//trigger navigation
#define TRIGREADNAVMATCH_FUNC isSamePtr 
#include "TrigTauD3PDMaker/TrigReadNavTool.h"

namespace D3PD {

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrigTauChainFiller::TrigTauChainFiller (const std::string& type,
					  const std::string& name,
					  const IInterface* parent)
    : BlockFillerTool<void> (type, name, parent),
      m_trigRnt("TrigTauTool::TrigReadNavTool/TrigReadNavTool")
  {
    declareProperty("TrigReadNavTool", m_trigRnt);    

    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode TrigTauChainFiller::initialize()
  {
    StatusCode sc;

    if( !m_trigRnt.empty() ){
      sc = m_trigRnt.retrieve();
      if( !sc.isSuccess() ){
	return sc;
      }
    }

    return StatusCode::SUCCESS;
  }

  /**
   * @brief Book variables for this block.
   */
  StatusCode TrigTauChainFiller::book()
  {
    CHECK( addVariable ("chainRoImapChains", m_chainRoImapChains ) );
    CHECK( addVariable ("chainRoImapRoIwords", m_chainRoImapRoIwords ) );
    return StatusCode::SUCCESS;
  }

  /**
   * @brief Fill one block --- type-safe version.
   *
   * This is called once per event?.  
   */
  StatusCode TrigTauChainFiller::fill ()
  {
    std::map< std::string, std::vector<unsigned int> > chainsToRoIwords = m_trigRnt->mapChainsToRoIs("((L1.*TAU.*)|(L2.*tau.*)|(EF.*tau.*))");
    std::map<std::string, std::vector<unsigned int> >::iterator it;
    for(it=chainsToRoIwords.begin(); it!=chainsToRoIwords.end(); it++){
      std::vector<unsigned int> rois = (*it).second;
      if(rois.size()>0){
	(*m_chainRoImapChains).push_back( (*it).first );
	(*m_chainRoImapRoIwords).push_back( rois );
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace D3PD
