/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <xAODTrigger/TrigCompositeAuxContainer.h>
#include <TrigSteeringEvent/TrigRoiDescriptorCollection.h>
#include <DecisionHandling/TrigCompositeUtils.h>
#include "TestRecoAlg.h"



namespace HLTTest {

  TestRecoAlg::TestRecoAlg( const std::string& name, 
			    ISvcLocator* pSvcLocator ) : 
    ::AthAlgorithm( name, pSvcLocator )
  {}
  

  TestRecoAlg::~TestRecoAlg() {}

  void split(const std::string& src, char delim, std::vector<std::string>& result) {
    std::istringstream i(src);
    std::string element;
    while ( getline(i, element, delim) ) {
      result.push_back(element);
    }
  }
  std::string trim(const std::string& str) {
    size_t firstNonSpace = str.find_first_not_of(' ');
    size_t lastNonSpace = str.find_last_not_of(' ');
    return str.substr( firstNonSpace, lastNonSpace-firstNonSpace+1 );
  }
  
  StatusCode TestRecoAlg::initialize() {    
    ATH_MSG_INFO ("Initializing " << name() << " reading input file " << m_fileName);


    CHECK( m_output.initialize() );
    CHECK( m_input.initialize( not m_input.key().empty() ) );
    std::ifstream inputFile( m_fileName );
    std::string line;
    typedef std::vector<std::string> Split_t;
  
    while (  getline(inputFile, line) ) {
      if ( line[0] == '#' ) continue;
      Split_t  objects;
      Event_t event;
      split( line, ';', objects );
      for ( auto obj: objects ) {
	Obj_t object;
	Split_t properties;
	split( obj, ',', properties );
	for ( auto prop: properties ) {
	  Split_t keyval;
	  split( prop, ':', keyval );
	  if ( keyval.size() != 2  ) {
	    ATH_MSG_ERROR("Pnput format error, property specification invalid, should be a:b " << prop );
	    return StatusCode::FAILURE;	  
	  }
	  object.push_back( std::make_pair( trim ( keyval[0] ), std::stof(keyval[1]) ) );	
	}
	if ( object.size() != 0 ) 
	  event.push_back(object);
      }
      m_data.push_back(event);
    }
    ATH_MSG_DEBUG( "Loaded " << m_data.size() << " pseudo events" );
    if ( m_data.size() == 0 ) {
      ATH_MSG_ERROR( "Can not run with zero pseudo events" );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TestRecoAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode TestRecoAlg::execute() {  
    using namespace TrigCompositeUtils;
    ATH_MSG_DEBUG ("Executing " << name() << "...");

    const EventContext& context = Gaudi::Hive::currentContext();
    const size_t eventNo = context.evt() % m_data.size();
    auto objects= m_data[eventNo];

    auto output = std::make_unique<xAOD::TrigCompositeContainer>();
    auto aux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
    output->setStore( aux.get() );

    
    auto inputHandle = SG::makeHandle(m_input);
    ATH_MSG_DEBUG("Input " << m_input.key() << " has "<<inputHandle->size() <<" elements, scanning it");
    for ( auto i: *inputHandle.cptr() ) {     
      auto featureInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( i, "initialRoI"  );
      auto roiLink = featureInfo.link;
      CHECK( roiLink.isValid() );
      if ( roiLink.isValid() ) {
	auto roiPtr(roiLink.cptr());
	ATH_MSG_DEBUG("RoI" << **roiPtr );
	// create new outpu objects and add the properties
	if (objects.size() > output->size()) {
	  auto object=objects[output->size()];
	  auto xobj = new xAOD::TrigComposite;
	  output->push_back( xobj );
	  // maintain link to previous collections
	  xobj->setObjectLink( "initialRoI", roiLink );// this is used by the HypoAlg

	  ATH_MSG_DEBUG( "Reconstructed object" );
	  for ( auto prop : object )  {
	    xobj->setDetail( prop.first, prop.second );
	    ATH_MSG_DEBUG( "  " << prop.first << " : " << prop.second );
	  }
	}
	else {
	  ATH_MSG_DEBUG( "No reco object created for this RoI because it's not found in the event");
	}
      } else {
	ATH_MSG_DEBUG("RoI information missing");
      }
    }
  

    ATH_MSG_DEBUG("Reconstructed "<<output->size() <<" objects");
  
    auto handle = SG::makeHandle(m_output);
    CHECK( handle.record( std::move(output), std::move(aux) ) );

    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest
