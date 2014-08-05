/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBTDCRaw.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "TBEvent/TBTDCRawCont.h"
#include <vector>

CBNTAA_TBTDCRaw::CBNTAA_TBTDCRaw(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator)
{
  declareProperty("NeverReturnFailure", m_neverReturnFailure=false);
	declareProperty("ContainerKey1",m_containerKey1="TDCRawCont");
	m_tdc=NULL;
	m_underThreshold=NULL;
        m_eventStore = 0;
}

CBNTAA_TBTDCRaw::~CBNTAA_TBTDCRaw() 
{//Clean up arrays of ntuple entries (if they have been booked)
	if (m_tdc)
    delete m_tdc;
	if (m_underThreshold)
    delete m_underThreshold;
}

StatusCode CBNTAA_TBTDCRaw::CBNT_initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << "in initialize()" 
      << endreq;

 StatusCode sc = service("StoreGateSvc", m_eventStore);
  if (sc.isFailure())
    {
      log << MSG::ERROR
	  << "Unable to retrieve pointer to StoreGate Service"
	  << endreq;
      return sc;
    }

  IToolSvc* toolSvc;
  sc=service( "ToolSvc",toolSvc  );
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) 
    {
      log << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
      return StatusCode::FAILURE;
    } 

  addBranch("TBTDCRaw",m_tdc);
  addBranch("TBTDCRaw_underThreshold",m_underThreshold);

  return StatusCode::SUCCESS; 
  
}

StatusCode CBNTAA_TBTDCRaw::CBNT_execute()
{
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  
  StatusCode sc;
  
  const TBTDCRawCont * tdcCont;
	sc = m_eventStore->retrieve(tdcCont,m_containerKey1);
  if (sc.isFailure()) 
    {
      log << MSG::ERROR << "\033[31m" << " Cannot read TBTDCRawCont from StoreGate! key= " << m_containerKey1 << "\033[0m" << endreq;
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    {
			log << MSG::DEBUG << "\033[31m" << "Going over TBTDCRawCont channels ..."<< "\033[0m" <<endreq;

			const unsigned nTDC = (TBTDCRawCont::size_type)tdcCont->size();
			m_tdc->resize(nTDC);
			m_underThreshold->resize(nTDC);

			unsigned NtupleVectorIndex = 0;
      TBTDCRawCont::const_iterator it_tdc = tdcCont->begin();
      TBTDCRawCont::const_iterator last_tdc = tdcCont->end();
      for(;it_tdc!=last_tdc;it_tdc++,NtupleVectorIndex++) {
				const TBTDCRaw * tdc = (*it_tdc);
				(*m_tdc)[NtupleVectorIndex] = tdc->getTDC();
				(*m_underThreshold)[NtupleVectorIndex] = tdc->isUnderThreshold();
			}
    }

  return StatusCode::SUCCESS;
}  

StatusCode CBNTAA_TBTDCRaw::CBNT_clear()
{
  if (m_tdc)
     m_tdc->clear();
  if (m_underThreshold)
     m_underThreshold->clear();

  return StatusCode::SUCCESS;
}

StatusCode CBNTAA_TBTDCRaw::CBNT_finalize()
{
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << "in finalize()" 
      << endreq;
  
  return StatusCode::SUCCESS;
}

std::string CBNTAA_TBTDCRaw::add_name(const char* base, const std::string extension) {
  std::string retval(base);
  for (unsigned i=0;i<extension.size();i++) {
    const char& ch=extension[i];
    if (ch=='=' || ch==':' || ch=='/')
      continue; //Inore these characters
    else if (ch=='-')
      retval.append("m");//replace by letter m
    else if (ch=='+')
      retval.append("p");//replace by letter p
    else
      retval.append(1,ch);
  }
  return retval;
}
