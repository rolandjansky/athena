/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiGlobAlignDBTool.cxx
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-02-23
 *
 * @brief  File with implementation of SiGlobAlignDBTool
 */

#include "AFP_DBTools/SiGlobAlignDBTool.h"


namespace AFP
{
  SiGlobAlignDBTool::SiGlobAlignDBTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent)
  {
  }

  StatusCode SiGlobAlignDBTool::initialize()
  {
    ATH_CHECK( m_rch_glob.initialize() ); 
    ATH_MSG_DEBUG( "using DB with key " << m_rch_glob.fullKey() );
    return StatusCode::SUCCESS;
  }
  
  StatusCode SiGlobAlignDBTool::finalize()
  {
    ATH_MSG_DEBUG("in the finalize of SiGlobAlignDBTool, bye bye");
    return StatusCode::SUCCESS;
  }
  
  
  const SiGlobAlignData SiGlobAlignDBTool::alignment (const EventContext& ctx, const int stationID) const
  {
    ATH_MSG_DEBUG("will get global alignment for run "<<ctx.eventID().run_number()<<", lb "<<ctx.eventID().lumi_block()<<", event "<<ctx.eventID().event_number()<<", station "<<stationID);

    SG::ReadCondHandle<CondAttrListCollection> ch_glob( m_rch_glob, ctx );
    const CondAttrListCollection* attrGlobList { *ch_glob};
    if ( attrGlobList == nullptr )
    {
        ATH_MSG_WARNING("global alignment data for key " << m_rch_glob.fullKey() << " not found, returning zeros");
        return SiGlobAlignData(stationID);
    }
    
    if(attrGlobList->size()>1) ATH_MSG_INFO("there should be only one real channel in "<< m_rch_glob.fullKey() <<", there are "<<attrGlobList->size()<<" real channels, only the first one will be used ");
    
    CondAttrListCollection::const_iterator itr = attrGlobList->begin();  
    const coral::AttributeList &atr = itr->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    nlohmann::json jsondata = nlohmann::json::parse(data);
    nlohmann::json channeldata=jsondata["data"];

    // first, try to guess the channel nr.
    SiGlobAlignData GA_guess(stationID);
    std::vector<int> guess_ch_vec{stationID*4, stationID*4+1, stationID*4+2, stationID*4+3};
    int guess_ch_correct=0;
    for(auto guess_ch : guess_ch_vec)
    {
      nlohmann::json aligndata=channeldata.at(std::to_string(guess_ch)); // because using int would be too simple
      int st=aligndata["stationID"];
      std::string alignType=aligndata["alignType"];
      if(stationID==st)
      {   
        if(alignType=="tracker" && !(guess_ch_correct&1))
        {
          ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", alignType "<<alignType<<", channel guess "<<guess_ch);
          GA_guess.setTracker(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          guess_ch_correct+=1;
        }
        else if(alignType=="beam" && !(guess_ch_correct&2))
        {
          ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", alignType "<<alignType<<", channel guess "<<guess_ch);
          GA_guess.setBeam(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          guess_ch_correct+=2;
        }
        else if(alignType=="RP" && !(guess_ch_correct&4))
        {
          ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", alignType "<<alignType<<", channel guess "<<guess_ch);
          GA_guess.setRP(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          guess_ch_correct+=4;
        }
        else if(alignType=="correction" && !(guess_ch_correct&8))
        {
          ATH_MSG_DEBUG("channel guessed correctly, stationID "<<st<<", alignType "<<alignType<<", channel guess "<<guess_ch);
          GA_guess.setCorr(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          guess_ch_correct+=8;
        }
        else ATH_MSG_DEBUG("alignType or channel is probably incorrect, stationID "<<stationID<<", alignType "<<alignType<<", channel guess "<<guess_ch<<", guess_ch_correct "<<guess_ch_correct);
      }
    }
      
    if(guess_ch_correct==15)
    {
      ATH_MSG_DEBUG("channels guessed correctly, stationID "<<stationID);
      return GA_guess;
    }
    else
    {
      ATH_MSG_DEBUG("channels were not guessed correctly, stationID "<<stationID);
    }
      
    // if guess is not correct, loop over all channels
    SiGlobAlignData GA_loop(stationID);
    int loop_ch_correct=0;
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1,10,...,15,2,3,...,9
      nlohmann::json aligndata=chan.value();
      
      int st=aligndata["stationID"];
      std::string alignType=aligndata["alignType"];

      if(stationID==st)
      {  
        if(alignType=="tracker" && !(loop_ch_correct&1))
        {
          ATH_MSG_DEBUG("channel found for stationID "<<st<<", alignType "<<alignType<<", channel nr. "<<chan.key());
          GA_loop.setTracker(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          loop_ch_correct+=1;
        }
        else if(alignType=="beam" && !(loop_ch_correct&2))
        {
          ATH_MSG_DEBUG("channel found for stationID "<<st<<", alignType "<<alignType<<", channel nr. "<<chan.key());
          GA_loop.setBeam(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          loop_ch_correct+=2;
        }
        else if(alignType=="RP" && !(loop_ch_correct&4))
        {
          ATH_MSG_DEBUG("channel found for stationID "<<st<<", alignType "<<alignType<<", channel nr. "<<chan.key());
          GA_loop.setRP(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          loop_ch_correct+=4;
        }
        else if(alignType=="correction" && !(loop_ch_correct&8))
        {
          ATH_MSG_DEBUG("channel found for stationID "<<st<<", alignType "<<alignType<<", channel nr. "<<chan.key());
          GA_loop.setCorr(aligndata["shiftX"], aligndata["shiftY"], aligndata["shiftZ"], aligndata["alpha"], aligndata["beta"], aligndata["gamma"]);
          loop_ch_correct+=8;
        }
        else ATH_MSG_DEBUG("alignType is probably incorrect, stationID "<<stationID<<", alignType "<<alignType<<", channel nr. "<<chan.key()<<", loop_ch_correct "<<loop_ch_correct);
      }

      if(loop_ch_correct==15)
      {
        ATH_MSG_DEBUG("channels found correctly, stationID "<<stationID);
        return GA_loop;
      }
      else
      {
        ATH_MSG_DEBUG("channels were not found correctly, stationID "<<stationID);
      }
    }

    ATH_MSG_WARNING("global alignment data stationID "<<stationID<<" not found in any channels, returning zeros");
    return SiGlobAlignData(stationID);
  }
  
  

} // AFP namespace

