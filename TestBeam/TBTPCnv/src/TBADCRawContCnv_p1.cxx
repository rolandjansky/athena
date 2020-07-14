/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBADCRawContCnv_p1.h"
#include "TBEvent/TBADCRawCont.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBADCRawCont_p1.h"



void
TBADCRawContCnv_p1::persToTrans(const TBADCRawCont_p1* pers, 
                                     TBADCRawCont* trans, MsgStream &/*log*/) const
{

	const unsigned nADCRaws = pers->m_adc.size();

	// clear the trans contained and reserve space for the new scints
  trans->clear();
  trans->reserve(nADCRaws);

	// copy all the scints from the pers to the trans
  for (unsigned nADCRawNow=0; nADCRawNow<nADCRaws; nADCRawNow++) {
    TBADCRaw * ADCRaw = new TBADCRaw();

		// fill in the scint properties
    ADCRaw -> setADC(            pers->m_adc[nADCRawNow] );
    ADCRaw -> setDetectorName(   pers->m_tbDetectorName[nADCRawNow] );
    ADCRaw -> setOverflow(       pers->m_overflow[nADCRawNow] );

    // fill the container with the scint object
    trans->push_back(ADCRaw);
	}

}


void
TBADCRawContCnv_p1::transToPers(const TBADCRawCont* trans, 
                                     TBADCRawCont_p1* pers, MsgStream &/*log*/) const
{

  const unsigned nADCRaws = trans->size();

	pers -> m_adc.reserve(nADCRaws);
  pers -> m_tbDetectorName.reserve(nADCRaws);
  pers -> m_overflow.reserve(nADCRaws);

	// iterators for the container
  TBADCRawCont::const_iterator ADCRawIt   = trans->begin();
  TBADCRawCont::const_iterator ADCRawIt_e = trans->end();

  // copy all the scints from the trans to the pers
  for (; ADCRawIt!=ADCRawIt_e; ++ADCRawIt) {
    const TBADCRaw * ADCRaw = * ADCRawIt;

		pers -> m_adc.push_back(            ADCRaw->getADC() );
		pers -> m_tbDetectorName.push_back( ADCRaw->getDetectorName() );
    pers -> m_overflow.push_back(       ADCRaw->isOverflow() );

	}
  
}

