/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArFEBTemp2Ntuple.h"
#include "LArElecCalib/ILArFEBTempTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"

LArFEBTemp2Ntuple::LArFEBTemp2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): LArCond2NtupleBase(name, pSvcLocator) {

   m_ntTitle="FEB Temperatures";
   m_ntpath="/NTUPLES/FILE1/FEBTEMP";

}

LArFEBTemp2Ntuple::~LArFEBTemp2Ntuple() 
{}

StatusCode LArFEBTemp2Ntuple::stop() {
  StatusCode sc;
  NTuple::Item<float> temp1;
  NTuple::Item<float> temp2;
 
   sc=m_nt->addItem("temp1",temp1,-1000.,5000.);
   if (sc!=StatusCode::SUCCESS)
     {(*m_log) << MSG::ERROR << "addItem 'temp1' failed" << endmsg;
	return StatusCode::FAILURE;
     }
   
   sc=m_nt->addItem("temp2",temp2,-1000.,5000.);
   if (sc!=StatusCode::SUCCESS)
     {(*m_log) << MSG::ERROR << "addItem 'temp2' failed" << endmsg;
	return StatusCode::FAILURE;
     }
   
   IToolSvc* toolSvc=0;
   sc = service( "ToolSvc",toolSvc);
   if (sc!=StatusCode::SUCCESS) 
     {
	(*m_log)  << MSG::ERROR << "Unable to retrieve IToolSvc"
	  << endmsg;
     }   

   ILArFEBTempTool *larFEBTempTool;
   sc = toolSvc->retrieveTool("LArFEBTempTool", larFEBTempTool);
   if (sc!=StatusCode::SUCCESS) {
      (*m_log)  << MSG::ERROR << "Unable to retrieve LArFEBTempTool from ToolSvc" << endmsg;
      return StatusCode::FAILURE;
   } 

   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();

   for(; itOnId!=itOnIdEnd;++itOnId)
     {
	const HWIdentifier hwid = *itOnId;
	FEBTemp tv = larFEBTempTool->getFebTemp(hwid);
        (*m_log)  << MSG::DEBUG << hwid << " " << tv.size()<<endmsg;
	
	if( !tv.empty() )
	  {	     	 
	     FEBTemp::const_iterator itb = tv.begin();
	     FEBTemp::const_iterator ite = tv.end();
	     
	     for(;itb!=ite;++itb)
	       {		  
		  temp1 = (*itb).first;
		  temp2 = (*itb).second;
		  
		  fillFromIdentifier(hwid);
	     
		  sc=ntupleSvc()->writeRecord(m_nt);
	      
		  if (sc!=StatusCode::SUCCESS) 
		    {
		       (*m_log) << MSG::ERROR << "writeRecord failed" << endmsg;
		       return StatusCode::FAILURE;
		    }
	       }	     
	  }	
     }
   
   (*m_log) << MSG::INFO << "LArFEBTemp2Ntuple has finished." << endmsg;
   
   return StatusCode::SUCCESS;
   
}// end finalize-method.
   
