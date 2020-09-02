// DerivationFrameworkCore includes
#include "GoodRunsListFilterAlgorithm.h"

#include "xAODEventInfo/EventInfo.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "GoodRunsLists/TGoodRunsListReader.h"
#include "PathResolver/PathResolver.h"

namespace DerivationFramework {

GoodRunsListFilterAlgorithm::GoodRunsListFilterAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ), 
  m_grlTool("GoodRunsListSelectionTool/grlTool",this), 
  m_brlTool("GoodRunsListSelectionTool/brlTool",this)
{

  declareProperty( "GoodRunsListVec", m_goodRunsListVec, "Good Runs Lists files ... will pass events" );
  declareProperty( "BlackRunsListVec", m_blackRunsListVec, "Black Runs Lists files ... will veto events" );

}


GoodRunsListFilterAlgorithm::~GoodRunsListFilterAlgorithm() {}


StatusCode GoodRunsListFilterAlgorithm::initialize() {
  
  //configure the tools
  CHECK( AAH::setProperty( m_grlTool , "GoodRunsListVec", m_goodRunsListVec ) );
  CHECK( AAH::setProperty( m_brlTool , "GoodRunsListVec", m_blackRunsListVec ) );

  //determine the maxGRLRunNumber ...
  if(m_goodRunsListVec.size()) {
    CHECK( m_grlTool.retrieve() ); //will check the files are ok ..

    Root::TGoodRunsListReader reader;
    for(auto& f : m_goodRunsListVec) {
      std::string path = PathResolverFindXMLFile( f );
      if(path=="") path = PathResolverFindCalibFile( f );
      if(path=="") continue; //skip this file .. should never happen
      reader.AddXMLFile( path );
    }

    if(!reader.Interpret()) {
      ATH_MSG_ERROR("Failed to interpret GRLs ... this should never happen");
      return StatusCode::FAILURE;
    }

    std::vector<int> runs = reader.GetMergedGoodRunsList().GetRunlist();
    m_maxGRLRunNumber = *max_element( runs.begin(), runs.end() );

    ATH_MSG_INFO("Max GRL RunNumber = " << m_maxGRLRunNumber << ". Data in later runs will pass GRL check");

  }


  return StatusCode::SUCCESS;
}

StatusCode GoodRunsListFilterAlgorithm::finalize() {
  


  return StatusCode::SUCCESS;
}

StatusCode GoodRunsListFilterAlgorithm::execute() {  
  //This algorithm decorates EventInfo with passDFGRL to indicate if
  //GRL&&!BRL requirement has passed or failed
  //The algorithm is also marked as not passing if it failed
  //
  //Any event that has a runNumber > the biggest runNumber in the GRLs
  //will be passed (unless it fails the BRL requirement)


  const xAOD::EventInfo* ei = 0;
  CHECK( evtStore()->retrieve( ei, "EventInfo" ) );

  bool decision(true);
  if( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
    decision=true; //pass for MC
  } else if( m_blackRunsListVec.size()>0 && m_brlTool->passRunLB( *ei ) ) {
    decision=false; //fail because in black list
  } else if(ei->runNumber() > m_maxGRLRunNumber) {
    decision=true; //pass because run number is past end of GRL
  } else if( m_goodRunsListVec.size()==0 || m_grlTool->passRunLB( *ei ) ) {
    decision=true; //pass: either there is no GRL given, or the event is in the GRL
  } else {
    decision=false; //event is not in the GRL
  }

  setFilterPassed(decision);

  static SG::AuxElement::Decorator< char > decEventInfo_passDFGRL("passDFGRL");
  decEventInfo_passDFGRL( *ei ) = decision;

  ATH_MSG_VERBOSE("passDFGRL = " << decision);

  return StatusCode::SUCCESS;
}

} //end of namespace
