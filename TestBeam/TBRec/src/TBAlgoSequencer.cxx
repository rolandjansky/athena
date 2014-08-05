/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/Algorithm.h"

#include "TBRec/TBAlgoSequencer.h"

#include <string>
#include <map>
#include <iostream>

/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBAlgoSequencer::TBAlgoSequencer(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator)
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
  m_storeGate = 0;
}

TBAlgoSequencer::~TBAlgoSequencer()
{ }

////////////////////
// Initialization //
////////////////////

StatusCode
TBAlgoSequencer::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

  // message service
  MsgStream log(messageService(),name());
  log << MSG::INFO
      << "initialize..."
      << endreq;

  // StoreGate
  StatusCode checkOut =service("StoreGateSvc",m_storeGate);
  if ( checkOut.isFailure() )
    {
      log << MSG::ERROR
	  << "unable to retrieve pointer to StoreGate service."
	  << endreq;
      return checkOut;
    }

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
	  log << MSG::INFO
	      << "Subalgorithm ("
	      << std::setw(2)
	      << acceptedAlgos
	      << ") ... created type/name ... "
	      << theAlgItem.type()
	      << "/"
	      << theAlgItem.name()
	      << endreq;
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

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////

  MsgStream log(messageService(),name());
  IChronoStatSvc* theTicker = chronoSvc();

  m_timingOn = theTicker != 0 && m_timingOn;

  //////////////////////
  // Event Statistics //
  //////////////////////

  m_eventCounter++;

  if ( m_eventCounter == 1 || m_eventCounter % m_eventPrintFreq == 0 )
    {
      log << MSG::INFO
	  << "Number of events processed: "
	  << std::setw(9)
	  << m_eventCounter
	  << endreq;
      //	  << ", this event: Run "
      //	  << setw(6)
      //	  << theEvent->getRunNumber()
      //	  << " Event "
      //	  << setw(6)
      //	  << theEvent->getEventNumber()
      //	  << endreq;
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
  return SUCCESS;
}

//////////////
// Finalize //
//////////////

StatusCode
TBAlgoSequencer::finalize()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////

  MsgStream log(messageService(),name());

  ////////////////////////////////
  // Summary on Accepts/Rejects //
  ////////////////////////////////  

  double allReject = m_eventCounter > 0 
    ? ((double)m_rejectCounter)/((double)m_eventCounter)*100.
    : 100.;
  double noEvtReject = m_eventCounter > 0
    ? ((double)m_rejectNoEvent)/((double)m_eventCounter)*100.
    : 100;

  log << MSG::INFO
      << "======================================================== " << endreq;
  log << MSG::INFO
      << "Total events analyzed .................: "
      << std::setw(6)
      << m_eventCounter
      << endreq;
  log << MSG::INFO 
      << "Total events rejected .................: "
      << std::setw(6)
      << m_rejectCounter
      << " ("
    //      << fixed()
      << allReject
      << " %)"
      << endreq;
  log << MSG::INFO
      << "Events without EventHeader (rejected) .: "
      << std::setw(6)
      << m_rejectNoEvent
      << " ("
    //      << fixed()
      << noEvtReject
      << " %)"
      << endreq;
  log << MSG::INFO
      << "-------------------------------------------------------- " << endreq;
  log << MSG::INFO
      << "Reject patterns: " << endreq;
  log << MSG::INFO
      << "-------------------------------------------------------- " << endreq;
  
  std::map<std::string,unsigned int>::iterator 
    firstReject = m_rejectPattern.begin();
  std::map<std::string,unsigned int>::iterator
    firstAccept = m_acceptPattern.begin();

  for ( ; firstReject != m_rejectPattern.end() ; firstReject++ )
    {
      double percentReject = m_eventCounter > 0
	? ((double)(*firstReject).second)/((double)m_eventCounter)*100.
	: 100;
      log << MSG::INFO
	  << "Algorithm ";
      log.width(20);
      log << MSG::INFO
	  << (*firstReject).first
	  << " rejected "
	  << std::setw(6)
	  << (*firstReject).second
	  << " events (";
      log << MSG::INFO
	  << std::setprecision(5)
	  << percentReject
	  << " %)"
	  << endreq;
    }
  log << MSG::INFO
      << "-------------------------------------------------------- " << endreq;
  log << MSG::INFO
      << "Accept patterns: " << endreq;
  log << MSG::INFO
      << "-------------------------------------------------------- " << endreq;
  for ( ; firstAccept != m_acceptPattern.end() ; firstAccept++ )
    {
      double percentAccept = m_eventCounter > 0
	? ((double)(*firstAccept).second)/((double)m_eventCounter)*100.
	: 100;
      log << MSG::INFO
	  << "Algorithm ";
      log.width(20);
      log << MSG::INFO
	  << (*firstAccept).first
	  << " accepted "
	  << std::setw(6)
	  << (*firstAccept).second
	  << " events (";
      log.setf(std::ios::fixed);
      log << MSG::INFO
	  << std::setprecision(5)
	  << percentAccept
	  << " %)"
	  << endreq;
    }
  log << MSG::INFO
      << "======================================================== " << endreq;

  return StatusCode::SUCCESS;

}
