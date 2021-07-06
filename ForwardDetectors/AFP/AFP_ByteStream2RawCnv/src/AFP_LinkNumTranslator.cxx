/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_LinkNumTranslator.h"

AFP_LinkNumTranslator::AFP_LinkNumTranslator(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

AFP_LinkNumTranslator::~AFP_LinkNumTranslator()
{
}

StatusCode AFP_LinkNumTranslator::initialize()
{
  if(m_useDB)
  {
    ATH_MSG_WARNING("Using DB is not implemented yet");
/*
    ATH_CHECK( m_readDBKey_AF.initialize() );
    ATH_MSG_INFO( "Using DB with key " << m_readDBKey_AF.fullKey() );
    ATH_CHECK( m_readDBKey_AN.initialize() );
    ATH_MSG_INFO( "Using DB with key " << m_readDBKey_AN.fullKey() );
    ATH_CHECK( m_readDBKey_CF.initialize() );
    ATH_MSG_INFO( "Using DB with key " << m_readDBKey_CF.fullKey() );
    ATH_CHECK( m_readDBKey_CN.initialize() );
    ATH_MSG_INFO( "Using DB with key " << m_readDBKey_CN.fullKey() );
*/
  }
  else
  {
    ATH_MSG_INFO("Using hard-coded values");
  }
  return StatusCode::SUCCESS;
}

StatusCode AFP_LinkNumTranslator::finalize()
{
  return StatusCode::SUCCESS;
}

unsigned int AFP_LinkNumTranslator::translate(unsigned int origlink) const
{

  const EventContext& ctx = Gaudi::Hive::currentContext();
  // EventIDBase t( ctx.eventID() );

  if(m_useDB)
  {
    ATH_MSG_WARNING("Using DB is not implemented yet, will return 0");
    return 0;
    
    // TODO: distinguish between A and C?
    // TODO: change based on reality
  /*  
    SG::ReadCondHandle<CondAttrListCollection> readHandle_AF( m_readDBKey_AF, ctx );
    const CondAttrListCollection* attrLocList_AF { *readHandle_AF };
    if ( attrLocList_AF == nullptr )
    {
      ATH_MSG_ERROR("data for key " << m_readDBKey_AF.fullKey() << " not found");
      return 0;
    }
    CondAttrListCollection::const_iterator chanIt=attrLocList_AF->begin();
    CondAttrListCollection::const_iterator chanIt_e=attrLocList_AF->end();
    for(;chanIt!=chanIt_e;++chanIt) {
      const unsigned channel=chanIt->first;
      const coral::AttributeList& attr=chanIt->second;
      int linkNr = (attr)["linkNr"].data<int>(); // TODO: get link number from the blob
      ATH_MSG_INFO("execute: run "<<ctx.eventID().run_number()<<", lb "<<ctx.eventID().lumi_block()<<", evnt "<<ctx.eventID().event_number()<<", channel "<<channel<<", linkNr "<<linkNr);

      if((unsigned int)linkNr==origlink)
      {
        switch(channel)
        {
          case 0: return kNS0;
          case 1: return kNS1;
          case 2: return kNS2;
          case 3: return kNS3;
        }
      }
    }

    // not found in far, try near
    SG::ReadCondHandle<CondAttrListCollection> readHandle_AN( m_readDBKey_AN, ctx );
    const CondAttrListCollection* attrLocList_AN { *readHandle_AN };
    if ( attrLocList_AN == nullptr )
    {
      ATH_MSG_ERROR("data for key " << m_readDBKey_AN.fullKey() << " not found");
      return 0;
    }
    chanIt=attrLocList_AN->begin();
    chanIt_e=attrLocList_AN->end();
    for(;chanIt!=chanIt_e;++chanIt) {
      const unsigned channel=chanIt->first;
      const coral::AttributeList& attr=chanIt->second;
      int linkNr = (attr)["linkNr"].data<int>(); // TODO: get link number from the blob
      ATH_MSG_INFO("execute: run "<<ctx.eventID().run_number()<<", lb "<<ctx.eventID().lumi_block()<<", evnt "<<ctx.eventID().event_number()<<", channel "<<channel<<", linkNr "<<linkNr);

      if((unsigned int)linkNr==origlink)
      {
        switch(channel)
        {
          case 0: return kFS0;
          case 1: return kFS1;
          case 2: return kFS2;
          case 3: return kFS3;
          case 4: return kFS4;
          case 5: return kFS5;
        }
      }
    }

    ATH_MSG_ERROR("did not find the corresponding entry in DB; requested translation of link number " << origlink << " for run " << ctx.eventID().run_number());
    return 0;
    */
  }
  else
  {
    // don't use DB

    int run_nr = ctx.eventID().run_number();
    int Run = (run_nr > 370000 ? 3 : 2);

    if(Run==2)
    {
      ATH_MSG_DEBUG("asked for link "<<origlink<<", from run "<<run_nr<<", Run "<<Run);
      return origlink;
    }
    else if(Run==3)
    {
      unsigned int kReturn=999;
      switch(origlink)
      {
        case 0: kReturn=kNS0; break;
        case 2: kReturn=kNS1; break;
        case 4: kReturn=kNS2; break;
        case 6: kReturn=kNS3; break;
        case 8: kReturn=kFS0; break;
        case 10: kReturn=kFS1; break;
        case 12: kReturn=kFS2; break;
        case 14: kReturn=kFS3; break;
        case 9:  kReturn=kFS4; break;
        case 11: kReturn=kFS5; break;
        default:
          ATH_MSG_ERROR("requested translation of unknown link number " << origlink << " for run " << run_nr);
          kReturn=0;
      }
      ATH_MSG_DEBUG("asked for link "<<origlink<<", from run "<<run_nr<<", Run "<<Run<<", will return "<<kReturn);

      return kReturn;
    }
    else
    {
      ATH_MSG_ERROR("requested link " << origlink << " for unknown Run " << Run );
      return 0;
    }
  }

}


