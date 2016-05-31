/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimTimer.h"
#include <iostream>
#include <iomanip>

//
// Current design of G4SimTimer
//

void G4SimTimer::BeginOfEvent(const G4Event*)
{
  nrOfEntries++;
  eventTimer->Start();
}
void G4SimTimer::EndOfEvent(const G4Event*)
{
  eventTimer->Stop();
  eventTime=eventTimer->GetUserElapsed()+eventTimer->GetSystemElapsed();
  if(nrOfEntries>1){ // discard first event
    accumulatedEventTime+=eventTime;
    accumulatedEventTimeSquared+=eventTime*eventTime;
  }
  ATH_MSG_INFO("\t Event nr. "<<nrOfEntries<<" took " << std::setprecision(4) << eventTime << " s. New average " <<
               std::setprecision(4) << averageTimePerEvent()<<" +- "<<std::setprecision(4) << Sigma());
}
void G4SimTimer::BeginOfRun(const G4Run*)
{
  runTimer->Start();
}
void G4SimTimer::EndOfRun(const G4Run*)
{
  runTimer->Stop();
  runTime=runTimer->GetUserElapsed()+runTimer->GetSystemElapsed();
  ATH_MSG_INFO("*****************************************");
  ATH_MSG_INFO("**                                     **");
  ATH_MSG_INFO("    End of run - time spent is "<<std::setprecision(4) << runTime);
  ATH_MSG_INFO("    Average time per event was "<<std::setprecision(4) << averageTimePerEvent()<<" +- "<< std::setprecision(4) << Sigma());
  ATH_MSG_INFO("**                                     **");
  ATH_MSG_INFO("*****************************************");
}

double G4SimTimer::averageTimePerEvent()
{
  if (nrOfEntries<2) return -1;
  return accumulatedEventTime/(nrOfEntries-1);
}

double G4SimTimer::Sigma()
{
  if (nrOfEntries<3) return -1;
  return sqrt((accumulatedEventTimeSquared/(nrOfEntries-1)-averageTimePerEvent()*averageTimePerEvent())/(nrOfEntries-2));
}

StatusCode G4SimTimer::initialize()
{
  return StatusCode::SUCCESS;
}


StatusCode G4SimTimer::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
// New design of G4SimTimer for multithreading
//=============================================================================

// Local includes
#include "G4UserActions/G4SimTimer.h"

// Infrastructure includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor for timing results struct
  //---------------------------------------------------------------------------
  G4SimTimer::Report::Report()
    : nEvent(0), eventTime(0), eventTimeSquared(0)
  {}

  //---------------------------------------------------------------------------
  // Calculate the mean and sample standard deviation
  //---------------------------------------------------------------------------
  std::pair<double, double> G4SimTimer::Report::meanAndSigma()
  {
    double mean = nEvent > 0 ? eventTime / nEvent : -1.;
    double sigma = -1.;
    if(nEvent > 1)
      sigma = sqrt( (eventTimeSquared/nEvent - mean*mean) / (nEvent-1) );
    return std::make_pair(mean, sigma);
  }

  //---------------------------------------------------------------------------
  // G4SimTimer constructor
  //---------------------------------------------------------------------------
  G4SimTimer::G4SimTimer()
    : m_msg("G4SimTimer"), m_firstEvent(true)
  {}

  //---------------------------------------------------------------------------
  // Begin-event action
  //---------------------------------------------------------------------------
  void G4SimTimer::beginOfEvent(const G4Event* /*event*/)
  {
    //ATH_MSG_INFO("beginOfEvent");
    m_eventTimer.Start();
  }

  //---------------------------------------------------------------------------
  // End-event action
  //---------------------------------------------------------------------------
  void G4SimTimer::endOfEvent(const G4Event* /*event*/)
  {
    m_eventTimer.Stop();
    // We define time as user+system time.
    auto eventTime = m_eventTimer.GetUserElapsed() + m_eventTimer.GetSystemElapsed();
    // Skip the first event
    if(!m_firstEvent) {
      m_results.nEvent++;
      m_results.eventTime += eventTime;
      m_results.eventTimeSquared += eventTime*eventTime;
      auto meanSigma = m_results.meanAndSigma();
      ATH_MSG_INFO("Event  " << m_results.nEvent << " took " <<
                   std::setprecision(4) << eventTime << " s. New average " <<
                   std::setprecision(4) << meanSigma.first << " +- " <<
                   std::setprecision(4) << meanSigma.second);
    }
    else m_firstEvent = false;
  }

} // namespace G4UA
