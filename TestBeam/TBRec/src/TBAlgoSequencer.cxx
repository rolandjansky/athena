/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "TBRec/TBAlgoSequencer.h"

#include <string>
#include <map>
#include <iostream>

/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBAlgoSequencer::TBAlgoSequencer(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
  , m_timingOn(true)
  , m_eventPrintFreq(100)
  , m_eventCounter(0)
  , m_rejectCounter(0)
  , m_rejectNoEvent(0)
 {
  // job options
  declareProperty("SubAlgorithms",  m_subAlgoNames);
  declareProperty("PrintFrequency", m_eventPrintFreq);
  declareProperty("TimingOn", m_timingOn);
}

TBAlgoSequencer::~TBAlgoSequencer()
{ }

////////////////////
// Initialization //
////////////////////

StatusCode
TBAlgoSequencer::initialize()
{
  ATH_MSG_INFO ( "initialize..." );

  /////////////////////////////////////////////
  // Register and Initialize (Sub)Algorithms //
  /////////////////////////////////////////////

  m_algoNameStore.resize(m_subAlgoNames.size());
  m_subAlgos.resize(m_subAlgoNames.size());

  std::vector<std::string>::iterator subAlgos = m_subAlgoNames.begin();
  StatusCode registerAlgs;
  unsigned int numberOfAlgorithms = 0;
  unsigned int acceptedAlgos = 0;
  while ( numberOfAlgorithms < m_subAlgoNames.size() && 
	  ! registerAlgs.isFailure() )
    {
      ListItem   theAlgItem(*subAlgos);
      Algorithm* theAlgo;
      registerAlgs = createSubAlgorithm(theAlgItem.type(),theAlgItem.name(), 
					theAlgo);
      if ( ! registerAlgs.isFailure() && theAlgo != 0 )
	{
	  // store sub-algo
	  std::string myName             = theAlgItem.name(); 
	  m_subAlgos[acceptedAlgos]      = theAlgo;
	  m_algoNameStore[acceptedAlgos] = myName;
	  ATH_MSG_INFO
            ( "Subalgorithm ("
	      << std::setw(2)
	      << acceptedAlgos
	      << ") ... created type/name ... "
	      << theAlgItem.type()
	      << "/"
	      << theAlgItem.name() );
	  // set common properties
	  //	  theAlgo->
	  //	    setProperty(StringProperty("EventHeaderKey",m_eventHeaderKey));
	  //	  theAlgo->initialize();
	  // register algorithm with accumulators
	  m_rejectPattern[m_algoNameStore[acceptedAlgos]] = 0;
	  m_acceptPattern[m_algoNameStore[acceptedAlgos]] = 0;
	  // increment accepted algo counter
	  acceptedAlgos++;
	}
      numberOfAlgorithms++;
      subAlgos++;
    }

  ////////////////////////
  // Prepare Statistics //
  ////////////////////////

  m_rejectPattern.clear();
  
  return numberOfAlgorithms > 0
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

/////////////
// Execute //
/////////////

StatusCode
TBAlgoSequencer::execute()
{
  IChronoStatSvc* theTicker = chronoSvc();

  m_timingOn = theTicker != 0 && m_timingOn;

  //////////////////////
  // Event Statistics //
  //////////////////////

  m_eventCounter++;

  if ( m_eventCounter == 1 || m_eventCounter % m_eventPrintFreq == 0 )
    {
      ATH_MSG_INFO
        ( "Number of events processed: "
	  << std::setw(9)
	  << m_eventCounter );
      //	  << ", this event: Run "
      //	  << setw(6)
      //	  << theEvent->getRunNumber()
      //	  << " Event "
      //	  << setw(6)
      //	  << theEvent->getEventNumber()
      //	  << endmsg;
    }

  ////////////////////
  // Algorithm Loop //
  ////////////////////

  AlgoIterator algoCounter = m_subAlgos.begin();
  StatusCode executeAlgs;

  unsigned int algoIndex = 0;
  while ( ! executeAlgs.isFailure() && algoCounter != m_subAlgos.end() )
    {
      // execute the algorithm
      if ( m_timingOn ) theTicker->chronoStart(m_algoNameStore[algoIndex]);
      executeAlgs = (*algoCounter)->execute();
      if ( m_timingOn ) theTicker->chronoStop(m_algoNameStore[algoIndex]);
      // failure/reject
      if ( executeAlgs.isFailure() )
	{
	  m_rejectPattern[m_algoNameStore[algoIndex]]++;
	  m_rejectCounter++;
	}
      else
	{
	  m_acceptPattern[m_algoNameStore[algoIndex]]++;
	}
      // iterator and counter increments
      algoIndex++;
      algoCounter++;
    }
  // this is the trick - catch it before the framework terminates the job!  
  return StatusCode::SUCCESS;
}

//////////////
// Finalize //
//////////////

StatusCode
TBAlgoSequencer::finalize()
{
  ////////////////////////////////
  // Summary on Accepts/Rejects //
  ////////////////////////////////  

  double allReject = m_eventCounter > 0 
    ? ((double)m_rejectCounter)/((double)m_eventCounter)*100.
    : 100.;
  double noEvtReject = m_eventCounter > 0
    ? ((double)m_rejectNoEvent)/((double)m_eventCounter)*100.
    : 100;

  ATH_MSG_INFO
    ( "======================================================== " );
  ATH_MSG_INFO
    ( "Total events analyzed .................: "
      << std::setw(6)
      << m_eventCounter );
  ATH_MSG_INFO 
    ( "Total events rejected .................: "
      << std::setw(6)
      << m_rejectCounter
      << " ("
    //      << fixed()
      << allReject
      << " %)" );
  ATH_MSG_INFO
    ( "Events without EventHeader (rejected) .: "
      << std::setw(6)
      << m_rejectNoEvent
      << " ("
    //      << fixed()
      << noEvtReject
      << " %)" );
  ATH_MSG_INFO
    ( "-------------------------------------------------------- " );
  ATH_MSG_INFO
    ( "Reject patterns: " );
  ATH_MSG_INFO
    ( "-------------------------------------------------------- " );
  
  std::map<std::string,unsigned int>::iterator 
    firstReject = m_rejectPattern.begin();
  std::map<std::string,unsigned int>::iterator
    firstAccept = m_acceptPattern.begin();

  for ( ; firstReject != m_rejectPattern.end() ; firstReject++ )
    {
      double percentReject = m_eventCounter > 0
	? ((double)(*firstReject).second)/((double)m_eventCounter)*100.
	: 100;
      msg() << MSG::INFO
	  << "Algorithm ";
      msg().width(20);
      msg() << MSG::INFO
	  << (*firstReject).first
	  << " rejected "
	  << std::setw(6)
	  << (*firstReject).second
	  << " events (";
      msg() << MSG::INFO
	  << std::setprecision(5)
	  << percentReject
	  << " %)"
	  << endmsg;
    }
  ATH_MSG_INFO
    ( "-------------------------------------------------------- " );
  ATH_MSG_INFO
    ( "Accept patterns: " );
  ATH_MSG_INFO
    ( "-------------------------------------------------------- " );
  for ( ; firstAccept != m_acceptPattern.end() ; firstAccept++ )
    {
      double percentAccept = m_eventCounter > 0
	? ((double)(*firstAccept).second)/((double)m_eventCounter)*100.
	: 100;
      msg() << MSG::INFO
	  << "Algorithm ";
      msg().width(20);
      msg() << MSG::INFO
	  << (*firstAccept).first
	  << " accepted "
	  << std::setw(6)
	  << (*firstAccept).second
	  << " events (";
      msg().setf(std::ios::fixed);
      msg() << MSG::INFO
	  << std::setprecision(5)
	  << percentAccept
	  << " %)"
	  << endmsg;
    }
  ATH_MSG_INFO
    ( "======================================================== " );

  return StatusCode::SUCCESS;

}
