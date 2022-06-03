/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_DBTools/AFPDBTester.h"

AFPDBTester::AFPDBTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_key("EMFracClassify")
{
  ATH_MSG_DEBUG("in the constructor of AFPDBTester");
}

AFPDBTester::~AFPDBTester()
{
  ATH_MSG_DEBUG("in the destructor of AFPDBTester");
}

StatusCode AFPDBTester::initialize()
{
  ATH_MSG_DEBUG("in the initialize of AFPDBTester, hello there");

  ATH_CHECK( m_key.initialize() );
  ATH_MSG_INFO( "m_key id: " << m_key.fullKey() );
  ATH_CHECK( m_readKey.initialize() );
  ATH_MSG_INFO( "m_readKey id: " << m_readKey.fullKey() );
  ATH_CHECK( m_rch_locshiftX.initialize() ); 
  ATH_MSG_INFO( "m_rch_locshiftX id: " << m_rch_locshiftX.fullKey() );
  ATH_CHECK( m_rch_globshiftX.initialize() ); 
  ATH_MSG_INFO( "m_rch_globshiftX id: " << m_rch_globshiftX.fullKey() );
  ATH_CHECK( m_rch_loctof.initialize() );
  ATH_MSG_INFO( "m_rch_loctof id: " << m_rch_loctof.fullKey() );
  ATH_CHECK( m_rch_vtxtof.initialize() );
  ATH_MSG_INFO( "m_rch_vtxtof id: " << m_rch_vtxtof.fullKey() );
  
  return StatusCode::SUCCESS;
}

StatusCode AFPDBTester::finalize()
{
  ATH_MSG_DEBUG("in the finalize of AFPDBTester, bye bye");
  return StatusCode::SUCCESS;
}

StatusCode AFPDBTester::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  EventIDBase t( getContext().eventID() );

// check if calo conditions can be loaded
  ATH_MSG_INFO("execute: "<<getContext().eventID().event_number()<<" , before CaloLocalHadCoeff");
  SG::ReadCondHandle<CaloLocalHadCoeff> rch(m_key, ctx);
  const CaloLocalHadCoeff* condObject = *rch;
  if(condObject==0) {
    ATH_MSG_ERROR("Unable to access conditions object CaloLocalHadCoeff");
    return StatusCode::FAILURE;
  }

// check if beamspot conditions can be loaded
  ATH_MSG_INFO("execute: "<<getContext().eventID().event_number()<<" , before beam pos");
  SG::ReadCondHandle<AthenaAttributeList> readHandle{ m_readKey, ctx };  
  const AthenaAttributeList* raw { *readHandle };
  if ( raw == nullptr )
  {
    ATH_MSG_ERROR("Beam Spot data for key " << m_readKey.fullKey() << " not found");
    return StatusCode::FAILURE;
  }
  float beamposX { (*raw)["posX"].data<float>() };
  ATH_MSG_INFO("execute: "<<getContext().eventID().event_number()<<" , beam pos X = "<<beamposX);

// load local AFP alignment and print out something
  ATH_MSG_INFO("execute: "<<getContext().eventID().event_number()<<" , before loc shiftX");
  SG::ReadCondHandle<CondAttrListCollection> ch_locshiftX( m_rch_locshiftX, ctx );
  const CondAttrListCollection* attrLocList { *ch_locshiftX };
  if ( attrLocList == nullptr )
  {
    ATH_MSG_ERROR("local shiftX data for key " << m_rch_locshiftX.fullKey() << " not found");
    return StatusCode::FAILURE;
  }

  int counter=1;
  CondAttrListCollection::const_iterator itr;
  for (itr = attrLocList->begin(); itr != attrLocList->end(); ++itr) {
    if(counter>1) ATH_MSG_INFO("there should be only one real channel in /FWD/AFP/Align/Local, this is real channel nr. "<<counter);

    const coral::AttributeList &atr = itr->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    nlohmann::json jsondata = nlohmann::json::parse(data);
    int nchannels=jsondata["nchannels"];
    nlohmann::json channeldata=jsondata["data"];
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1,10,...,15,2,3,...,9
      ATH_MSG_INFO("reading channel nr. "<<chan.key());
      nlohmann::json aligndata=chan.value();
      
      std::string channel=chan.key();
      int layerID=aligndata["layerID"];
      float shiftX=aligndata["shiftX"];

      ATH_MSG_INFO("execute: run "<<getContext().eventID().run_number()<<", lb "<<getContext().eventID().lumi_block()<<", evnt "<<getContext().eventID().event_number()<<", channel "<<channel<<"/"<<nchannels<<", layerID "<<layerID<<", shiftX = "<<shiftX);
    }

    ++counter;
  }


