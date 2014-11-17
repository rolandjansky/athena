/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQuality.cxx                        #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modified: Feb 7/2005                         #
//#####################################################
                                 

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TBEvent/TBEventInfo.h"

#include "TBRec/TBBeamQualityTool.h"
#include "TBRec/TBBeamQuality.h"


TBBeamQuality::TBBeamQuality(const std::string& name,  
			     ISvcLocator* pService)
  : AthAlgorithm(name,pService)
{ //properties
  declareProperty("BQParticles", m_bqparticle=std::vector<std::string>(1,"noselect"));
  declareProperty("BQToolNames", m_bqtools);
} //properties

TBBeamQuality::~TBBeamQuality()
{ }

/*--------------- INITIALIZE ---------------*/

StatusCode TBBeamQuality::initialize()
{//init
  // message service
  ATH_MSG_DEBUG ("Initialize");
  
  ATH_CHECK( m_bqtools.retrieve() );

  // check tools
  if ( m_bqtools.size() == 0 ) {
    ATH_MSG_ERROR ( "TBBeamQuality: No Tools found" );
      return StatusCode::FAILURE;
  } else {
     for(unsigned int i = 0; i < m_bqtools.size(); ++i) {
        m_bqacceptCounter[m_bqtools[i]] = 0;
        m_bqrejectCounter[m_bqtools[i]] = 0;    
        m_bqtotalCounter[m_bqtools[i]] =0;
     }
  }
  
  // print out list of tools
  ATH_MSG_INFO (" ");
  ATH_MSG_INFO (" TBBeamQuality: List of Tools:" );
  ATH_MSG_INFO  ("-----------------------------------");
  
  unsigned int toolCtr = 0;
  ToolHandleArray<TBBeamQualityTool>::iterator first_tool = m_bqtools.begin();
  ToolHandleArray<TBBeamQualityTool>::iterator last_tool = m_bqtools.end();
  for ( ;first_tool != last_tool; first_tool++)
    { 
      toolCtr ++;
      ATH_MSG_INFO 
        (" Tool Name: "
	 <<(*first_tool)->name()
	 <<" Tool Type: "
	 <<(*first_tool)->type() );
    }
  return StatusCode::SUCCESS;
}//init

/*---------------EXECUTE---------------*/
StatusCode TBBeamQuality::execute()
{ // execute
  ATH_MSG_DEBUG ("Execute" );
  
  if(m_bqparticle[0]=="noselect") { //obtain particle type
    // determining particel type
    const TBEventInfo* eventInfo;
    StatusCode sc = evtStore()->retrieve(eventInfo);
    if (sc.isFailure())
    {
      ATH_MSG_ERROR ( "TBBeamQuality Algo Tool: Could not retrieve event info" );
    }
    std::string particle=eventInfo->getBeamParticle();
    m_bqparticle=std::vector<std::string>(1,particle);
    ATH_MSG_INFO("Beam Quality selected default particle: "<<m_bqparticle[0]);
  }
 
  // invoke tools
  ToolHandleArray<TBBeamQualityTool>::iterator first_Tool = m_bqtools.begin();
  ToolHandleArray<TBBeamQualityTool>::iterator last_Tool = m_bqtools.end();
  
  // setting event to success
  bool successFlag = true;
  while ( successFlag && first_Tool != last_Tool )
    {
      m_bqtotalCounter[*first_Tool]++;
      successFlag = (*first_Tool)->accept(m_bqparticle) == StatusCode::SUCCESS;
      if ( successFlag )
	{
	  m_bqacceptCounter[*first_Tool]++;
	}
      else
	{
	  m_bqrejectCounter[*first_Tool]++;
	}
      first_Tool++;
    }
  
  setFilterPassed(successFlag);
  return StatusCode::SUCCESS;
} //execute

/*---------------FINALIZE---------------*/
StatusCode TBBeamQuality::finalize()
{ //finalize
  // print summary
  ToolHandleArray<TBBeamQualityTool>::iterator first_Tool = m_bqtools.begin();
  ToolHandleArray<TBBeamQualityTool>::iterator last_Tool = m_bqtools.end();
  
  ATH_MSG_INFO
    ( "************ Beam Quality Tool ************" );
  for (; first_Tool != last_Tool; first_Tool++ )
    {
      ATH_MSG_INFO
        ((*first_Tool)->name() <<" : "
	  << "(Total/Accept/Rejected)"
	  << " ("
	  << m_bqtotalCounter[*first_Tool]  << "/"
	  << m_bqacceptCounter[*first_Tool] << "/"
	  << m_bqrejectCounter[*first_Tool] 
          << ")" );
    } 
  return StatusCode::SUCCESS;
} //finalize



