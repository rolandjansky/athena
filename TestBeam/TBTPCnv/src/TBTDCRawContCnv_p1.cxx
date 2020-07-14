/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTDCRawContCnv_p1.h"
#include "TBEvent/TBTDCRawCont.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTDCRawCont_p1.h"



void
TBTDCRawContCnv_p1::persToTrans(const TBTDCRawCont_p1* pers, 
                                     TBTDCRawCont* trans, MsgStream &/*log*/) const
{

	const unsigned nTDCRaws = pers->m_tdc.size();

	// clear the trans contained and reserve space for the new scints
  trans->clear();
  trans->reserve(nTDCRaws);

	// copy all the scints from the pers to the trans
  for (unsigned nTDCRawNow=0; nTDCRawNow<nTDCRaws; nTDCRawNow++) {
    TBTDCRaw * TDCRaw = new TBTDCRaw();

		// fill in the scint properties
    TDCRaw -> setTDC(            pers->m_tdc[nTDCRawNow] );
    TDCRaw -> setUnderThreshold( pers->m_underThreshold[nTDCRawNow] );
    TDCRaw -> setDetectorName(   pers->m_tbDetectorName[nTDCRawNow] );
    TDCRaw -> setOverflow(       pers->m_overflow[nTDCRawNow] );

    // fill the container with the scint object
    trans->push_back(TDCRaw);
	}
}


void
TBTDCRawContCnv_p1::transToPers(const TBTDCRawCont* trans, 
                                     TBTDCRawCont_p1* pers, MsgStream &/*log*/) const
{

  const unsigned nTDCRaws = trans->size();

	pers -> m_tdc.reserve(nTDCRaws);
	pers -> m_underThreshold.reserve(nTDCRaws);
  pers -> m_tbDetectorName.reserve(nTDCRaws);
  pers -> m_overflow.reserve(nTDCRaws);

	// iterators for the container
  TBTDCRawCont::const_iterator TDCRawIt   = trans->begin();
  TBTDCRawCont::const_iterator TDCRawIt_e = trans->end();

  // copy all the scints from the trans to the pers
  for (; TDCRawIt!=TDCRawIt_e; ++TDCRawIt) {
    const TBTDCRaw * TDCRaw = * TDCRawIt;

		pers -> m_tdc.push_back(            TDCRaw->getTDC() );
		pers -> m_underThreshold.push_back( TDCRaw->isUnderThreshold() );
		pers -> m_tbDetectorName.push_back( TDCRaw->getDetectorName() );
    pers -> m_overflow.push_back(       TDCRaw->isOverflow() );

	}
  
}

