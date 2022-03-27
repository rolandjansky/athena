/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RodDecoder.cxx
//   Implementation file for class BCM_RodDecoder
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-01 12/05/2008 Daniel Dobos
//  Version 00-00-11 05/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RodDecoder.h"

#include <cinttypes>

static const InterfaceID IID_IBCM_RodDecoder("BCM_RodDecoder", 1, 0);
const InterfaceID& BCM_RodDecoder::interfaceID()
{ 
  return IID_IBCM_RodDecoder;
}

////////////////////////
// constructor
////////////////////////
BCM_RodDecoder::BCM_RodDecoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent)
{
  declareInterface<BCM_RodDecoder>(this);
}

////////////////////////
// destructor  
////////////////////////
BCM_RodDecoder::~BCM_RodDecoder()
{
}

////////////////////////
// initialize() - 
////////////////////////
StatusCode BCM_RodDecoder::initialize() 
{
#ifdef BCM_DEBUG
  ATH_MSG_VERBOSE( "in BCM_RodDecoder::initialize" );
#endif

  ATH_CHECK( AthAlgTool::initialize() );

  m_fragment_number = 0;
  m_LVL1A_number = 0;
  m_hit_number = 0;

  return StatusCode::SUCCESS;
}

////////////////////////
// finalize() - 
////////////////////////
StatusCode BCM_RodDecoder::finalize()
{
#ifdef BCM_DEBUG
  ATH_MSG_VERBOSE( "in BCM_RodDecoder::finalize" );
#endif

  ATH_MSG_INFO( "Bytestream summary: " << m_fragment_number << " fragments found" );
  ATH_MSG_INFO( "Bytestream summary: " << m_LVL1A_number << " LVL1As found" );
  ATH_MSG_INFO( "Bytestream summary: " << m_hit_number << " hits found" );

  return StatusCode::SUCCESS;
}

////////////////////////
// fillCollection() - 
////////////////////////
StatusCode BCM_RodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, BCM_RDO_Container* rdoCont, std::vector<unsigned int>* /*vecHash*/) const
{
#ifdef BCM_DEBUG
  ATH_MSG_INFO( "in BCM_RodDecoder::fillCollection" );
#endif

  StatusCode sc = StatusCode::SUCCESS;

  BCM_RDO_Collection* coll = nullptr;

  // set the data pointer type
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data(vint);

  // get source ID
  int ROD_source_ID = robFrag->rod_source_id();

  // get LVL1 ID
  int ROD_LVL1_ID = robFrag->rod_lvl1_id();

  // get BC ID
  //int m_ROD_BC_ID = robFrag->rod_bc_id();

  // get number of data words in the fragment
  uint32_t dataword_it_end = robFrag->rod_ndata();

  unsigned int Channel = 0;
  unsigned int Pulse1Position = 0;
  unsigned int Pulse1Width  = 0;
  unsigned int Pulse2Position = 0;
  unsigned int Pulse2Width  = 0;
  unsigned int LVL1A = 0;
  unsigned int BCID = 0;
  unsigned int Error = 0;
  uint32_t rawDataWord_buffer = 0;
  
  m_fragment_number++;

  // loop over the data words in the fragment
  for (uint32_t dataword_it = 0; dataword_it < dataword_it_end; ++dataword_it) {
     uint32_t rawDataWord = vint[dataword_it];   // set the dataword to investigate     
     unsigned int dataword_position = dataword_it%6;
     switch(dataword_position) {
	case 0:
          if (dataword_it + 5 <=dataword_it_end) {
            uint32_t lastDataWord = vint[dataword_it+5];
            Error              = (lastDataWord & 0x0000000f);
          } else {
            Error              = 15;
          }
	  BCID               = (rawDataWord & 0xfff00000) >> 20;
          LVL1A              = dataword_it / 6;
          m_LVL1A_number++;  
          Channel            = getChannelID(ROD_source_ID, 0);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x000fc000) >> 14;
	  Pulse1Width        = (rawDataWord & 0x00003e00) >> 9;
	  Pulse2Position     = (rawDataWord & 0x000001f8) >> 3;
	  rawDataWord_buffer = (rawDataWord & 0x00000007); 
	  break;
	case 1:
          Pulse2Width        = ((rawDataWord & 0xc0000000) >> 30) | (rawDataWord_buffer << 2);
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 1);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x3f000000) >> 24;
	  Pulse1Width        = (rawDataWord & 0x00f80000) >> 19;
	  Pulse2Position     = (rawDataWord & 0x0007e000) >> 13;
	  Pulse2Width        = (rawDataWord & 0x00001f00) >> 8;
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 2);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x000000fc) >> 2;
	  rawDataWord_buffer = (rawDataWord & 0x00000003);
	  break;
	case 2:
	  Pulse1Width        = ((rawDataWord & 0xe0000000) >> 29) | (rawDataWord_buffer << 3);
	  Pulse2Position     = (rawDataWord & 0x1f800000) >> 23;
	  Pulse2Width        = (rawDataWord & 0x007c0000) >> 18;
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 3);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x0003f000) >> 12;
	  Pulse1Width        = (rawDataWord & 0x00000f80) >> 7;
	  Pulse2Position     = (rawDataWord & 0x0000007e) >> 1;
	  rawDataWord_buffer = (rawDataWord & 0x00000001);	  	  
	  break;
	case 3:
	  Pulse2Width        = ((rawDataWord & 0xf0000000) >> 28) | (rawDataWord_buffer << 4);
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 4);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x0fc00000) >> 22;
	  Pulse1Width        = (rawDataWord & 0x003e0000) >> 17;
	  Pulse2Position     = (rawDataWord & 0x0001f800) >> 11;
	  Pulse2Width        = (rawDataWord & 0x000007c0) >> 6;
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width);
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 5);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x0000003f);
	  break;
	case 4:
	  Pulse1Width        = (rawDataWord & 0xf8000000) >> 27;
	  Pulse2Position     = (rawDataWord & 0x07e00000) >> 21;
	  Pulse2Width        = (rawDataWord & 0x001f0000) >> 16;
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 6);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x0000fc00) >> 10;
	  Pulse1Width        = (rawDataWord & 0x000003e0) >> 5;
	  rawDataWord_buffer = (rawDataWord & 0x0000001f);
	  break;
	case 5:
	  Pulse2Position     = ((rawDataWord & 0x80000000) >> 31) | (rawDataWord_buffer << 1);
	  Pulse2Width        = (rawDataWord & 0x7c000000) >> 26;
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
          m_hit_number++;
          Channel            = getChannelID(ROD_source_ID, 7);
          coll             = getCollection(Channel,rdoCont);
	  Pulse1Position     = (rawDataWord & 0x03f00000) >> 20;
	  Pulse1Width        = (rawDataWord & 0x000f8000) >> 15;
	  Pulse2Position     = (rawDataWord & 0x00007e00) >> 9;
	  Pulse2Width        = (rawDataWord & 0x000001f0) >> 4;
