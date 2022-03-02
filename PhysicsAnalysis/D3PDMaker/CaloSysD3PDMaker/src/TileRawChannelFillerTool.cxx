/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileRawChannelFillerTool.cxx
 * Author: Stephen Cole <scole@niu.edu>
 * 
 * Created on March 1, 2011, 9:27 AM
 */

#include "TileRawChannelFillerTool.h"
#include "TileIdentifier/TileHWID.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

    TileRawChannelFillerTool::TileRawChannelFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent):
      Base(type, name,parent),
      m_tilehwid(0)
    {
      m_SaveHWid = true;
      m_SaveRawCh = false;
      TileRawChannelFillerTool::book().ignore(); // Avoid coverity warnings

      declareProperty("SaveHardwareInfo",   m_SaveHWid=true);
      declareProperty("SaveRawChannel",   m_SaveRawCh=false);
    }

    StatusCode TileRawChannelFillerTool::initialize()
    {
      StoreGateSvc* detStore = 0;
      CHECK( service("DetectorStore",detStore) );
      CHECK( detStore->retrieve(m_tilehwid) );
      return StatusCode::SUCCESS;
    }

    StatusCode TileRawChannelFillerTool::book(){
        if(m_SaveHWid){
            CHECK(addVariable("ros",m_ros));
            CHECK(addVariable("drawer",m_drawer));
            CHECK(addVariable("channel",m_channel));
            CHECK(addVariable("gain",m_gain));
        }

        if(m_SaveRawCh){
            CHECK(addVariable("amplitude",m_amplitude));
            CHECK(addVariable("time",m_time));
            CHECK(addVariable("quality",m_quality));
            CHECK(addVariable("pedestal",m_ped));
        }

        return StatusCode::SUCCESS;
    }

    StatusCode TileRawChannelFillerTool::fill(const TileRawChannel& p){
        const TileRawChannel* tileR=&p;
        if(m_SaveHWid){
            HWIdentifier hwid=tileR->adc_HWID();
            *m_ros=(char)m_tilehwid->ros(hwid);
            *m_drawer=(char)m_tilehwid->drawer(hwid);
            *m_channel=(char)m_tilehwid->channel(hwid);
            *m_gain=(char)m_tilehwid->adc(hwid);
        }
        if(m_SaveRawCh){
            *m_ped=tileR->pedestal();
            size_t nentry=tileR->size();
            m_amplitude->reserve(nentry);
            for(unsigned int n=0;n<nentry;++n){
              m_amplitude->push_back(tileR->amplitude(n));
            }
            m_quality->reserve(nentry);
            for(unsigned int n=0;n<nentry;++n){
              m_quality->push_back(tileR->quality(n));
            }
            nentry=tileR->sizeTime();
            m_time->reserve(nentry);
            for(unsigned int n=0;n<nentry;++n){
              m_time->push_back(tileR->time(n));
            }
        }
        return StatusCode::SUCCESS;
    }

}
