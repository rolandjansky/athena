/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBScintillatorRaw.h"

#include "TBEvent/TBBeamDetector.h"



TBScintillatorRaw::TBScintillatorRaw(const std::string& scintillatorName,
		 const TBTDCRawCont* tdcCont,TBTDCRaw* tbtdc,
		 const TBADCRawCont* adcCont,TBADCRaw* tbadc)
  : TBBeamDetector(scintillatorName)
{
  m_tdclink.toContainedElement(*tdcCont,tbtdc);
  m_adclink.toContainedElement(*adcCont,tbadc);    
}

void TBScintillatorRaw::setSignals(const TBTDCRawCont* tdcCont,TBTDCRaw* tbtdc,
				   const TBADCRawCont* adcCont,TBADCRaw* tbadc){
  m_tdclink.toContainedElement(*tdcCont,tbtdc);
  m_adclink.toContainedElement(*adcCont,tbadc);    
}

