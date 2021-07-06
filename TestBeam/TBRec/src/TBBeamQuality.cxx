/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
                                 

#include "TBBeamQuality.h"
#include "TBBeamQualityTool.h"
#include "TBEvent/TBEventInfo.h"


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
  for (ToolHandle<TBBeamQualityTool>& tool : m_bqtools)
    { 
      toolCtr ++;
      ATH_MSG_INFO 
        (" Tool Name: "
	 <<tool->name()
	 <<" Tool Type: "
	 <<tool->type() );
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
  
  // setting event to success
  bool successFlag = true;
  for (ToolHandle<TBBeamQualityTool>& tool : m_bqtools)
    {
      m_bqtotalCounter[tool]++;
      if( tool->accept(m_bqparticle) == StatusCode::SUCCESS )
	{
	  m_bqacceptCounter[tool]++;
	}
      else
	{
	  m_bqrejectCounter[tool]++;
          successFlag = false;
          break;
	}
    }
  
  setFilterPassed(successFlag);
  return StatusCode::SUCCESS;
} //execute

/*---------------FINALIZE---------------*/
StatusCode TBBeamQuality::finalize()
{ //finalize
  // print summary
  ATH_MSG_INFO
    ( "************ Beam Quality Tool ************" );
  for (ToolHandle<TBBeamQualityTool>& tool : m_bqtools)
    {
      ATH_MSG_INFO
        (tool->name() <<" : "
	  << "(Total/Accept/Rejected)"
	  << " ("
	  << m_bqtotalCounter[tool]  << "/"
	  << m_bqacceptCounter[tool] << "/"
	  << m_bqrejectCounter[tool] 
          << ")" );
    } 
  return StatusCode::SUCCESS;
} //finalize



