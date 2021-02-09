/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigTimeAlgs/CookTimer.cxx
 * @brief  Some specialized classes of Athena::AlgorithmTimer
 * @author Frank Winklmeier
 */

#include "TrigTimeAlgs/CookTimer.h"

// Gaudi includes
#include "GaudiKernel/Incident.h"

// Athena includes
#include "AthenaKernel/ICoreDumpSvc.h"
#include "AthenaKernel/getMessageSvc.h"

// Root includes
#include "TSystem.h"

// STL includes
#include <iostream>
#include <sstream>

#include <boost/bind/bind.hpp>

using namespace HLT;


//--------------------------------------------------------------------------------
// AbortingCookTimer
//--------------------------------------------------------------------------------

AbortingCookTimer::AbortingCookTimer() :
  Athena::AlgorithmTimer(0, boost::bind(&AbortingCookTimer::abortJob, this))
{
}

AbortingCookTimer::AbortingCookTimer(unsigned int milliseconds) :
  Athena::AlgorithmTimer(milliseconds, boost::bind(&AbortingCookTimer::abortJob, this))
{
}


void AbortingCookTimer::abortJob()
{
  /*
   * Print some information, stack trace and abort the job.
   */
  std::ostringstream os;
  os << "Timeout (" << m_timeout << " sec) reached";

  ServiceHandle<ICoreDumpSvc> coreDumpSvc("CoreDumpSvc", "HLT::AbortingCookTimer");
  if ( coreDumpSvc.retrieve().isSuccess() ) {
    coreDumpSvc->setCoreDumpInfo("Reason", os.str());
    std::cout << coreDumpSvc->dump() << std::endl;
  }
  else std::cout << os.str() << std::endl;

  gSystem->StackTrace();
  abort();
}


//--------------------------------------------------------------------------------
// IncidentTimer
//--------------------------------------------------------------------------------

IncidentTimer::IncidentTimer(unsigned int milliseconds,
                             const std::string& incident) :
  Athena::AlgorithmTimer(milliseconds,
                         boost::bind(&IncidentTimer::fireIncident, this),
                         AlgorithmTimer::DELIVERYBYTHREAD),
  m_name(incident),
  m_incSvc("IncidentSvc", "IncidentTimer."+incident)
{
  if ( m_incSvc.retrieve().isFailure() ) {
    MsgStream log(Athena::getMessageSvc(), "IncidentTimer");
    log << MSG::FATAL << "Could not retrieve IncidentSvc" << endmsg;
  }
}


void IncidentTimer::fireIncident()
{
  m_incSvc->fireIncident(Incident(m_name,m_name));
}
