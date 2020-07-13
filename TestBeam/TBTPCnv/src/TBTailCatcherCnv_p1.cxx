/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTailCatcherCnv_p1.h"
#include "TBEvent/TBTailCatcher.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTailCatcher_p1.h"

// additional include needed because TBTailCatcher inherits from TBScintillator and has a TBScintillatorCont
#include "TBEvent/TBScintillatorCont.h"


void
TBTailCatcherCnv_p1::persToTrans(const TBTailCatcher_p1* pers, 
                                     TBTailCatcher* trans, MsgStream &/*log*/) const
{

  const unsigned nScints = pers->m_signal.size();

	// clear the trans contained and reserve space for the new scints
  trans->clear();
  trans->reserve(nScints);

  // copy all the scints from the pers to the trans
  for (unsigned nScintNow=0; nScintNow<nScints; nScintNow++) {
    TBScintillator * scint = new TBScintillator();

    // fill in the scint properties
    scint -> setSignal(         pers->m_signal[nScintNow] );
    scint -> setSignalOverflow( pers->m_signal_overflow[nScintNow] );
    scint -> setTimeSignal(     pers->m_time_signal[nScintNow] );
    scint -> setTimeOverflow(   pers->m_time_overflow[nScintNow] );
    scint -> setDetectorName(   pers->m_tbDetectorName[nScintNow] );
    scint -> setOverflow(       pers->m_overflow[nScintNow] );

    // fill the container with the scint object
    trans->addScintillator(scint);

//		log << MSG::DEBUG << "\033[34m" << "\t- pers->trans:  scint->isOverflow()       " << scint->isOverflow() << "\033[0m" <<endmsg;
  }

  unsigned int nSignals=pers->m_signals.size();
  trans->m_signals.reserve(nSignals);
  trans->m_signals = pers->m_signals;

}


void
TBTailCatcherCnv_p1::transToPers(const TBTailCatcher* trans, 
                                     TBTailCatcher_p1* pers, MsgStream &/*log*/) const
{

  const unsigned nScints = trans->size();

  pers -> m_signal.reserve(nScints);
  pers -> m_time_signal.reserve(nScints);
  pers -> m_signal_overflow.reserve(nScints);
  pers -> m_time_overflow.reserve(nScints);
  pers -> m_tbDetectorName.reserve(nScints);
  pers -> m_overflow.reserve(nScints);

  // iterators for the container
  TBScintillatorCont::const_iterator scintIt   = trans->begin();
  TBScintillatorCont::const_iterator scintIt_e = trans->end();

  // copy all the scints from the trans to the pers
  for (; scintIt!=scintIt_e; ++scintIt) {
    const TBScintillator * scint = * scintIt;

    // fill in the scint properties
    pers -> m_signal.push_back(          scint->getSignal() );
    pers -> m_signal_overflow.push_back( scint->isSignalOverflow() );
    pers -> m_time_signal.push_back(     scint->getTimeSignal() );
    pers -> m_time_overflow.push_back(   scint->isTimeOverflow() );
    pers -> m_tbDetectorName.push_back(  scint->getDetectorName() );
    pers -> m_overflow.push_back(        scint->isOverflow() );
	}

  pers->m_signals = trans->getSignals();
}

