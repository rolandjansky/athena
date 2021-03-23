/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CBNTAA_TBADCRaw.h"

#include "TBEvent/TBADCRawCont.h"
#include <vector>

CBNTAA_TBADCRaw::CBNTAA_TBADCRaw(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator)
{
  declareProperty("NeverReturnFailure", m_neverReturnFailure=false);
  declareProperty("ContainerKey1",m_containerKey1="ADCRawCont");
  m_adc=NULL;
}

CBNTAA_TBADCRaw::~CBNTAA_TBADCRaw() 
{//Clean up arrays of ntuple entries (if they have been booked)
  delete m_adc;
}

StatusCode CBNTAA_TBADCRaw::CBNT_initialize()
{
  ATH_MSG_DEBUG ( "in initialize()" );

  addBranch("TBADCRaw",m_adc);
  return StatusCode::SUCCESS; 
  
}

StatusCode CBNTAA_TBADCRaw::CBNT_execute()
{
  const TBADCRawCont * adcCont;
  StatusCode sc = evtStore()->retrieve(adcCont,m_containerKey1);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR ( "\033[31m" << " Cannot read TBADCRawCont from StoreGate! key= " << m_containerKey1 << "\033[0m" );
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    {
      ATH_MSG_DEBUG ( "\033[31m" << "Going over TBADCRawCont channels ..."<< "\033[0m" );

      const unsigned nADC = (TBADCRawCont::size_type)adcCont->size();
      m_adc->resize(nADC);

      unsigned NtupleVectorIndex = 0;
      for (const TBADCRaw* adc : *adcCont) {
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
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}

std::string CBNTAA_TBADCRaw::add_name(const char* base, const std::string& extension) {
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
