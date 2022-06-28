/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_DBTools/ToFVtxParamDBTool.h"

namespace AFP
{
  ToFVtxParamDBTool::ToFVtxParamDBTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent)
  {
  }
  

  StatusCode ToFVtxParamDBTool::initialize()
  {
    ATH_CHECK( m_rch_vtx.initialize() ); 
    ATH_MSG_DEBUG( "using DB with key " << m_rch_vtx.fullKey() );
    return StatusCode::SUCCESS;
  }

  StatusCode ToFVtxParamDBTool::finalize()
  {
    ATH_MSG_DEBUG("in the finalize of ToFVtxParamDBTool, bye bye");
    return StatusCode::SUCCESS;
  }
  

  nlohmann::json ToFVtxParamDBTool::parametersData(const EventContext& ctx) const
  { 
    ATH_MSG_DEBUG("will get vertex ToF parameters for run "<<ctx.eventID().run_number()<<", lb "<<ctx.eventID().lumi_block()<<", event "<<ctx.eventID().event_number());

    SG::ReadCondHandle<CondAttrListCollection> ch_loc( m_rch_vtx, ctx );
    const CondAttrListCollection* attrLocList { *ch_loc};
    if ( attrLocList == nullptr )
    {
        ATH_MSG_WARNING("vertex ToF parameters data for key " << m_rch_vtx.fullKey() << " not found, returning empty string");
        return nlohmann::json::parse("");
    }
    
    if(attrLocList->size()>1) ATH_MSG_INFO("there should be only one real channel in "<< m_rch_vtx.fullKey() <<", there are "<<attrLocList->size()<<" real channels, only the first one will be used ");
    
    CondAttrListCollection::const_iterator itr = attrLocList->begin();
    const coral::AttributeList &atr = itr->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    return nlohmann::json::parse(data);
  }
  
  
  const ToFVtxParamData ToFVtxParamDBTool::parameters(const nlohmann::json& jsondata, const int stationID) const
  {
    ATH_MSG_DEBUG("will get vertex ToF parameters for station "<<stationID);
    nlohmann::json channeldata=jsondata["data"];
    
    // first, try to guess the channel nr.
    int guess_ch=(stationID/3);
      
    nlohmann::json paramdata=channeldata.at(std::to_string(guess_ch)); // because using int would be too simple
    int st=paramdata["stationID"];
    if(stationID==st)
    {
      ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", channel guess "<<guess_ch);
      return ToFVtxParamData(st, paramdata["timeGlobalOffset"], paramdata["timeOffset"], paramdata["timeSlope"], paramdata["trainEdge"]);
    }
    else
    {
      ATH_MSG_DEBUG("channel was not guessed correctly, stationID "<<st<<", channel guess "<<guess_ch);
    }
    
    // if guess is not correct, loop over all channels
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1
      nlohmann::json paramdata=chan.value();
        
      int st=paramdata["stationID"];
      if(stationID==st)
      {
        ATH_MSG_DEBUG("channel found for stationID "<<st<<", channel nr. "<<chan.key());
        return ToFVtxParamData(st, paramdata["timeGlobalOffset"], paramdata["timeOffset"], paramdata["timeSlope"], paramdata["trainEdge"]);
      }
    }

    ATH_MSG_WARNING("vertex ToF parameters data stationID "<<stationID<<" not found in any channels, returning zeros");
    return ToFVtxParamData(stationID);
  }

} // AFP namespace
