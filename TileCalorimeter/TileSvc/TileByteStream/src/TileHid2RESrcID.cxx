/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cassert>

#include "TileByteStream/TileROD_Decoder.h" 
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileRecUtils/TileCellBuilder.h"
#include "TileByteStream/TileHid2RESrcID.h" 
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileRecUtils/ITileRawChannelTool.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "eformat/SourceIdentifier.h" 
using eformat::helper::SourceIdentifier;

// implementation of Tile Frag ID to RESrcID conversion. 
// 

TileHid2RESrcID::TileHid2RESrcID(const TileHWID* tileHWID, uint32_t runnum)
  : m_runnum(0)
{
  m_tileHWID = tileHWID;
  if (m_tileHWID) {
    // make internal maps
    initialize(runnum);
    initializeMuRcv(runnum);
  }
}

void TileHid2RESrcID::setTileHWID(const TileHWID* tileHWID, uint32_t runnum)
{
  if ( m_tileHWID != tileHWID ||  m_runnum != runnum) {
    m_tileHWID = tileHWID;
    if (m_tileHWID) {
      // make internal maps
      initialize(runnum);
    }
  }
}

void TileHid2RESrcID::initialize(uint32_t runnum)
{
  m_runnum = runnum;
  eformat::SubDetector detid[6];
  
  detid[0] = eformat::TILECAL_LASER_CRATE;    // 0x50 - beam crate
  detid[1] = eformat::TILECAL_BARREL_A_SIDE;  // 0x51 - barrel positive
  detid[2] = eformat::TILECAL_BARREL_C_SIDE;  // 0x52 - barrel negative
  detid[3] = eformat::TILECAL_EXT_A_SIDE;     // 0x53 - ext.bar positive
  detid[4] = eformat::TILECAL_EXT_C_SIDE;     // 0x54 - ext.bar negative
  detid[5] = eformat::TDAQ_BEAM_CRATE;        // 0x70 - common beam crate
  
  /** iterator over all drawer Identifiers */
  std::vector<HWIdentifier>::const_iterator first = m_tileHWID->drawer_begin();
  std::vector<HWIdentifier>::const_iterator  last = m_tileHWID->drawer_end();

  for ( ; first!=last; ++first) {
    int ros    = m_tileHWID->ros(*first);
    int drawer = m_tileHWID->drawer(*first);
    int frag   = m_tileHWID->frag(*first); 

    uint32_t id = 0; // id is always 0 for Beam ROD (ros=0)
    if (ros > 0) {
      if (runnum>318000) {
        // new frag->ROB mapping since March 2017 
        // put 4 drawers in two subsequent RODs
        // odd drawers in odd ROD, even drawers in even ROD
        id = ((drawer & 0xFC) >> 1) | (drawer & 0x1) ;
      } else {
        // old mapping
        // (put 4 drawers in one ROD - remove last 2 bits)
        id = (drawer >> 2);
      }
    } else if (drawer>0x7 && drawer < 0xFF) {
      ros = 5; // frags from common beam crate
    }
    
    // build ROD id
    SourceIdentifier sid = SourceIdentifier(detid[ros],id);    
    uint32_t rod_id =  sid.code();

    // add ROD id to the map
    m_frag2ROD[frag] = rod_id;
  }

  // laser fragments in specific ROB
  if (runnum>318000) {
    // new frag->ROB mapping since March 2017 
    m_frag2ROD[0x16] = 0x520020;
  } else {
    m_frag2ROD[0x16] = 0x520010;
  }
  m_frag2ROD[0x17] = 0;
}

void TileHid2RESrcID::initializeMuRcv(uint32_t runnum)
{
  m_TileMuRcvRunnum = runnum;
  eformat::SubDetector detid[6];

  detid[0] = eformat::TILECAL_LASER_CRATE;    // 0x50 - beam crate
  detid[1] = eformat::TILECAL_BARREL_A_SIDE;  // 0x51 - barrel positive
  detid[2] = eformat::TILECAL_BARREL_C_SIDE;  // 0x52 - barrel negative
  detid[3] = eformat::TILECAL_EXT_A_SIDE;     // 0x53 - ext.bar positive
  detid[4] = eformat::TILECAL_EXT_C_SIDE;     // 0x54 - ext.bar negative
  detid[5] = eformat::TDAQ_BEAM_CRATE;        // 0x70 - common beam crate

  // iterator over all drawer Identifiers
  std::vector<HWIdentifier>::const_iterator first = m_tileHWID->drawer_begin();
  std::vector<HWIdentifier>::const_iterator  last = m_tileHWID->drawer_end();

  for ( ; first!=last; ++first) {
    int ros    = m_tileHWID->ros(*first);
    int drawer = m_tileHWID->drawer(*first);
    int frag   = m_tileHWID->frag(*first);
    uint32_t id = 0;

    //std::cout << "TileHid2RESrcID::setTileMuRcvHWID ros:" << ros << " drawer: " << std::hex <<  drawer << " frag: " << frag << std::dec << std::endl;

    if (ros>2){
      // Extended Barrel
      // (put 8 drawers in one ROD - remove last 3 bits)
      id = (drawer >> 3) + 0x100;
    } else {
      // Long Barrel
      // (put 4 drawers in one ROD - remove last 2 bits)
      id = (drawer >> 2) + 0x100;
    }
    // build ROD id
    SourceIdentifier sid = SourceIdentifier(detid[ros],id);
    uint32_t rod_id =  sid.code();

    // add ROD id to the map
    m_TileMuRcvFrag2ROD[frag] = rod_id;
  }
}

