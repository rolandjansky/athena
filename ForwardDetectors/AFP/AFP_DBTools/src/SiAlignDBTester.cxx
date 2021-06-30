/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_DBTools/SiAlignDBTester.h"

SiAlignDBTester::SiAlignDBTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_key("EMFracClassify")
{
  ATH_MSG_DEBUG("in the constructor of SiAlignDBTester");
}

SiAlignDBTester::~SiAlignDBTester()
{
  ATH_MSG_DEBUG("in the destructor of SiAlignDBTester");
}

StatusCode SiAlignDBTester::initialize()
{
  ATH_MSG_DEBUG("in the initialize of SiAlignDBTester, hello there");

  ATH_CHECK( m_key.initialize() );
  ATH_MSG_INFO( "m_key id: " << m_key.fullKey() );
  ATH_CHECK( m_readKey.initialize() );
  ATH_MSG_INFO( "m_readKey id: " << m_readKey.fullKey() );
  ATH_CHECK( m_rch_locshiftX.initialize() ); 
  ATH_MSG_INFO( "m_rch_locshiftX id: " << m_rch_locshiftX.fullKey() );
  ATH_CHECK( m_rch_globshiftX.initialize() ); 
  ATH_MSG_INFO( "m_rch_globshiftX id: " << m_rch_globshiftX.fullKey() );
  return StatusCode::SUCCESS;
}

StatusCode SiAlignDBTester::finalize()
{
  ATH_MSG_DEBUG("in the finalize of SiAlignDBTester, bye bye");
  return StatusCode::SUCCESS;
}

StatusCode SiAlignDBTester::execute()
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
    if(counter>1) ATH_MSG_INFO("there should be only one real channel in /FWD/AFP/LocalAlignment, this is real channel nr. "<<counter);

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
    if(counter>1) ATH_MSG_INFO("there should be only one real channel in /FWD/AFP/GlobalAlignment, this is real channel nr. "<<counter);

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

  return StatusCode::SUCCESS;
}

