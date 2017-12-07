/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
/// Author: Guillaume Unal
/// Date:   december 2007
/// See HitRdoOverlayExample.h for details
/////////////////////////////////////////////////////////////

#include "HitRdoOverlayExample.h"

#include "EventInfo/PileUpTimeEventIndex.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"

//Constructor

HitRdoOverlayExample::HitRdoOverlayExample(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_mergeSvc("PileUpMergeSvc", name),
  m_nevt(0)
{

}

//Destructor

HitRdoOverlayExample::~HitRdoOverlayExample()
{
  ATH_MSG_DEBUG ( "HitRdoOverlayExample destructor called" );
}

//__________________________________________________________________________

StatusCode HitRdoOverlayExample::initialize()
{
  ATH_MSG_DEBUG ("HitRdoOverlayExample initialize()" );

  CHECK(m_mergeSvc.retrieve());
  m_nevt=0;
  return StatusCode::SUCCESS;

}

//__________________________________________________________________________
StatusCode HitRdoOverlayExample::finalize()
{


  ATH_MSG_DEBUG ( "HitRdoOverlayExample finalize()" );

  return StatusCode::SUCCESS;

}

//__________________________________________________________________________
StatusCode HitRdoOverlayExample::execute()
{


  ATH_MSG_DEBUG ( "HitRdoOverlayExample execute()" );

  m_nevt++;

  if ((m_nevt%100)==1) {
    ATH_MSG_INFO ( " ***** Event " << m_nevt );
}
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

      if (!(m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer]

                                            ,hitContList).isSuccess()) && hitContList.size()==0) {

        ATH_MSG_WARNING ( " cannot retrieve hit container " );

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

            ATH_MSG_INFO ( " hit subevt time " << SubEvtTimOffset );

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

  ATH_MSG_INFO ( " Number of LAr hits read " << nhit_tot );

  // get LArDigit

  int ndigit_tot=0;

  typedef PileUpMergeSvc::TimedList<LArDigitContainer>::type TimedDigitContList;

  TimedDigitContList digitContList;

  if ( m_mergeSvc->retrieveSubEvtsData("LArDigitContainer_MC",digitContList).isFailure() ) {
    ATH_MSG_WARNING ( " cannot retrieve digit container " );
  }
  else
    {
      ATH_MSG_INFO ( "DigititContainer size is " << digitContList.size() );

      TimedDigitContList::iterator iFirstCont(digitContList.begin());

      TimedDigitContList::iterator iEndCont(digitContList.end());

      double SubEvtTimOffset;

      while (iFirstCont != iEndCont) {

        // get time for this subevent

        const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);

        SubEvtTimOffset = time_evt->time();

        ATH_MSG_INFO ( " digit subevt time " << SubEvtTimOffset );

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

  ATH_MSG_INFO ( " Number of LAr digits read " << ndigit_tot );

  return StatusCode::SUCCESS;

}