void TileHid2RESrcID::setROD2ROBmap(const std::vector<std::string> & ROD2ROB,
                                    MsgStream & log)
{
  // overwrite ROD IDs for some frags if they are set in jobOptions
  // vector ROB2ROD contains (frag,ROB) pairs (note in eformat V3 ROD == ROB ! )
  int size=ROD2ROB.size();
  int fragCount = 0;
  for (int i=1; i<size; i+=2) {
    uint32_t frag = strtol(ROD2ROB[i-1].data(),NULL,0);
    uint32_t rob = strtol(ROD2ROB[i].data(),NULL,0) & 0xFFFFFF;
    if ( frag < 0x1000 ) { 
      // this is actually remapping for fragments inside ROB, bypassing ROD ID
      m_frag2ROD[frag] = rob;
      log << MSG::INFO << "TileHid2RESrcID:: mapping frag 0x"<< MSG::hex 
          << frag << " to ROB 0x" << rob << MSG::dec << endmsg;
      ++fragCount;
    } else {
      log << MSG::ERROR << "You are using obsolete ROD to ROB mapping" << endmsg;
      log << MSG::ERROR << "Please, replace it by frag -> ROB mapping" << endmsg;
      assert(0);
    }    
  }
  if (fragCount > 0)
    log << MSG::INFO << "TileHid2RESrcID:: " << fragCount
        << " frag to ROD remappings set via jobOptions" << endmsg;

  if (m_frag2ROD[0x17] != 0) m_frag2ROD[0x16] = m_frag2ROD[0x17];
  else m_frag2ROD[0x17] = m_frag2ROD[0x16];
}