// load global AFP alignment and print out something else
  ATH_MSG_INFO("test: "<<getContext().eventID().event_number()<<" , before glob shiftX");
  SG::ReadCondHandle<CondAttrListCollection> ch_globshiftX( m_rch_globshiftX, ctx );
  const CondAttrListCollection* attrGlobList { *ch_globshiftX };
  if ( attrGlobList == nullptr )
  {
    ATH_MSG_ERROR("global shiftX data for key " << m_rch_globshiftX.fullKey() << " not found");
    return StatusCode::FAILURE;
  }

  counter=1;
  CondAttrListCollection::const_iterator itr2;
  for (itr2 = attrGlobList->begin(); itr2 != attrGlobList->end(); ++itr2) {
    if(counter>1) ATH_MSG_INFO("there should be only one real channel in /FWD/AFP/Align/Global, this is real channel nr. "<<counter);

    const coral::AttributeList &atr = itr2->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    nlohmann::json jsondata = nlohmann::json::parse(data);
    int nchannels=jsondata["nchannels"];
    nlohmann::json channeldata=jsondata["data"];
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1,10,...,15,2,3,...,9
      ATH_MSG_INFO("reading channel nr. "<<chan.key());
      nlohmann::json aligndata=chan.value();
      
      std::string channel=chan.key();
      std::string alignType=aligndata["alignType"];
      float shiftX=aligndata["shiftX"];

      ATH_MSG_INFO("execute: run "<<getContext().eventID().run_number()<<", lb "<<getContext().eventID().lumi_block()<<", evnt "<<getContext().eventID().event_number()<<", channel "<<channel<<"/"<<nchannels<<", alignType "<<alignType<<", shiftX = "<<shiftX);
    }

    ++counter;
  }


// load local AFP ToF parameters and print out something
  ATH_MSG_INFO("execute: "<<getContext().eventID().event_number()<<" , before loc tof");
  SG::ReadCondHandle<CondAttrListCollection> ch_loctof( m_rch_loctof, ctx );
  const CondAttrListCollection* attrLocToFList { *ch_loctof };
  if ( attrLocToFList == nullptr )
  {
    ATH_MSG_ERROR("local ToF data for key " << m_rch_loctof.fullKey() << " not found");
    return StatusCode::FAILURE;
  }

  counter=1;
  CondAttrListCollection::const_iterator itr3;
  for (itr3 = attrLocToFList->begin(); itr3 != attrLocToFList->end(); ++itr3) {
    if(counter>1) ATH_MSG_INFO("there should be only one real channel in /FWD/AFP/ToFParameters/Local, this is real channel nr. "<<counter);

    const coral::AttributeList &atr = itr3->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    nlohmann::json jsondata = nlohmann::json::parse(data);
    int nchannels=jsondata["nchannels"];
    nlohmann::json channeldata=jsondata["data"];
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1,10,...,19,2,20,21,...29,3,30,31,4,...,9
      ATH_MSG_INFO("reading channel nr. "<<chan.key());
      nlohmann::json paramdata=chan.value();
      
      std::string channel=chan.key();
      int trainID=paramdata["trainID"];
      float barWeight=paramdata["barWeight"];

      ATH_MSG_INFO("execute: run "<<getContext().eventID().run_number()<<", lb "<<getContext().eventID().lumi_block()<<", evnt "<<getContext().eventID().event_number()<<", channel "<<channel<<"/"<<nchannels<<", trainID "<<trainID<<", barWeight = "<<barWeight);
    }

    ++counter;
  }

// load vertex AFP ToF parameters and print out something
  ATH_MSG_INFO("execute: "<<getContext().eventID().event_number()<<" , before vtx tof");
  SG::ReadCondHandle<CondAttrListCollection> ch_vtxtof( m_rch_vtxtof, ctx );
  const CondAttrListCollection* attrVtxToFList { *ch_vtxtof };
  if ( attrVtxToFList == nullptr )
  {
    ATH_MSG_ERROR("vertex ToF data for key " << m_rch_vtxtof.fullKey() << " not found");
    return StatusCode::FAILURE;
  }

  counter=1;
  CondAttrListCollection::const_iterator itr4;
  for (itr4 = attrVtxToFList->begin(); itr4 != attrVtxToFList->end(); ++itr4) {
    if(counter>1) ATH_MSG_INFO("there should be only one real channel in /FWD/AFP/ToFParameters/Vertex, this is real channel nr. "<<counter);

    const coral::AttributeList &atr = itr4->second;
    std::string data = *(static_cast<const std::string *>((atr["data"]).addressOfData()));

    nlohmann::json jsondata = nlohmann::json::parse(data);
    int nchannels=jsondata["nchannels"];
    nlohmann::json channeldata=jsondata["data"];
    for(auto& chan : channeldata.items())
    {
      // channels are ordered alphabetically: 0,1
      ATH_MSG_INFO("reading channel nr. "<<chan.key());
      nlohmann::json paramdata=chan.value();
      
      std::string channel=chan.key();
      int stationID=paramdata["stationID"];
      float timeGlobalOffset=paramdata["timeGlobalOffset"];
      std::vector<float> trainEdge=paramdata["trainEdge"];

      ATH_MSG_INFO("execute: run "<<getContext().eventID().run_number()<<", lb "<<getContext().eventID().lumi_block()<<", evnt "<<getContext().eventID().event_number()<<", channel "<<channel<<"/"<<nchannels<<", stationID "<<stationID<<", timeGlobalOffset = "<<timeGlobalOffset<<", trainEdge = ("<<trainEdge.at(0)<<", "<<trainEdge.at(1)<<", ... "<<trainEdge.at(4)<<")");
    }

    ++counter;
  }
  
  return StatusCode::SUCCESS;
}

