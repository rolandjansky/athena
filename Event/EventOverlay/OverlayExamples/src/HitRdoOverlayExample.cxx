/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
/// Author: Guillaume Unal
/// Date:   december 2007
/// See HitRdoOverlayExample.h for details
/////////////////////////////////////////////////////////////

#include "OverlayExamples/HitRdoOverlayExample.h"

#include "GaudiKernel/Property.h"

#include "GaudiKernel/MsgStream.h"

#include "EventInfo/PileUpTimeEventIndex.h"

#include "LArSimEvent/LArHit.h"

#include "LArSimEvent/LArHitContainer.h"

#include "LArRawEvent/LArDigit.h"

#include "LArRawEvent/LArDigitContainer.h"

//Constructor

HitRdoOverlayExample::HitRdoOverlayExample(const std::string& name, ISvcLocator* pSvcLocator):

    Algorithm(name,pSvcLocator){

}

//Destructor

HitRdoOverlayExample::~HitRdoOverlayExample()
  {

    MsgStream log( messageService(), name() ) ;

    log << MSG::DEBUG << "HitRdoOverlayExample destructor called" << endreq;

  }

  //__________________________________________________________________________

StatusCode HitRdoOverlayExample::initialize()
  {

    MsgStream log( messageService(), name() );

    log << MSG::DEBUG <<"HitRdoOverlayExample initialize()" << endreq;


    // Get the StoreGateSvc

    if (service("StoreGateSvc", m_sgSvc).isFailure()) {

      log << MSG::ALWAYS << "No StoreGate!!!!!!!" << endreq; 

      return StatusCode::FAILURE;

    }

    if (!(service("PileUpMergeSvc", p_mergeSvc)).isSuccess() ||

       0 == p_mergeSvc) {

     log << MSG::ERROR << "Could not find PileUpMergeSvc" << endreq;

     return StatusCode::FAILURE;

    }

    m_nevt=0;

    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode HitRdoOverlayExample::finalize()
  {

    MsgStream log( messageService(), name() );

    log << MSG::DEBUG << "HitRdoOverlayExample finalize()" << endreq;

    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode HitRdoOverlayExample::execute()
  {

    MsgStream log( messageService(), name() );

    log << MSG::DEBUG << "HitRdoOverlayExample execute()" << endreq;

    m_nevt++;

    if ((m_nevt%100)==1)
       log << MSG::INFO << " ***** Event " << m_nevt << endreq;

    // get LArHit

    std::vector <std::string> m_HitContainer;

    m_HitContainer.push_back("LArHitEMB");

    m_HitContainer.push_back("LArHitEMEC");

    m_HitContainer.push_back("LArHitHEC");

    m_HitContainer.push_back("LArHitFCAL");

    int nhit_tot=0;

    for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)

    {

      typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;

      TimedHitContList hitContList;

      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer]

          ,hitContList).isSuccess()) && hitContList.size()==0) {

        log << MSG::WARNING << " cannot retrieve hit container " << endreq;

      }  

      else

      {

         TimedHitContList::iterator iFirstCont(hitContList.begin());

         TimedHitContList::iterator iEndCont(hitContList.end());

         double SubEvtTimOffset;

         while (iFirstCont != iEndCont) {

            // get time for this subevent

            const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);

            SubEvtTimOffset = time_evt->time();

            log << MSG::INFO << " hit subevt time " << SubEvtTimOffset << endreq;

            // get LArHitContainer for this subevent

            const LArHitContainer& firstCont = *(iFirstCont->second);

            LArHitContainer::const_iterator hititer;

            for(hititer=firstCont.begin();

                hititer != firstCont.end();hititer++)

            {

              //LArHit* hit = (*hititer);

              nhit_tot++;

            }

            iFirstCont++;

         }

      }

    } 

    log << MSG::INFO << " Number of LAr hits read " << nhit_tot << endreq;

    // get LArDigit

    int ndigit_tot=0;

    typedef PileUpMergeSvc::TimedList<LArDigitContainer>::type TimedDigitContList;

    TimedDigitContList digitContList;
   
    if ( p_mergeSvc->retrieveSubEvtsData("LArDigitContainer_MC",digitContList).isFailure() ) {
        log << MSG::WARNING << " cannot retrieve digit container " << endreq; 
    }       
    else    
    {
         log << MSG::INFO << "DigititContainer size is " << digitContList.size() << endreq;

         TimedDigitContList::iterator iFirstCont(digitContList.begin());

         TimedDigitContList::iterator iEndCont(digitContList.end());

         double SubEvtTimOffset;

         while (iFirstCont != iEndCont) {

            // get time for this subevent

            const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);

            SubEvtTimOffset = time_evt->time();

            log << MSG::INFO << " digit subevt time " << SubEvtTimOffset << endreq; 

            const LArDigitContainer& digit_container =  *(iFirstCont->second);

            LArDigitContainer::const_iterator digititer;

            for (digititer=digit_container.begin();

                 digititer!=digit_container.end();digititer++) 

            {

               //LArDigit* digit = (*digititer);

               ndigit_tot++;

            }

            iFirstCont++;

         }

    }

    log << MSG::INFO << " Number of LAr digits read " << ndigit_tot << endreq;

    return StatusCode::SUCCESS;

   } 