void TileHid2RESrcID::setROD2ROBmap (const eformat::FullEventFragment<const uint32_t*> * event,
                                     bool& of2Default,
                                     MsgStream & log)
{

  uint32_t runnum = event->run_no();
  if ( (runnum>318000 && m_runnum<=318000) || (runnum<=318000 && m_runnum>318000) )
    initialize(runnum);
  
  uint32_t nBeamFrag=0, nRODfrag=0, nDataFrag[10]={0,0,0,0,0,0,0,0,0,0}, flags=0xFFFF, flags5=0xFFFF;
  std::map<int,int> fragMap;
  std::map<int,int> beamMap;
  std::vector<int> fragIDs;
  
  bool cisparFound = false;
  uint32_t lascisROB = 0;
  
  uint32_t ndet = event->nchildren();
  for (size_t idet=0; idet<ndet; ++idet) {
    const uint32_t* fpdet;
    event->child(fpdet, idet);
    const eformat::ROBFragment<const uint32_t*> robf(fpdet);

    uint32_t ROBid = robf.source_id();

    const auto ROBSubDetID = eformat::helper::SourceIdentifier(ROBid).subdetector_id();
    const auto ROBSubDetGroup = eformat::helper::SourceIdentifier(ROBid).subdetector_group();
    if (ROBSubDetGroup == eformat::SubDetectorGroup::TILECAL ||
        ROBSubDetID == eformat::SubDetector::TDAQ_BEAM_CRATE || ROBSubDetID == 0) { // SubDetID=00 used in testbeam

        unsigned int source_id   = robf.rod_source_id();
        eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(source_id);
        unsigned int subdet_id   = id.subdetector_id();

        unsigned int size = robf.rod_ndata();
        unsigned int max_allowed_size = robf.rod_fragment_size_word();
        unsigned int delta = robf.rod_header_size_word() + robf.rod_trailer_size_word();
        if (max_allowed_size > delta) max_allowed_size-=delta; else max_allowed_size=0;
        if (size > max_allowed_size) size = max_allowed_size;
        if ( size > 0 ) {

          const uint32_t * data;
          if (robf.rod_status_position()==0 && 
            robf.rod_nstatus() + robf.rod_header_size_word() + robf.rod_trailer_size_word() >= robf.rod_fragment_size_word()) {
            robf.rod_status(data);
          } else {
            robf.rod_data(data);
          }

          switch ( subdet_id ) {
          case TILE_BEAM_ID:   // TILE BEAM ROD
          case COMMON_BEAM_ID: // COMMON BEAM ROD
          case TILE_POS_ID: // POSITIVE ROD
          case TILE_NEG_ID: // NEGATIVE ROD
          case TILE_EBPOS_ID: // POSITIVE EB ROD
          case TILE_EBNEG_ID: // NEGATIVE EB ROD
            {
              
              unsigned int offset = 0, sizePos = 1, idPos = 2;
              if ( *(data) != 0xff1234ff ) { // appeared since Sep 2005
                --sizePos;
                --idPos;
              }

              while ( offset+idPos < size ) {

                uint32_t fragsize =    *(data + offset + sizePos);
                uint32_t fragid    = ( *(data + offset + idPos) ) & 0xFFFF;
                uint32_t upperhalf = ( *(data + offset + idPos) ) >> 16;
                offset += fragsize;

                if ( fragsize <= idPos || fragsize > size) break; // too small or too big frag - ignore and exit

                int fragtype = upperhalf & 0xFF;

                if (fragtype >= 0x40 && fragtype < 0x50) { // TMDB fragments

                  int ros = subdet_id&0xF;
                  if (ros>0 && ros<=4) {
                    uint32_t vers = fragid;
                    int nmod = (ros>2)?8:4; // we have 8 modules per fragment in ext.barrel, 4 modules in barrel
                    fragid = (ros<<8) | (source_id&0xF)*nmod;

                    FRAGRODMAP::const_iterator it = m_TileMuRcvFrag2ROD.find(fragid);
                    if(it == m_TileMuRcvFrag2ROD.end()){
                      log << MSG::INFO << "New TMDB frag 0x" << MSG::hex << fragid
                          << " type 0x" << fragtype << " vers 0x"<< vers
                          << " in ROB 0x" << ROBid << MSG::dec << endmsg;
                    } else {
                      if ( (*it).second != ROBid ) {
                        log << MSG::INFO << "TMDB frag 0x" << MSG::hex << fragid
                            << " type 0x" << fragtype << " vers 0x"<< vers
                            << " remapping from ROB 0x" << (*it).second
                            << " to 0x" << ROBid << MSG::dec << endmsg;
                      } else {
                        log << MSG::DEBUG << "TMDB frag 0x" << MSG::hex << fragid
                            << " type 0x" << fragtype << " vers 0x"<< vers
                            << " found in ROB 0x" << (*it).second
                            << " as expected" << MSG::dec << endmsg;
                      }
                    }
                    for (int nf=0; nf<nmod; ++nf) {
                      m_TileMuRcvFrag2ROD[fragid] = ROBid;
                      ++fragid;
                    }
                  }
                  break;
                }

                FRAGRODMAP::const_iterator it = m_frag2ROD.find(fragid); 
                if(it == m_frag2ROD.end()){
                  log << MSG::INFO << "New frag 0x" << MSG::hex << fragid 
                      << " in ROB 0x" << ROBid << MSG::dec << endmsg;
                } else {
                  if ( (*it).second != ROBid ) {
                    log << MSG::INFO << "Frag 0x" << MSG::hex << fragid 
                        <<" remapping from ROB 0x" << (*it).second
                        << " to 0x" << ROBid << MSG::dec << endmsg;
                  } else {
                    log << MSG::DEBUG << "Frag 0x" << MSG::hex << fragid 
                        <<" found in ROB 0x" << (*it).second
                        << " as expected" << MSG::dec << endmsg;
                  }
                }

                if (fragid < 0xff) { // all testbeam frags and laser frag
                    ++nBeamFrag;
                    m_frag2ROD[fragid] = ROBid;
                    ++beamMap[fragtype];
                } else if (fragid < 0x100) { // CIS par which can come from two sources
                    ++nBeamFrag;
                    ++beamMap[fragtype];
                    if (fragtype == 0x30) { // CIS par in LAST ROD
                        lascisROB = ROBid;
                    } else {
                        m_frag2ROD[fragid] = ROBid; // separate CIS par in every partition 
                        cisparFound = true;
                    }
                } else if (fragid < 0x500) { // normal drawers
                    ++nRODfrag;
                    m_frag2ROD[fragid] = ROBid;
                    ++fragMap[fragtype];
                    if (fragtype == 4) flags = upperhalf;
                    else if (fragtype == 5) {
                      flags5 = upperhalf;
                      fragIDs.push_back((int)fragid);
                    }
                }
              }
              break;
            }
          default:
            break;
          }
        }
    }
  }
  if (!cisparFound) {
    if (lascisROB) {
      m_frag2ROD[0xff] = lascisROB;
      log << MSG::INFO << "TileHid2RESrcID: Attention! Taking CISpar from lastROD 0x" << MSG::hex << lascisROB << MSG::dec << endmsg;
    } else {
      log << MSG::INFO << "TileHid2RESrcID: no CISpar in the data" << endmsg;
    }
  }
  log << MSG::INFO << "TileHid2RESrcID: " << nBeamFrag << " beam sub-frags (";
  std::map<int,int>::const_iterator itr=beamMap.begin();
  std::map<int,int>::const_iterator end=beamMap.end();
  for (; itr!=end; ++itr) {
      log << " type=0x" << MSG::hex << itr->first << MSG::dec << " nf="<<itr->second;
  }
  log << MSG::INFO << " ) and " << nRODfrag  << " ROD sub-frags (";
  itr=fragMap.begin();
  end=fragMap.end();
  for (; itr!=end; ++itr) {
      log << " type=0x" << MSG::hex << itr->first << MSG::dec << " nf="<<itr->second;
      if (itr->first < 10) nDataFrag[itr->first]+=itr->second; // id=0 - all digits, id=1 - digits above threshold
                                                               // id=2,3,4,5 - reco frags
  }
  log << " ) was found in the data" << endmsg;
  
  if (m_frag2ROD[0x17] != 0) m_frag2ROD[0x16] = m_frag2ROD[0x17];
  else m_frag2ROD[0x17] = m_frag2ROD[0x16];

  bool of2=true; // default for RUN1 data
  if (nDataFrag[4]!=0) {
    if ((flags & 0x3000) < 0x3000) // real data
      of2 = ((flags & 0x400) != 0);
  } else if (nDataFrag[5]!=0) {
    if ((flags5 & 0x3000) < 0x3000) // real data
      of2 = ((flags5 & 0x400) != 0);
  }
  log << MSG::DEBUG << "OF2 flag in data is " << ((of2)?"True":"False") << endmsg;

  if (of2 != of2Default) {
    log << MSG::INFO << "Setting OF2 flag in in TileROD_Decoder to " << ((of2) ? "True" : "False")
        << " instead of " << ((of2Default) ? "True" : "False") << endmsg;
    of2Default = of2;
  }
}

