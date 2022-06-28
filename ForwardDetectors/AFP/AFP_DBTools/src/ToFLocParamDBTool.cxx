/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_DBTools/ToFLocParamDBTool.h"

namespace AFP
{
  ToFLocParamDBTool::ToFLocParamDBTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent)
  {
  }
  

  StatusCode ToFLocParamDBTool::initialize()
  {
    ATH_CHECK( m_rch_loc.initialize() ); 
    ATH_MSG_DEBUG( "using DB with key " << m_rch_loc.fullKey() );
    return StatusCode::SUCCESS;
  }

  StatusCode ToFLocParamDBTool::finalize()
  {
    ATH_MSG_DEBUG("in the finalize of ToFLocParamDBTool, bye bye");
    return StatusCode::SUCCESS;
  }
  

  nlohmann::json ToFLocParamDBTool::parametersData(const EventContext& ctx) const
  { 
    ATH_MSG_DEBUG("will get local ToF parameters for run "<<ctx.eventID().run_number()<<", lb "<<ctx.eventID().lumi_block()<<", event "<<ctx.eventID().event_number());

    SG::ReadCondHandle<CondAttrListCollection> ch_loc( m_rch_loc, ctx );
    const CondAttrListCollection* attrLocList { *ch_loc};
    if ( attrLocList == nullptr )
    {
        ATH_MSG_WARNING("local ToF parameters data for key " << m_rch_loc.fullKey() << " not found, returning empty string");
        return nlohmann::json::parse("");
    }
    
    if(attrLocList->size()>1) ATH_MSG_INFO("there should be only one real channel in "<< m_rch_loc.fullKey() <<", there are "<<attrLocList->size()<<" real channels, only the first one will be used ");
    
    CondAttrListCollection::const_iterator itr = attrLocList->begin();
    const coral::AttributeList &atr = itr->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    return nlohmann::json::parse(data);
  }
  
  
  const ToFLocParamData ToFLocParamDBTool::parameters(const nlohmann::json& jsondata, const int stationID, const int trainID, const int barID) const
  {
    ATH_MSG_DEBUG("will get local ToF parameters for station "<<stationID<<", trainID "<<trainID<<", barID "<<barID);
    nlohmann::json channeldata=jsondata["data"];
    
    // first, try to guess the channel nr.
    int guess_ch=(stationID/3)*16+trainID*4+barID;
      
    nlohmann::json paramdata=channeldata.at(std::to_string(guess_ch)); // because using int would be too simple
    int st=paramdata["stationID"];
    int tr=paramdata["trainID"];
    int ba=paramdata["barID"];
    if(stationID==st && trainID==tr && barID==ba)
    {
      ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", trainID "<<trainID<<", barID "<<barID<<", channel guess "<<guess_ch);
      return ToFLocParamData(st, tr, ba, paramdata["barWeight"], paramdata["barTimeOffset"]);
    }
    else
    {
      ATH_MSG_DEBUG("channel was not guessed correctly, stationID "<<st<<", trainID "<<trainID<<", barID "<<barID<<", channel guess "<<guess_ch);
    }
    
    // if guess is not correct, loop over all channels
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1,10,...,19,2,20,21,...29,3,30,31,4,...,9
      nlohmann::json paramdata=chan.value();
        
      int st=paramdata["stationID"];
      int tr=paramdata["trainID"];
      int ba=paramdata["barID"];
        
      if(stationID==st && trainID==tr && barID==ba)
      {
        ATH_MSG_DEBUG("channel found for stationID "<<st<<", trainID "<<trainID<<", barID "<<barID<<", channel nr. "<<chan.key());
        return ToFLocParamData(st, tr, ba, paramdata["barWeight"], paramdata["barTimeOffset"]);
      }
    }

    ATH_MSG_WARNING("local ToF parameters data stationID "<<stationID<<", trainID "<<trainID<<", barID "<<barID<<" not found in any channels, returning zeros");
    return ToFLocParamData(stationID, trainID, barID);
  }

} // AFP namespace
