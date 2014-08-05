/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBADCRaw.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "TBEvent/TBADCRawCont.h"
#include <vector>

CBNTAA_TBADCRaw::CBNTAA_TBADCRaw(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator)
{
  declareProperty("NeverReturnFailure", m_neverReturnFailure=false);
	declareProperty("ContainerKey1",m_containerKey1="ADCRawCont");
	m_adc=NULL;
        m_eventStore=0;
}

CBNTAA_TBADCRaw::~CBNTAA_TBADCRaw() 
{//Clean up arrays of ntuple entries (if they have been booked)
	if (m_adc)
    delete m_adc;
}

StatusCode CBNTAA_TBADCRaw::CBNT_initialize()
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

  addBranch("TBADCRaw",m_adc);

  return StatusCode::SUCCESS; 
  
}

StatusCode CBNTAA_TBADCRaw::CBNT_execute()
{
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  
  StatusCode sc;
  
  const TBADCRawCont * adcCont;
	sc = m_eventStore->retrieve(adcCont,m_containerKey1);
  if (sc.isFailure()) 
    {
      log << MSG::ERROR << "\033[31m" << " Cannot read TBADCRawCont from StoreGate! key= " << m_containerKey1 << "\033[0m" << endreq;
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    {
			log << MSG::DEBUG << "\033[31m" << "Going over TBADCRawCont channels ..."<< "\033[0m" <<endreq;

			const unsigned nADC = (TBADCRawCont::size_type)adcCont->size();
			m_adc->resize(nADC);

			unsigned NtupleVectorIndex = 0;
      TBADCRawCont::const_iterator it_adc = adcCont->begin();
      TBADCRawCont::const_iterator last_adc = adcCont->end();
      for(;it_adc!=last_adc;it_adc++,NtupleVectorIndex++) {
				const TBADCRaw * adc = (*it_adc);
				(*m_adc)[NtupleVectorIndex] = adc->getADC();
			}
    }

  return StatusCode::SUCCESS;
}  

StatusCode CBNTAA_TBADCRaw::CBNT_clear()
{
  if (m_adc)
     m_adc->clear();

  return StatusCode::SUCCESS;
}

StatusCode CBNTAA_TBADCRaw::CBNT_finalize()
{
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << "in finalize()" 
      << endreq;
  
  return StatusCode::SUCCESS;
}

std::string CBNTAA_TBADCRaw::add_name(const char* base, const std::string extension) {
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
