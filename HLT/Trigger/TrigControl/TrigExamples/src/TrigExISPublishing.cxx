/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "hltinterface/ContainerFactory.h"
#include "hltinterface/IInfoRegister.h"

#include "TrigExISPublishing.h"

#include <vector>

TrigExISPublishing::TrigExISPublishing(const std::string& name, ISvcLocator* svcLoc) :
  AthReentrantAlgorithm(name, svcLoc)
{}

StatusCode TrigExISPublishing::initialize()
{
  // construct the LAr noise burst container and register it
  auto cfact = hltinterface::ContainerFactory::getInstance();
  if (cfact) {
    try {
      const std::string ISname = "LArISInfo_NoiseBurstAlg";
      const std::string IStype = "LArNoiseBurstCandidates";
      m_IsObject = cfact->constructContainer(ISname, IStype);
      m_evntPos = cfact->addIntVector(m_IsObject, "Flag",
                                      hltinterface::GenericHLTContainer::LASTVALUE);
      m_timeTagPos = cfact->addIntVector(m_IsObject, "TimeStamp",
                                         hltinterface::GenericHLTContainer::LASTVALUE);
      ATH_MSG_DEBUG("Registering container in IS with name /HLTObjects/" << ISname);
      hltinterface::IInfoRegister::instance()->registerObject("/HLTObjects/", m_IsObject);
    }
    catch (std::exception& ex) {
      ATH_MSG_ERROR("Cannot publish to IS: " << ex.what());
    }
  }
  else {
    ATH_MSG_INFO("IS publishing not available");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigExISPublishing::execute(const EventContext& ctx) const
{
  if (m_IsObject) {
    boost::property_tree::ptree event_tree;
    event_tree.put("eventNumber", ctx.eventID().event_number());
    event_tree.put("LBNumber", ctx.eventID().lumi_block());
    try {
      hltinterface::IInfoRegister::instance()->beginEvent(event_tree);

      m_IsObject->appendField(m_evntPos, std::vector<long>{0});
      m_IsObject->appendField(m_timeTagPos, std::vector<long>{(long int)ctx.eventID().time_stamp()});

      hltinterface::IInfoRegister::instance()->endEvent(event_tree);
    }
    catch (const std::exception& ex) {
      ATH_MSG_INFO("Caught exception during IS publication: " << ex.what());
    }
  }

  return StatusCode::SUCCESS;
}
