/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellRawAssociationTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on October 26, 2011, 11:08 AM
 */

#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileRawChannel.h"
#include "TileEvent/TileCell.h"

#include "TileCellRawAssociationTool.h"


using namespace std;

namespace D3PD{
    
TileCellRawAssociationTool::TileCellRawAssociationTool(const string& type, 
        const string& name, const IInterface* parent) :
  Base(type,name,parent),
  m_rawGetter(type,name,parent),
  m_tilehwid(0),
  m_nRawReturned(0)
{
    declareProperty("TileRawChannelSGKey",m_tileRawKey = "TileRawChannelFlt");
    
}

TileCellRawAssociationTool::~TileCellRawAssociationTool() {
}

StatusCode TileCellRawAssociationTool::initialize()
{
  CHECK( m_rawGetter.initialize() );
  CHECK( m_rawGetter.setProperty("SGKey",m_tileRawKey) );
  CHECK( detStore()->retrieve(m_tilehwid) );
  return StatusCode::SUCCESS;
}

StatusCode TileCellRawAssociationTool::reset(const CaloCell& p){
    m_nRawReturned=0;

    CHECK( m_rawGetter.reset(false) );
    
    const CaloCell* cell=&p;
    const TileCell* tilecell = dynamic_cast<const TileCell*> (cell);
    if(!tilecell){
      MsgStream log(msgSvc(), name());
        log << MSG::ERROR
                << "Passed CaloCell is not a TileCell" << endmsg;
        return StatusCode::FAILURE;
    }
    const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
    long gain1 = tilecell->gain1();
    long gain2 = tilecell->gain2();
    IdentifierHash hash1 = caloDDE->onl1();
    IdentifierHash hash2 = caloDDE->onl2();
    if (hash1 != TileHWID::NOT_VALID_HASH) {
        m_adcId1 = m_tilehwid->adc_id(hash1,gain1);
    }
    else{ 
      MsgStream log(msgSvc(), name());
        log<<MSG::DEBUG<<"TileCellRawAssociationTool: Invalid hash on gain1"<<
                endmsg;
        m_adcId1.clear();
    }
    
    if (hash2 != TileHWID::NOT_VALID_HASH) {
        m_adcId2 = m_tilehwid->adc_id(hash2,gain2);
    }
    else{ 
      MsgStream log(msgSvc(), name());
        log<<MSG::DEBUG<<"TileCellRawAssociationTool: Invalid hash on gain2"<<
                endmsg;
        m_adcId2.clear();
    }
    
    return StatusCode::SUCCESS;
}

const TileRawChannel* TileCellRawAssociationTool::next(){
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in TileCellRawAssociationTool::next()" << 
            endmsg;
    if(m_nRawReturned>1)return 0;
    
    const TileRawChannel* tileraw=
        (const TileRawChannel*) m_rawGetter.nextUntyped();
    while(tileraw){
        HWIdentifier rId=tileraw->adc_HWID();
        if(m_adcId1.is_valid()){
            if(m_adcId1==rId){
                ++m_nRawReturned;
                m_adcId1.clear();
                return tileraw;
            }
        }
        if(m_adcId2.is_valid()){
            if(m_adcId2==rId){
                ++m_nRawReturned;
                m_adcId2.clear();
                return tileraw;
            }
        }
        tileraw=(const TileRawChannel*) m_rawGetter.nextUntyped();
    }    
    
    log<<MSG::DEBUG<<"TileCellRawAssociationTool: Found only "<<
            m_nRawReturned<<" associated raw channels."<<endmsg;
    return 0;    
}

}