uint32_t TileHid2RESrcID::getRobFromFragID(int frag_id) const
{ 
  // this method returns a RESrcID of the ROB, for a given fragment ID
  // in eformat V3 ROB and ROD IDs are the same
  return getRodID(frag_id);
}

uint32_t TileHid2RESrcID::getRodID(int frag_id) const
{ 
  // this method returns a RESrcID for the ROD, for a given fragment ID

  FRAGRODMAP::const_iterator it = m_frag2ROD.find(frag_id); 
  if(it == m_frag2ROD.end()){
    std::cout <<" TileHid2RESrcID invalid FRAG ID 0x"<<std::hex<<frag_id<<std::dec<<std::endl;
    assert(0);
    return 0;
  }	

  return ( (*it).second ) ; 
}

/** mapping SrcID from ROD to ROB
 */ 
uint32_t TileHid2RESrcID::getRobID( uint32_t rod_id) const
{
  // in eformat V3 ROB and ROD IDs are the same
  return rod_id;
}

/** mapping SrcID from ROB to ROS
 */ 
uint32_t TileHid2RESrcID::getRosID( uint32_t rob_id) const
{
//  Change Module Type to ROS, moduleid = 0  

 SourceIdentifier  id  = SourceIdentifier(rob_id);
 SourceIdentifier  id2 = SourceIdentifier(id.subdetector_id(),0); 
 return    id2.code(); 

}

/** mapping SrcID from ROS to Det
 */ 
uint32_t TileHid2RESrcID::getDetID  ( uint32_t ros_id) const
{
//  ROS to DET

 SourceIdentifier  id  = SourceIdentifier(ros_id);
 SourceIdentifier  id2 = SourceIdentifier(id.subdetector_id(),0); 
 return    id2.code(); 
}

uint32_t TileHid2RESrcID::getRobFromTileMuRcvFragID(int frag_id) const
{
  // this method returns a RESrcID of the ROB, for a given fragment ID
  // in eformat V3 ROB and ROD IDs are the same
  return getRodTileMuRcvID(frag_id);
}

uint32_t TileHid2RESrcID::getRodTileMuRcvID(int frag_id) const
{
  // this method returns a RESrcID for the ROD, for a given fragment ID

  FRAGRODMAP::const_iterator it = m_TileMuRcvFrag2ROD.find(frag_id);
  if(it == m_TileMuRcvFrag2ROD.end()){
    std::cout <<" TileHid2RESrcID invalid FRAG ID 0x"<<std::hex<<frag_id<<std::dec<<std::endl;
    assert(0);
    return 0;
  }

  return ( (*it).second ) ;
}
