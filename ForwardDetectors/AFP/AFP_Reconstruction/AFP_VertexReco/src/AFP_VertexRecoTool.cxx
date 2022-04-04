/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_VertexReco/AFP_VertexRecoTool.h"

using namespace std;

AFP_VertexRecoTool::AFP_VertexRecoTool(const std::string &type,
                                       const std::string &name,
                                       const IInterface *parent)
  : base_class(type, name, parent)
{
  ATH_MSG_DEBUG("in AFP_VertexRecoTool constructor");
}


StatusCode AFP_VertexRecoTool::initialize()
{
  ATH_MSG_DEBUG("in AFP_VertexRecoTool::initialize()");

  // vertex reconstruction tools
  if(m_recoToolsList.empty())
  {
    ATH_MSG_ERROR("No vertex reconstruction tools set, check settings in AFP_VertexReco/AFP_VertexReco_joboption.py. Aborting.");
    return StatusCode::FAILURE;
  }
  else
  {
    ATH_MSG_DEBUG("There are these tools in m_recoToolsList: "<<m_recoToolsList<<", will retrieve");
    CHECK( m_recoToolsList.retrieve() );
  }

  // output containers for the vertex reconstruction tools
  if(m_arrayOfWriteHandleKeys.empty())
  {
    ATH_MSG_ERROR("No output vertex containers provided, check settings in AFP_VertexReco/AFP_VertexReco_joboption.py. Aborting.");
    return StatusCode::FAILURE;
  }
  else
  {
    ATH_MSG_DEBUG("There are these write handle keys in m_arrayOfWriteHandleKeys: "<<m_arrayOfWriteHandleKeys<<", will initialize");
    CHECK( m_arrayOfWriteHandleKeys.initialize() );
  }


  // the code in execute relies on the facts that (a) the keys in WriteHandleKeyArray are unique; (b) all output container names from vertex reco tools are also in WriteHandleKeyArray; and moreover (c) that there are no extra WriteHandleKeys that are not present in any vertex reco tools
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
    ATH_MSG_ERROR("It seems write handle keys do not have unique values, check settings in AFP_VertexReco/AFP_VertexReco_joboption.py. Aborting.");
    return StatusCode::FAILURE;
  }
  // get names from proton reco tools
  std::vector<std::string> listOfOutputContainers;
  for(const auto &recoTool : m_recoToolsList)
  {
    ATH_MSG_DEBUG("have ouput container "<<recoTool->outputContainerName());
    listOfOutputContainers.push_back(recoTool->outputContainerName());
  }
  // remove duplicities, they are allowed
  std::sort(listOfOutputContainers.begin(), listOfOutputContainers.end());
  listOfOutputContainers.erase(std::unique(listOfOutputContainers.begin(),listOfOutputContainers.end()), listOfOutputContainers.end());
  // write handle keys and names from proton reco tools should have the same size
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
      ATH_MSG_ERROR("Cannot find proton reconstruction output container "<<trkOutCont<<" in write handle keys. Aborting");
      doAbort=true;
    }
  }
  for(const auto &whk : listOfWHKeys)
  {
    if(std::find(listOfOutputContainers.begin(),listOfOutputContainers.end(),whk) == listOfOutputContainers.end())
    {
      ATH_MSG_ERROR("Cannot find write handle key "<<whk<<" in vertex reconstruction output containers. Aborting");
      doAbort=true;
    }
  }
  if(doAbort) return StatusCode::FAILURE;
  // Now, we are sure it's setup correctly


// monitoring tool to be added soon
  // monitoring
//   if (!(m_monTool.name().empty())) {
//     CHECK( m_monTool.retrieve() );
//     ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
//   }
  
  return StatusCode::SUCCESS;
}

StatusCode AFP_VertexRecoTool::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("in AFP_VertexRecoTool::execute()");


  for(const auto &whk : m_arrayOfWriteHandleKeys)
  {
    // reconstruct protons
    auto afpVertex=std::make_unique<xAOD::AFPVertexContainer>();
    auto afpVertexAux=std::make_unique<xAOD::AFPVertexAuxContainer>();
    afpVertex->setStore(afpVertexAux.get());

    for(const auto &recoTool : m_recoToolsList)
    {
      if(recoTool->outputContainerName() != whk.key()) continue;

      if( recoTool->doVertexReco(afpVertex, ctx).isFailure() )
      {
        ATH_MSG_WARNING ("Failed to reconstruct protons with algorithm = "<<recoTool->name());
      }
    }
    
    ATH_MSG_DEBUG("write handle key "<<whk<<", have "<<afpVertex->size()<<" reconstructed protons");
        

    SG::WriteHandle<xAOD::AFPVertexContainer> vertexContainer(whk, ctx);
    ATH_CHECK( vertexContainer.record(std::move(afpVertex), std::move(afpVertexAux)) );
  }
  
  return StatusCode::SUCCESS;
}

