#include <AsgTools/MessageCheck.h>
#include "ArtTest.h"

#include "AthenaMonitoring/MonitoredScope.h"

#include <xAODEventInfo/EventInfo.h>

#include <vector>
#include <cmath>

using namespace std;

ArtTest :: ArtTest (const std::string& name, ISvcLocator *pSvcLocator) : AthAlgorithm (name, pSvcLocator)
{
}

// ******

StatusCode ArtTest :: initialize ()
{
  ANA_MSG_INFO ("******************************* Initializing *******************************");

  // Only try to retrieve the tool if one has been set
  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: beginInputFile ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: firstExecute ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: execute ()
{
  using namespace Monitored;

  // Retrieve eventInfo from the event store
  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK (evtStore()->retrieve (eventInfo, "EventInfo"));

  /*
  if((eventInfo->eventNumber())%500 == 0) {
    ANA_MSG_INFO ("******************************** In Execute ********************************");
    ANA_MSG_INFO ("in execute, runNumber = " << eventInfo->runNumber() << ", eventNumber = " << eventInfo->eventNumber());
    ANA_MSG_INFO ("****************************************************************************");
  }
  */
  
  auto runNumb = MonitoredScalar::declare<int>("runNmb", 0);
  auto evtNumb = MonitoredScalar::declare<int>("evtNmb", 0);
  auto timer   = MonitoredTimer::declare("TIME_execute");

  auto mon     = MonitoredScope::declare(m_monTool, runNumb, evtNumb, timer);
  
  runNumb = eventInfo->runNumber();
  evtNumb = eventInfo->eventNumber();
  
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: finalize ()
{
  ANA_MSG_INFO ("******************************** Finalizing ********************************");
  
  return StatusCode::SUCCESS;
}
