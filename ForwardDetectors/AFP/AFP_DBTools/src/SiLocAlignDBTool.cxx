/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_DBTools/SiLocAlignDBTool.h"

namespace AFP
{
  SiLocAlignDBTool::SiLocAlignDBTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent)
  {
  }
  

  StatusCode SiLocAlignDBTool::initialize()
  {
    ATH_CHECK( m_rch_loc.initialize() ); 
    ATH_MSG_DEBUG( "using DB with key " << m_rch_loc.fullKey() );
    return StatusCode::SUCCESS;
  }

  StatusCode SiLocAlignDBTool::finalize()
  {
    ATH_MSG_DEBUG("in the finalize of SiLocAlignDBTool, bye bye");
    return StatusCode::SUCCESS;
  }
  

  const SiLocAlignData SiLocAlignDBTool::alignment (const EventContext& ctx, const int stationID, const int layerID) const
  { 
    ATH_MSG_DEBUG("will get local alignment for run "<<ctx.eventID().run_number()<<", lb "<<ctx.eventID().lumi_block()<<", event "<<ctx.eventID().event_number()<<", station "<<stationID<<", layerID "<<layerID);

    SG::ReadCondHandle<CondAttrListCollection> ch_loc( m_rch_loc, ctx );
    const CondAttrListCollection* attrLocList { *ch_loc};
    if ( attrLocList == nullptr )
    {
        ATH_MSG_WARNING("local alignment data for key " << m_rch_loc.fullKey() << " not found, returning zeros");
        return SiLocAlignData(stationID,layerID);
    }
    
    if(attrLocList->size()>1) ATH_MSG_INFO("there should be only one real channel in "<< m_rch_loc.fullKey() <<", there are "<<attrLocList->size()<<" real channels, only the first one will be used ");
    
    CondAttrListCollection::const_iterator itr = attrLocList->begin();
    const coral::AttributeList &atr = itr->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    nlohmann::json jsondata = nlohmann::json::parse(data);
    nlohmann::json channeldata=jsondata["data"];
      
    // first, try to guess the channel nr.
    int guess_ch=stationID*4+layerID;
      
    nlohmann::json aligndata=channeldata.at(std::to_string(guess_ch)); // because using int would be too simple
    int st=aligndata["stationID"];
    int la=aligndata["layerID"];
    if(stationID==st && layerID==la)
    {
      ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", layerId "<<la<<", channel guess "<<guess_ch);
      return SiLocAlignData(st, la, aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
    }
    else
    {
      ATH_MSG_DEBUG("channel was not guessed correctly, stationID "<<st<<", layerId "<<la<<", channel guess "<<guess_ch);
    }
    
    // if guess is not correct, loop over all channels
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1,10,...,15,2,3,...,9
      nlohmann::json aligndata=chan.value();
        
      int st=aligndata["stationID"];
      int la=aligndata["layerID"];
        
      if(stationID==st && layerID==la)
      {
        ATH_MSG_DEBUG("channel found for stationID "<<st<<", layerId "<<la<<", channel nr. "<<chan.key());
        return SiLocAlignData(st, la, aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
      }
    }

    ATH_MSG_WARNING("local alignment data stationID "<<stationID<<", layerId "<<layerID<<" not found in any channels, returning zeros");
    return SiLocAlignData(stationID,layerID);
  }

} // AFP namespace
