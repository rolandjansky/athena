
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "LArRecEvent/LArNoisyROSummary.h"

#include "TrigLArNoiseBurstHypoTool.h"


using namespace TrigCompositeUtils;

TrigLArNoiseBurstHypoTool::TrigLArNoiseBurstHypoTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_noisyROTool("LArNoisyROTool",this),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
	declareProperty( "Tool",m_noisyROTool);
}

StatusCode TrigLArNoiseBurstHypoTool::initialize()  {

  
  // prepare mask
  m_mask = 0x0;
  if ( m_badFEBFlaggedPartitions ) m_mask|=0x1;
  if ( m_satTightFlaggedPartitions ) m_mask|=0x2;
  if ( m_mNBLooseFlaggedPartitions ) m_mask|=0x10;
  if ( m_mNBTightFlaggedPartitions ) m_mask|=0x20;
  if ( m_mNBTight_PsVetoFlaggedPartitions ) m_mask|=0x40;
  ATH_CHECK(m_noisyROTool.retrieve());
  
  ATH_MSG_INFO("TrigLArNoiseBurstHypoTool initialization completed successfully.");
 
  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode TrigLArNoiseBurstHypoTool::decide( std::vector<CaloCellNoiseInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
    addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

bool TrigLArNoiseBurstHypoTool::decide( const ITrigLArNoiseBurstHypoTool::CaloCellNoiseInfo& input ) const {

  bool monitor(true);
  if ( m_monTool.name().empty() ) monitor=false;
  auto timer = Monitored::Timer("TIME_larnoisetool");
  auto mon = Monitored::Group(m_monTool,timer);
  std::string bitWise_flags("bitWise_flags");
  
  
  // no cells, no discussion
  if ( !input.cells ) return false;
  unsigned int flag = 0;
  bool pass=false;
  ATH_MSG_DEBUG ("Got cell container, will process it");
  std::unique_ptr<LArNoisyROSummary> noisyRO = m_noisyROTool->process(input.cells, input.knownBadFEBs, input.knownMNBFEBs);
  ATH_MSG_DEBUG("processed it");
  if ( monitor ){
    auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"Input");
    fill(m_monTool,bitWise); 
  }
  if ( noisyRO->BadFEBFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : BadFEBFlaggedPartitions");
        flag |= 0x1;
	if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"BadFEBFlaggedPartitions");
          fill(m_monTool,bitWise); 
	}
  }
  if ( noisyRO->BadFEB_WFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : BadFEB_WFlaggedPartitions");
        flag |= 0x8;
	if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"BadFEB_WFlaggedPartitions");
          fill(m_monTool,bitWise); 
	}
  }
  if ( noisyRO->SatTightFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : SatTightFlaggedPartitions");
        flag |= 0x2;
	if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"SatTightFlaggedPartitions");
          fill(m_monTool,bitWise); 
	}
  }
  if ( noisyRO->MNBLooseFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : MNBLooseFlaggedPartions");
        flag |= 0x10;
	if ( monitor ) {
	  auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"MNBLooseFlaggedPartions");
          fill(m_monTool,bitWise); 
	}
  }
  if ( noisyRO->MNBTightFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : MNBTightFlaggedPartions");
        flag |= 0x20;
	if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"MNBTightFlaggedPartions");
          fill(m_monTool,bitWise); 
	}
  }

  ATH_MSG_DEBUG("got the flag : " << (unsigned int)flag);
  // hope it is ok to accumulate multiple hypothesis here.
  input.flag |= flag;

  if ( (flag & m_mask) != 0x0 ) {
        ATH_MSG_DEBUG("LAr Noise detected : ");
        pass = true;
	if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"Output");
          fill(m_monTool,bitWise); 
	}
  }
        else ATH_MSG_DEBUG("LAr Noise not detected!");
  return pass;
}
