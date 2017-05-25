/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigT1Result/RoIBResult.h"
#include "CTPUnpackingEmulationTool.h"


#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContextHash.h"
#include <fstream>

using namespace HLT;


CTPUnpackingEmulationTool::CTPUnpackingEmulationTool( const std::string& type,
				    const std::string& name, 
				    const IInterface* parent ) 
  : AthAlgTool(type, name, parent), 
    m_inputFileName("CTPEmulation.dat")
{
  declareProperty("CTPToChainMapping", m_ctpToChainProperty, "Mapping of the form: '34:HLT_x', '35:HLT_y', ..., both CTP ID and chain may appear many times");
  declareProperty("ForceEnableAllChains", m_forceEnable=false, "Enables all chains in each event, testing mode");
  declareProperty("InputFilename", m_inputFileName, "Fake CTP RoIb input filename");
}


CTPUnpackingEmulationTool::~CTPUnpackingEmulationTool()
{}


StatusCode CTPUnpackingEmulationTool::parseInputFile() {
  std::ifstream inputFile(m_inputFileName);
  if ( not inputFile.good() ) {
    ATH_MSG_ERROR("Input file " << m_inputFileName << " inaccessible");
    return StatusCode::FAILURE;
  }
  
  std::string line;
  while ( std::getline( inputFile, line ) ) {
    //check if commented out
    if ( line[0] == '#' ) continue;
    // std::size_t found = line.find("#");
    // if (found != std::string::npos) {
    //   std::string line;
    //   std::getline(inputFile, line);
    //   continue;
    // }
    std::istringstream allChains( line );
    HLT::IDVec ids;
    while ( allChains ) {
      std::string chainName;
      allChains >> chainName;
      if ( not chainName.empty() ) 
	ids.push_back( HLT::Identifier(chainName).numeric() );
    }
    // auto chainId = HLT::Identifier(chainName);
    // std::map<size_t, HLT::IDVec>::iterator it;// IndexToIdentifiers;
    // for(it=m_ctpToChain.begin();it!=m_ctpToChain.end();it++)  {
    //   auto ctpId = it->first;
    //   HLT::IDVec chains= it->second;
    //   auto itr = find (chains.begin(), chains.end(), chainId);
    //   if ( itr != chains.end() ) {
    // 	event.insert( ctpId );
    //   }
    // }
   
    if ( not ids.empty() ) 
      m_events.push_back( ids ); // new event
  }
  inputFile.close(); 

  ATH_MSG_DEBUG("In input file "<<m_inputFileName<<" found "<<m_events.size()<<" chain sets");
  return StatusCode::SUCCESS;
}


StatusCode CTPUnpackingEmulationTool::initialize() {
  CHECK( decodeCTPToChainMapping()); 

  for(auto ctpid : m_ctpToChain){
    for (auto chain : ctpid.second){
      ATH_MSG_DEBUG( ctpid.first << " " << chain);
    }
  }
  return parseInputFile();
}




StatusCode CTPUnpackingEmulationTool::decode( const ROIB::RoIBResult& /*roib*/,  HLT::IDVec& enabledChains ) const {
  size_t numberPfActivatedBits= 0;

  if (m_events.size() ==0){
    ATH_MSG_ERROR("No chain set found. Cannot decode CTP emulation");
    return StatusCode::FAILURE;
  }


  

  const EventContext& context = Gaudi::Hive::currentContext();
 
  int line = context.evt() % m_events.size();
  //  ATH_MSG_DEBUG("Getting chains for event "<<ctx<<": retrieve combination from line "<< line);
  //  auto tav = m_events[line];

  enabledChains =  m_events[line];
  
  // for ( const auto& ctpIndex : tav ) {
  //   //    ATH_MSG_DEBUG("Enabling chain from CTP ID " << ctpIndex);  
  //   auto itr = m_ctpToChain.find(ctpIndex);
  //   if ( itr != m_ctpToChain.end() ) {
  //     enabledChains.insert( enabledChains.end(), itr->second.begin(), itr->second.end() );
  //   }
  //   numberPfActivatedBits++;
  // }

  // for ( auto chain: enabledChains ) {
  //   ATH_MSG_DEBUG( "Enabling chain: " << chain );
  // }
  // if ( numberPfActivatedBits == 0 ) {
  //   ATH_MSG_ERROR( "All CTP bits were disabled, this event shoudl not have shown here" );
  //   return StatusCode::FAILURE;
  // }

  return StatusCode::SUCCESS;
}


StatusCode CTPUnpackingEmulationTool::finalize()
{
  return StatusCode::SUCCESS;
}

