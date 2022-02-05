/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileModuleBitsFillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on March 15, 2012, 1:59 PM
 */

#include "TileModuleBitsFillerTool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

    TileModuleBitsFillerTool::TileModuleBitsFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent):Base(type, name,parent) {
        TileModuleBitsFillerTool::book().ignore();  //Avoid coverity warnings
    }

    StatusCode TileModuleBitsFillerTool::initialize() {
        StoreGateSvc* detStore;
        CHECK(service("DetectorStore",detStore));
        return StatusCode::SUCCESS; 
    }

    StatusCode TileModuleBitsFillerTool::book(){
        
        CHECK(addVariable("ID",m_ID));
        CHECK(addVariable("FragGlobalCRC",m_FragGlobalCRC));
        CHECK(addVariable("FragDSPBCID",m_FragDSPBCID));
        CHECK(addVariable("FragBCID",m_FragBCID));
        CHECK(addVariable("FragMemoryPar",m_FragMemoryPar));
        CHECK(addVariable("FragSstrobe",m_FragSstrobe));
        CHECK(addVariable("FragDstrobe",m_FragDstrobe));
        CHECK(addVariable("FragHeaderBit",m_FragHeaderBit));
        CHECK(addVariable("FragHeaderPar",m_FragHeaderPar));
        CHECK(addVariable("FragSampleBit",m_FragSampleBit));
        CHECK(addVariable("FragSamplePar",m_FragSamplePar));
        CHECK(addVariable("FragFEChipMask",m_FragFEChipMask));
        CHECK(addVariable("FragRODChipMask",m_FragRODChipMask));

        
        return StatusCode::SUCCESS;
    }

    StatusCode TileModuleBitsFillerTool::fill(const TileRawChannelCollection& p){
        
        *m_ID=p.identify();
        *m_FragGlobalCRC=p.getFragGlobalCRC();
        *m_FragDSPBCID=p.getFragDSPBCID();
        *m_FragBCID=p.getFragBCID();
        *m_FragMemoryPar=p.getFragMemoryPar();
        *m_FragSstrobe=p.getFragSstrobe();
        *m_FragDstrobe=p.getFragDstrobe();
        *m_FragHeaderBit=p.getFragHeaderBit();
        *m_FragHeaderPar=p.getFragHeaderPar();
        *m_FragSampleBit=p.getFragSampleBit();
        *m_FragSamplePar=p.getFragSamplePar();
        *m_FragFEChipMask=p.getFragFEChipMask();
        *m_FragRODChipMask=p.getFragRODChipMask();
        
        
        return StatusCode::SUCCESS;
    }

}
