/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "CTPUnpackingToolBase.h"
#include "DecisionHandling/HLTIdentifier.h"


CTPUnpackingToolBase::CTPUnpackingToolBase(const std::string& type, 
                                           const std::string& name, 
                                           const IInterface* parent) 
  : base_class(type, name, parent)
{
}


StatusCode CTPUnpackingToolBase::initialize() 
{
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode CTPUnpackingToolBase::decodeCTPToChainMapping() {
  std::istringstream input;
  for ( auto entry: m_ctpToChainProperty ) {
    input.clear();
    input.str(entry);
    size_t ctpId;
    input >> ctpId;
    char delim;
    input >> delim;    
    if ( delim != ':' ) {
      //      log << MSG::ERROR
      //	error() << "Error in conf. entry: " << entry << " missing ':'"<<endreq;
      //      ATH_MSG_ERROR( "Error in conf. entry: " << entry << " missing ':'" );
      return StatusCode::FAILURE;
    }
    std::string chainName;
    input >> chainName;
    //log << MSG::DEBUG <<"Chain " << chainName << " seeded from CTP item of ID " << ctpId
    //    ATH_MSG_DEBUG( "Chain " << chainName << " seeded from CTP item of ID " << ctpId );
    m_ctpToChain[ctpId].push_back( HLT::Identifier(chainName) );
  }
  return StatusCode::SUCCESS;
}