#ifdef BCM_DEBUG
          if (Pulse1Width != 0 || Pulse2Width != 0) {
            ATH_MSG_ERROR( "BCM-ISSUE L1ID: " << ROD_LVL1_ID << " Ch: " << Channel << " L1A: " << LVL1A << " BCID: " << BCID << " Hit: " << Pulse1Position << " " << Pulse1Width << " " << Pulse2Position << " " << Pulse2Width ); 
          }
          ATH_MSG_DEBUG( "Decoded ROD 0x"<< std::hex<<ROD_source_ID<<std::dec <<" channel: "<< Channel <<" - BCID: "<< BCID <<" : "<< Pulse1Position <<"-"<< Pulse1Width <<" "<< Pulse2Position <<"-"<< Pulse2Width );
#endif
	  coll->push_back(new RDO(Channel,Pulse1Position,Pulse1Width,Pulse2Position,Pulse2Width,LVL1A,BCID,ROD_LVL1_ID,Error));
	  m_hit_number++;
          break;
     }
  }

  return sc;
}

////////////////////////
// getChannelID() - convert ROD source ID, dataword position combination into ChannelID number 
////////////////////////
unsigned int BCM_RodDecoder::getChannelID(int ROD_source_ID, unsigned int dataword_position) const
{
  switch(ROD_source_ID) {
    // Run-1 Fragment 
  case 0x0081000A:  
    switch(dataword_position) {
    case 0:
      return 0;   
    case 1:
      return 9;
    case 2:
      return 2;
    case 3:
      return 11;
    case 4:
      return 4;   
    case 5:
      return 13;
    case 6:
      return 6;
    case 7:
      return 15;
    default:
      return 0xffffffff;
    }
    // Run-2 Fragment
  case 0x0081004A:  
    switch(dataword_position) {
    case 0:
      return 0;   
    case 1:
      return 9;
    case 2:
      return 2;
    case 3:
      return 11;
    case 4:
      return 4;   
    case 5:
      return 13;
    case 6:
      return 6;
    case 7:
      return 15;
    default:
      return 0xffffffff;
    }
    // Run-1 Fragment
    case 0x0081000C:
    switch(dataword_position) {
    case 0:
      return 8;   
    case 1:
      return 1;
    case 2:
      return 10;
    case 3:
      return 3;
    case 4:
      return 12;   
    case 5:
      return 5;
    case 6:
      return 14;
    case 7:
      return 7;
    default:
      return 0xffffffff;
    }
    // Run-2 fragment
  case 0x0081004C:
    switch(dataword_position) {
    case 0:
      return 8;   
    case 1:
      return 1;
    case 2:
      return 10;
    case 3:
      return 3;
    case 4:
      return 12;   
    case 5:
      return 5;
    case 6:
      return 14;
    case 7:
      return 7;
    default:
      return 0xffffffff;
    }
  default:
    return 0xffffffff;
  }
}

////////////////////////
// getCollection() - return collection corresponding to a particular channel 
//                   if it exists in container, or create it if it doesn't
////////////////////////
BCM_RDO_Collection* BCM_RodDecoder::getCollection(unsigned int chan, BCM_RDO_Container* cont) const
{
  BCM_RDO_Collection* coll;
  BCM_RDO_Container::iterator cont_it;
  bool collExists = false;
  BCM_RDO_Container::iterator it = cont->begin();
  BCM_RDO_Container::iterator itE= cont->end();
  for (; it!=itE; ++it) {
    if ((*it)->getChannel() == chan) {
      collExists = true;
      cont_it = it;
    }
  }
  if (collExists) {
    coll = *cont_it;
  } else {  // if collection does not exist create it
    coll = new BCM_RDO_Collection(chan);
    cont->push_back(coll);  // add collection to container
  }
  return coll;
}
