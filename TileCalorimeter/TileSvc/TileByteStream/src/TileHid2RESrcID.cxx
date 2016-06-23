/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/MsgStream.h"

#include "eformat/SourceIdentifier.h" 
using eformat::helper::SourceIdentifier;

// implementation of Tile Frag ID to RESrcID conversion. 
// 

TileHid2RESrcID::TileHid2RESrcID(const TileHWID* tileHWID)
{
  setTileHWID(tileHWID);
  setTileMuRcvHWID(tileHWID); 
}

void TileHid2RESrcID::setTileHWID(const TileHWID* tileHWID)
{
  m_tileHWID = tileHWID;
  if (!m_tileHWID) return;  

  // make internal maps 
  
  eformat::SubDetector detid[6];
  
  detid[0] = eformat::TILECAL_LASER_CRATE;    // 0x50 - beam crate
  detid[1] = eformat::TILECAL_BARREL_A_SIDE;  // 0x51 - barrel positive
  detid[2] = eformat::TILECAL_BARREL_C_SIDE;  // 0x52 - barrel negative
  detid[3] = eformat::TILECAL_EXT_A_SIDE;     // 0x53 - ext.bar positive
  detid[4] = eformat::TILECAL_EXT_C_SIDE;     // 0x54 - ext.bar negative
  detid[5] = eformat::TDAQ_BEAM_CRATE;        // 0x70 - common beam crate
  
  /** iterator over all drawer Identifiers */
  std::vector<HWIdentifier>::const_iterator first = tileHWID->drawer_begin();
  std::vector<HWIdentifier>::const_iterator  last = tileHWID->drawer_end();

  for ( ; first!=last; ++first) {
    int ros    = tileHWID->ros(*first);
    int drawer = tileHWID->drawer(*first);
    int frag   = tileHWID->frag(*first); 

    uint32_t id = 0; // id is always 0 for Beam ROD (ros=0)
    // (put 4 drawers in one ROD - remove last 2 bits)
    if (ros > 0) id = (drawer >> 2);
    else if (drawer>0x7 && drawer < 0xFF) ros = 5; // frags from common beam crate
    
    // build ROD id
    SourceIdentifier sid = SourceIdentifier(detid[ros],id);    
    uint32_t rod_id =  sid.code();

    // add ROD id to the map
    if (frag != 0x16)
      m_frag2ROD[frag] = rod_id;
    else
      m_frag2ROD[frag] = 0x520010;
  }
}

void TileHid2RESrcID::setTileMuRcvHWID(const TileHWID* tileHWID)
{
  m_tileHWID = tileHWID;
  if (!m_tileHWID) return;

  // make internal maps

  eformat::SubDetector detid[6];

  detid[0] = eformat::TILECAL_LASER_CRATE;    // 0x50 - beam crate
  detid[1] = eformat::TILECAL_BARREL_A_SIDE;  // 0x51 - barrel positive
  detid[2] = eformat::TILECAL_BARREL_C_SIDE;  // 0x52 - barrel negative
  detid[3] = eformat::TILECAL_EXT_A_SIDE;     // 0x53 - ext.bar positive
  detid[4] = eformat::TILECAL_EXT_C_SIDE;     // 0x54 - ext.bar negative
  detid[5] = eformat::TDAQ_BEAM_CRATE;        // 0x70 - common beam crate

  // iterator over all drawer Identifiers
  std::vector<HWIdentifier>::const_iterator first = tileHWID->drawer_begin();
  std::vector<HWIdentifier>::const_iterator  last = tileHWID->drawer_end();

  for ( ; first!=last; ++first) {
    int ros    = tileHWID->ros(*first);
    int drawer = tileHWID->drawer(*first);
    int frag   = tileHWID->frag(*first);
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
      if ( rob < 0x500000 ) {
        log << MSG::ERROR << "You are using obsolete mapping" << endreq;
        log << MSG::ERROR << "Please, upgrade it to event format V3" << endreq;
        assert(0);
        log << MSG::ERROR << "TileHid2RESrcID:: mapping frag 0x"<< MSG::hex 
            << frag << " to ROB 0x" << rob << MSG::dec << endreq;
      }
      // this is actually remapping for fragments inside ROB, bypassing ROD ID
      m_frag2ROD[frag] = rob;
      log << MSG::INFO << "TileHid2RESrcID:: mapping frag 0x"<< MSG::hex 
          << frag << " to ROB 0x" << rob << MSG::dec << endreq;
      ++fragCount;
    } else {
      log << MSG::ERROR << "You are using obsolete ROD to ROB mapping" << endreq;
      log << MSG::ERROR << "Please, replace it by frag -> ROB mapping" << endreq;
      assert(0);
    }    
  }
  if (fragCount > 0)
    log << MSG::INFO << "TileHid2RESrcID:: " << fragCount
        << " frag to ROD remappings set via jobOptions" << endreq;
}

