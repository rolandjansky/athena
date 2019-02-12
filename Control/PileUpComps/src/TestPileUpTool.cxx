/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "TestPileUpTool.h"

TestPileUpTool::TestPileUpTool(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : PileUpToolBase(type, name, parent)
  , m_doFiltering(false)
  , m_maxInTimePileUp(4)
{
  declareProperty("DoFiltering", m_doFiltering, "");
  declareProperty("MaxInTimePileUp", m_maxInTimePileUp, "");
}

StatusCode TestPileUpTool::prepareEvent(unsigned int nInputEvents)
{
  ATH_MSG_INFO( "prepareEvent: expect to process " << nInputEvents << " events this time." );
  return StatusCode::SUCCESS;
}

StatusCode TestPileUpTool::mergeEvent()
{
  for (const auto& bc : m_seen)
    {
      ATH_MSG_INFO( "mergeEvent: there are " << bc.first << " events in bunch xing " << bc.second );
    }
  return StatusCode::SUCCESS;
}

StatusCode TestPileUpTool::processBunchXing(int bunchXing,
                                            SubEventIterator bSubEvents,
                                            SubEventIterator eSubEvents)
{
  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  if(m_doFiltering && bunchXing==0)
    {
      if(m_maxInTimePileUp < m_seen.back().first)
        {
          ATH_MSG_INFO("Triggering filter as there were " << m_seen.back().first << " pile-up events in time.");
          m_filterPassed = false;
        }
    }

  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents)
    {
      StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
      ATH_MSG_DEBUG("SubEvt EventInfo from StoreGate " << seStore.name() << "  : "
                   << " bunch crossing : " << bunchXing
                   << " time offset: " << iEvt->time()
                   << " event: " << iEvt->ptr()->eventNumber()
                   << " run: " << iEvt->ptr()->runNumber()
                   );
      ++iEvt;
    }
  return StatusCode::SUCCESS;
}
