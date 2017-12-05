/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
  void G4SimTimer::BeginOfEventAction(const G4Event* /*event*/)
  {
    m_eventTimer.Start();
  }

  //---------------------------------------------------------------------------
  // End-event action
  //---------------------------------------------------------------------------
  void G4SimTimer::EndOfEventAction(const G4Event* /*event*/)
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
