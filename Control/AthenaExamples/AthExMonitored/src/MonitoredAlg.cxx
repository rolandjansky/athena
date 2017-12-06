/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitoredAlg.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include <vector>
#include <cmath>

MonitoredAlg::MonitoredAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode MonitoredAlg::initialize()
{
  // Only try to retrieve the tool if one has been set
  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

/// Example class for monitoring
class Track {
public:
  Track(double eta, double phi, double pt) : m_eta(eta), m_phi(phi), m_pt(pt) {}
  double eta() const { return m_eta; }
  double phi() const { return m_phi; }
  double pt() const { return m_pt; }
private:
  double m_eta{0}, m_phi{0}, m_pt{0};
};


StatusCode MonitoredAlg::execute() 
{
  using namespace Monitored;   
  std::vector<Track> tracks;

  // In case you want to measure the execution time
  auto timer = MonitoredTimer::declare("TIME_execute");

  auto count = MonitoredScalar::declare<int>("nTracks", 0);  // explicit type

  // Access via member
  auto eta = MonitoredCollection::declare("eta", tracks, &Track::eta);

  // Access via function/lambda
  auto absphi = MonitoredCollection::declare("AbsPhi", tracks, []( const Track& t ) { return abs(t.phi()); });  

  auto mon = MonitoredScope::declare(m_monTool, count, eta, absphi, timer);

  count = 1 + (rand() % 10);   // random number of tracks

  // Creating some random tracks
  for (int i=0; i<count; ++i) {
    tracks.push_back(Track(-3.0 + 6.0*rand()/static_cast<float>(RAND_MAX),
                           -M_PI + 2*M_PI*rand()/static_cast<float>(RAND_MAX),
                           100.0*rand()/static_cast<float>(RAND_MAX)));                     
  }
  
  return StatusCode::SUCCESS;
}



