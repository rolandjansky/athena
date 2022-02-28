/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileDigitFillerTool.cxx
 * Author: Stephen Cole <scole@niu.edu>
 * 
 * Created on February 22, 2011, 4:06 PM
 */

#include "TileDigitFillerTool.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloIdentifier/TileID.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD{

    TileDigitFillerTool::TileDigitFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent):
      BlockFillerTool<TileDigits>(type,name,parent),
      m_tileid(0),
      m_tilehwid(0)
    {
      declareProperty("SaveOfflineInfo",   m_SaveOffId=false);
      declareProperty("SaveHardwareInfo",  m_SaveHWid=true);

      m_section = 0;
      m_side = 0;
      m_tower = 0;
      TileDigitFillerTool::book().ignore(); // Avoid coverity warnings
    }


    /**
     * @brief Standard Gaudi initialize method.
     */
    StatusCode TileDigitFillerTool::initialize()
    {
      StoreGateSvc* detStore = 0;
      CHECK( service("DetectorStore",detStore) );
      CHECK( detStore->retrieve(m_tileid) );
      CHECK( detStore->retrieve(m_tilehwid) );
      return StatusCode::SUCCESS;
    }



    StatusCode TileDigitFillerTool::book(){
        if(m_SaveHWid){
            CHECK(addVariable("ros",m_ros));
            CHECK(addVariable("drawer",m_drawer));
            CHECK(addVariable("channel",m_channel));
            CHECK(addVariable("gain",m_gain));
            CHECK(addVariable("samples",m_samples));
        }

        if(m_SaveOffId){
            CHECK(addVariable("section",m_section));
            CHECK(addVariable("side",m_side));
            CHECK(addVariable("tower",m_tower));
        }
        return StatusCode::SUCCESS;
    }

    StatusCode TileDigitFillerTool::fill(const TileDigits& p){
        const TileDigits* tileD=&p;
        HWIdentifier hwid=tileD->adc_HWID();

        if(m_SaveHWid){
            *m_ros=(char)m_tilehwid->ros(hwid);
            *m_drawer=(char)m_tilehwid->drawer(hwid);
            *m_channel=(char)m_tilehwid->channel(hwid);
            *m_gain=(char)m_tilehwid->adc(hwid);

            const std::vector<float>& Samples=tileD->samples();
            m_samples->assign (Samples.begin(), Samples.end());
        }

        if(m_SaveOffId){
            Identifier digID=tileD->adc_ID();
            *m_section=(char)m_tileid->section(digID);
            *m_side=(char)m_tileid->side(digID);
            *m_tower=(char)m_tileid->tower(digID);
        }

        return StatusCode::SUCCESS;
    }

}
