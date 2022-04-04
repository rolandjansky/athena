/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocRecoTool.h"

using namespace std;

AFP_SIDLocRecoTool::AFP_SIDLocRecoTool(const std::string &type,
                                       const std::string &name,
                                       const IInterface *parent)
  : base_class(type, name, parent)
{
  ATH_MSG_DEBUG("in AFP_SIDLocRecoTool constructor");
}


StatusCode AFP_SIDLocRecoTool::initialize()
{
  ATH_MSG_DEBUG("in AFP_SIDLocRecoTool::initialize()");

  // track reconstruction tools
  if(m_recoToolsList.empty())
  {
    ATH_MSG_ERROR("No track reconstruction tools set, check settings in AFP_LocReco/AFP_LocReco_joboption.py. Aborting.");
    return StatusCode::FAILURE;
  }
  else
  {
    CHECK( m_recoToolsList.retrieve() );
  }

  // output containers for the track reconstruction tools
  if(m_arrayOfWriteHandleKeys.empty())
  {
    ATH_MSG_ERROR("No output track containers provided, check settings in AFP_LocReco/AFP_LocReco_joboption.py. Aborting.");
    return StatusCode::FAILURE;
  }
  else
  {
    CHECK( m_arrayOfWriteHandleKeys.initialize() );
  }


  // the code in execute relies on the facts that (a) the keys in WriteHandleKeyArray are unique; (b) all output container names from track reco tools are also in WriteHandleKeyArray; and moreover (c) that there are no extra WriteHandleKeys that are not present in any track reco tools
  // case (c) wouldn't necessarily break things but it's not a sign of a good configuration and thus it's forbidden as well
  // first, get names from write handle keys
  std::vector<std::string> listOfWHKeys;
  for(const auto &whk : m_arrayOfWriteHandleKeys)
  {
    ATH_MSG_DEBUG("have WHKey "<<whk.key());
    listOfWHKeys.push_back(whk.key());
  }
  unsigned int all_WHkeys=listOfWHKeys.size();
  // remove duplicities, there shouldn't be any
  std::sort(listOfWHKeys.begin(), listOfWHKeys.end());
  listOfWHKeys.erase(std::unique(listOfWHKeys.begin(),listOfWHKeys.end()), listOfWHKeys.end());
  if(listOfWHKeys.size() != all_WHkeys)
  {
    ATH_MSG_ERROR("It seems write handle keys do not have unique values, check settings in AFP_LocReco/AFP_LocReco_joboption.py. Aborting.");
    return StatusCode::FAILURE;
  }
  // get names from track reco tools
  std::vector<std::string> listOfOutputContainers;
  for(const auto &recoTool : m_recoToolsList)
  {
    ATH_MSG_DEBUG("have ouput container "<<recoTool->outputContainerName());
    listOfOutputContainers.push_back(recoTool->outputContainerName());
  }
  // remove duplicities, they are allowed
  std::sort(listOfOutputContainers.begin(), listOfOutputContainers.end());
  listOfOutputContainers.erase(std::unique(listOfOutputContainers.begin(),listOfOutputContainers.end()), listOfOutputContainers.end());
  // write handle keys and names from track reco tools should have the same size
  if(listOfWHKeys.size() != listOfOutputContainers.size())
  {
    ATH_MSG_ERROR("There is different number of unique write handle keys ("<<listOfWHKeys.size()<<") and unique output containers ("<<listOfOutputContainers.size()<<"). Aborting");
    return StatusCode::FAILURE;
  }
  // check they are the same; one of these loops are not strictly neccessary, but they will print outstanding names
  bool doAbort=false;
  for(const auto &trkOutCont : listOfOutputContainers)
  {
    if(std::find(listOfWHKeys.begin(),listOfWHKeys.end(),trkOutCont) == listOfWHKeys.end())
    {
      ATH_MSG_ERROR("Cannot find track reconstruction output container "<<trkOutCont<<" in write handle keys. Aborting");
      doAbort=true;
    }
  }
  for(const auto &whk : listOfWHKeys)
  {
    if(std::find(listOfOutputContainers.begin(),listOfOutputContainers.end(),whk) == listOfOutputContainers.end())
    {
      ATH_MSG_ERROR("Cannot find write handle key "<<whk<<" in track reconstruction output containers. Aborting");
      doAbort=true;
    }
  }
  if(doAbort) return StatusCode::FAILURE;
  // Now, we are sure it's setup correctly


  // monitoring
  if (!(m_monTool.name().empty())) {
    CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }
  
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocRecoTool::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("in AFP_SIDLocRecoTool::execute()");


  auto trkSize = Monitored::Scalar("TrkSize", 0.0);

  for(const auto &whk : m_arrayOfWriteHandleKeys)
  {
    // reconstruct tracks
    auto afpTrk=std::make_unique<xAOD::AFPTrackContainer>();
    auto afpTrkAux=std::make_unique<xAOD::AFPTrackAuxContainer>();
    afpTrk->setStore(afpTrkAux.get());

    for(const auto &recoTool : m_recoToolsList)
    {
      if(recoTool->outputContainerName() != whk.key()) continue;

      if( recoTool->reconstructTracks(afpTrk, ctx).isFailure() )
      {
        ATH_MSG_WARNING ("Failed to reconstruct tracks with algorithm = "<<recoTool->name());
      }
    }
        
    trkSize += afpTrk->size();

    SG::WriteHandle<xAOD::AFPTrackContainer> trackContainer(whk, ctx);
    ATH_CHECK( trackContainer.record(std::move(afpTrk), std::move(afpTrkAux)) );
  }
  
  Monitored::Group( m_monTool, trkSize);

  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocRecoTool::finalize()
{
  return StatusCode::SUCCESS;
}