void TileHid2RESrcID::setROD2ROBmap (const eformat::FullEventFragment<const uint32_t*> * event,
                                     MsgStream & log)
{
  MSG::Level logLevel = log.level();
  bool debug = (logLevel<=MSG::DEBUG);

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

                if (fragtype >= 0x40 && fragtype < 0x50) break;

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
  if (!cisparFound) {
    if (lascisROB) {
      m_frag2ROD[0xff] = lascisROB;
      log << MSG::INFO << "TileHid2RESrcID: Attention! Taking CISpar from lastROD 0x" << MSG::hex << lascisROB << MSG::dec << endreq;
    } else {
      log << MSG::INFO << "TileHid2RESrcID: no CISpar in the data" << endreq;
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
  log << " ) was found in the data" << endreq;
  
  if ((nDataFrag[0]+nDataFrag[1]==0 || (nDataFrag[2]+nDataFrag[3]+nDataFrag[4]==0)) && nDataFrag[5] > 0 ) {
    // only frag5 in the data - make sure that TileROD_Decoder is configured properly
    StatusCode sc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    ThreadGaudi* threadGaudi = ThreadGaudi::instance();
    IToolSvc* toolSvc;
    sc = svcLoc->service( "ToolSvc"+threadGaudi->getThreadID(),toolSvc);
    if(sc.isFailure()){
      log << MSG::WARNING << "TileHid2RESrcID: Can not retrieve ToolSvc" << endreq;
      return;
    }

    // Get TileROD_Decoder
    TileROD_Decoder* rodDecoder=0;
    sc = toolSvc->retrieveTool("TileROD_Decoder",rodDecoder);
    if (sc.isFailure()) {
      log << MSG::WARNING << "TileHid2RESrcID: Unable to retrieve algtool TileROD_Decoder" << endreq;
      return;
    }

    int unit = flags5 >> 14;

    if (nDataFrag[0]+nDataFrag[1]==0) {
      rodDecoder->setUseFrag0 (false);
      rodDecoder->setUseFrag1 (false);
      rodDecoder->setUseFrag5Raw (true);
      log << MSG::INFO << "Setting TileROD_Decoder->useFrag5Raw to true, unit is " << unit << endreq;
      std::vector<int>::const_iterator itr=fragIDs.begin();
      std::vector<int>::const_iterator end=fragIDs.end();
      for ( ; itr!=end; ++itr) {
        if (debug) 
          log << MSG::DEBUG << "Init OFC for frag 0x" << MSG::hex << (*itr) << MSG::dec << endreq;
        rodDecoder->getOFW((*itr),unit);
      }
    }

    if (nDataFrag[2]+nDataFrag[3]+nDataFrag[4]==0) {
      rodDecoder->setUseFrag4 (false);
      rodDecoder->setUseFrag5Reco (true);
      log << MSG::INFO << "Setting TileROD_Decoder->useFrag5Reco to true, unit is " << unit << endreq;
    }
    if (toolSvc->releaseTool(rodDecoder).isFailure()) 
      log << MSG::WARNING << "Unable to release algtool TileROD_Decoder" << endreq;
  }

  bool of2=true; // default for RUN1 data
  if (nDataFrag[4]!=0) {
    if ((flags & 0x3000) < 0x3000) // real data
      of2 = ((flags & 0x400) != 0);
  } else if (nDataFrag[5]!=0) {
    if ((flags5 & 0x3000) < 0x3000) // real data
      of2 = ((flags5 & 0x400) != 0);
  }
  log << MSG::DEBUG << "OF2 flag in data is " << ((of2)?"True":"False") << endreq;

  bool do_merge = ( (nDataFrag[0]+nDataFrag[1]+nDataFrag[2]+nDataFrag[3]+nDataFrag[4]+nDataFrag[5] == 0) ||
                    nDataFrag[1] > 0 || nDataFrag[4] > 0);

  if (nDataFrag[0] == 0 && (do_merge || nDataFrag[2]+nDataFrag[3]+nDataFrag[4] > 0) && nDataFrag[5] == 0 ) {
    StatusCode sc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    ThreadGaudi* threadGaudi = ThreadGaudi::instance();
    IToolSvc* toolSvc;
    sc = svcLoc->service( "ToolSvc"+threadGaudi->getThreadID(),toolSvc);
    if(sc.isFailure()){
      log << MSG::WARNING << "TileHid2RESrcID: Can not retrieve ToolSvc" << endreq;
      return;
    }

    // Get Cell Builder
    TileCellBuilder* cellBuilder=0;
    sc = toolSvc->retrieveTool("TileCellBuilder",cellBuilder);
    if (sc.isFailure()) {
      log << MSG::WARNING << "TileHid2RESrcID: Unable to retrieve algtool TileCellBuilder " << endreq;
      return;
    }

    const Property& rawChannelCnt_prop = cellBuilder->getProperty ("TileRawChannelContainer");
    const Property& dspRawChannelContainer_prop = cellBuilder->getProperty ("TileDSPRawChannelContainer");
    const PropertyWithValue<bool>& mergeChannels_prop =
      dynamic_cast<const PropertyWithValue<bool>&> (cellBuilder->getProperty ("mergeChannels"));

    // Get any RawChannel Builder
    if (debug)
      log << MSG::DEBUG << " looking for TileRawChannelBuilder tool which provides container with name " 
          << rawChannelCnt_prop.toString() << endreq;
    TileRawChannelBuilder* channelBuilder=0;
    std::string toolType[6] = { "TileRawChannelBuilderOpt2Filter", "TileRawChannelBuilderOptFilter",
                                "TileRawChannelBuilderFitFilter", "TileRawChannelBuilderFitFilterCool", 
                                "TileRawChannelBuilderManyAmps", "TileRawChannelBuilderFlatFilter" };
    for (int i=0; i<6; ++i) {
      std::vector<std::string> toolName = toolSvc->getInstances(toolType[i]);
      if (debug)
        log << MSG::DEBUG << toolName.size() << " tools of type " <<  toolType[i] << " found " << endreq;
      if (toolName.size()!=0) {
        for (size_t j=0; j<toolName.size(); ++j) {
          size_t found = toolName[j].find('.');
          std::string name=(found!=std::string::npos) ? toolName[j].substr(found+1) : toolName[j];
          if (debug)
            log << MSG::DEBUG << " full name " << toolName[j] << " - using " << name << endreq;
          if (toolSvc->retrieveTool(name,channelBuilder).isSuccess()) {
            if (debug)
              log << MSG::DEBUG << " it provides " << channelBuilder->getTileRawChannelContainerID() << endreq;
            if (rawChannelCnt_prop.toString() == channelBuilder->getTileRawChannelContainerID()) {
              break;
            } else {
              if (toolSvc->releaseTool(channelBuilder).isFailure()) 
                log << MSG::WARNING << "Unable to release algtool " << channelBuilder << endreq;
              channelBuilder=0;
            }
          }
        }
        if (channelBuilder) break;
      }
    }

    if (do_merge) { // frag1 in the data - should merge offline and dsp reco
      if (rawChannelCnt_prop.toString() != dspRawChannelContainer_prop.toString() &&
          !mergeChannels_prop.value()) {
        if (nDataFrag[1] > 0) 
          log << MSG::INFO << "TileHid2RESrcID: only frag1 digi found, changing properties for TileCellBuilder";
        else 
          log << MSG::INFO << "TileHid2RESrcID: no TileCal digits found, but still changing properties for TileCellBuilder";
        log << " mergeChannels=True" << endreq;
        sc=cellBuilder->setProperty ("mergeChannels", true);
        if (sc.isFailure()) {
          log << MSG::ERROR << "Failed to set mergeChannels property in TileCellBuilder" << endreq;
        }
        if (channelBuilder && cellBuilder->m_noiseFilterTools.size() != channelBuilder->m_noiseFilterTools.size()) {
          log << MSG::INFO << " and number of NoiseFilterTools from " 
              << cellBuilder->m_noiseFilterTools.size() << " to " << channelBuilder->m_noiseFilterTools.size() 
              << " i.e. from " << cellBuilder->m_noiseFilterTools << " to " << channelBuilder->m_noiseFilterTools
              << endreq;
          cellBuilder->m_noiseFilterTools = channelBuilder->m_noiseFilterTools;
          sc=cellBuilder->m_noiseFilterTools.retrieve();
          if (sc.isFailure()) {
            log << MSG::WARNING << "Failed to retrieve " << cellBuilder->m_noiseFilterTools << endreq;
           } else {
            if (debug)
              log << MSG::DEBUG << "Successfully retrieved " << cellBuilder->m_noiseFilterTools << endreq;
          }
        }
      }
    } else { // neither frag0 nor frag1 in the data 
      // make sure that we read raw channels from BS, because offline reco will not work without digits anyhow
      if (rawChannelCnt_prop.toString() != dspRawChannelContainer_prop.toString()) {
        log << MSG::INFO << "TileHid2RESrcID: no digi frags found, changing properties for TileCellBuilder";
        log << " TileRawChannelContainer='" << dspRawChannelContainer_prop.toString() 
            << "' instead of '" << rawChannelCnt_prop.toString() << "'";
        sc=cellBuilder->setProperty ("TileRawChannelContainer",dspRawChannelContainer_prop.toString());
        if (sc.isFailure()) {
          log << MSG::ERROR << "Failed to set TileRawChannelContainer property in TileCellBuilder" << endreq;
        }
        flags = (flags>>8) & 0x3; // this is number of iterations
        if (flags == 0) { // no iterations - assume best phase was used, just need amplitude correction
          log << " and correctAmplitude=True";
          cellBuilder->m_correctAmplitude = true;
          cellBuilder->m_correctTime = false;
        } else { // with iterations - amplitude corrections are not needed, but want to correct time
          log << " and correctTime=True";
          cellBuilder->m_correctAmplitude = false;
          cellBuilder->m_correctTime = true;
        }
        log << " and of2=" << ((of2)?"True":"False");
        cellBuilder->m_of2 = of2; 
        log << endreq;
        if (channelBuilder && cellBuilder->m_noiseFilterTools.size() != channelBuilder->m_noiseFilterTools.size()) {
          log << MSG::INFO << " and number of NoiseFilterTools from " 
              << cellBuilder->m_noiseFilterTools.size() << " to " << channelBuilder->m_noiseFilterTools.size() 
              << " i.e. from " << cellBuilder->m_noiseFilterTools << " to " << channelBuilder->m_noiseFilterTools
              << endreq;
          cellBuilder->m_noiseFilterTools = channelBuilder->m_noiseFilterTools;
          sc=cellBuilder->m_noiseFilterTools.retrieve();
          if (sc.isFailure()) {
            log << MSG::WARNING << "Failed to retrieve " << cellBuilder->m_noiseFilterTools << endreq;
          }
          else {
            if (debug)
              log << MSG::DEBUG << "Successfully retrieved " << cellBuilder->m_noiseFilterTools << endreq;
          }
        }
      }
    }

    if (channelBuilder) {
      if (fabs(cellBuilder->m_ampMinThresh - channelBuilder->m_ampMinThresh) > 1.e-3) {
        log << MSG::INFO << "Setting AmpMinForAmpCorrection in TileCellBuilder to " << channelBuilder->m_ampMinThresh
            << " instead of " << cellBuilder->m_ampMinThresh << endreq;
        cellBuilder->m_ampMinThresh = channelBuilder->m_ampMinThresh;
      }
      if (fabs(cellBuilder->m_timeMinThresh - channelBuilder->m_timeMinThresh) > 1.e-3) {
        log << MSG::INFO << "Setting TimeMinForAmpCorrection in TileCellBuilder to " << channelBuilder->m_timeMinThresh
            << " instead of " << cellBuilder->m_timeMinThresh << endreq;
        cellBuilder->m_timeMinThresh = channelBuilder->m_timeMinThresh;
      }
      if (fabs(cellBuilder->m_timeMaxThresh - channelBuilder->m_timeMaxThresh) > 1.e-3) {
        log << MSG::INFO << "Setting TimeMaxForAmpCorrection in TileCellBuilder to " << channelBuilder->m_timeMaxThresh
            << " instead of " << cellBuilder->m_timeMaxThresh << endreq;
        cellBuilder->m_timeMaxThresh = channelBuilder->m_timeMaxThresh;
      }
   }

    // tune options in TileROD_Decoder
    TileROD_Decoder* rodDecoder=0;
    sc = toolSvc->retrieveTool("TileROD_Decoder",rodDecoder);
    if (sc.isFailure()) {
      log << MSG::WARNING << "TileHid2RESrcID: Unable to retrieve algtool TileROD_Decoder" << endreq;
    } else {
      if (fabs(cellBuilder->m_ampMinThresh - rodDecoder->m_ampMinThresh) > 1.e-3) {
        log << MSG::INFO << "Setting AmpMinForAmpCorrection in TileROD_Decoder to " << cellBuilder->m_ampMinThresh
            << " instead of " << rodDecoder->m_ampMinThresh << endreq;
        rodDecoder->updateAmpThreshold(cellBuilder->m_ampMinThresh);
      }
      if (fabs(cellBuilder->m_timeMinThresh - rodDecoder->m_timeMinThresh) > 1.e-3) {
        log << MSG::INFO << "Setting TimeMinForAmpCorrection in TileROD_Decoder to " << cellBuilder->m_timeMinThresh
            << " instead of " << rodDecoder->m_timeMinThresh << endreq;
        rodDecoder->m_timeMinThresh = cellBuilder->m_timeMinThresh;
      }
      if (fabs(cellBuilder->m_timeMaxThresh - rodDecoder->m_timeMaxThresh) > 1.e-3) {
        log << MSG::INFO << "Setting TimeMaxForAmpCorrection in TileROD_Decoder to " << cellBuilder->m_timeMaxThresh
            << " instead of " << rodDecoder->m_timeMaxThresh << endreq;
        rodDecoder->m_timeMaxThresh = cellBuilder->m_timeMaxThresh;
      }
      if (of2 != rodDecoder->m_of2) {
        log << MSG::INFO << "Setting OF2 flag in in TileROD_Decoder to " << ((of2)?"True":"False")
            << " instead of " << ((rodDecoder->m_of2)?"True":"False") << endreq;
        rodDecoder->m_of2 = of2;
      }
    }
  
    if (toolSvc->releaseTool(cellBuilder).isFailure()) 
      log << MSG::WARNING << "Unable to release algtool TileCellBuilder" << endreq;
    if (channelBuilder)
      if (toolSvc->releaseTool(channelBuilder).isFailure()) 
        log << MSG::WARNING << "Unable to release algtool TileRawChannelBuilder" << endreq;
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
