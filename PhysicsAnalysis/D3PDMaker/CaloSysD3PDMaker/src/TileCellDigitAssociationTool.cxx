/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellDigitAssociationTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on October 31, 2011, 11:23 AM
 */

#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileDigits.h"
#include "TileEvent/TileCell.h"

#include "TileCellDigitAssociationTool.h"

using namespace std;

namespace D3PD{

TileCellDigitAssociationTool::TileCellDigitAssociationTool(const string& type, 
        const string& name, const IInterface* parent) :
  Base(type,name,parent),
  m_digitGetter(type,name,parent),
  m_tilehwid(0),
  m_nDigitReturned(0)
{
    declareProperty("TileDigitsSGKey",m_tileDigitKey = "TileDigitsFlt");
}


TileCellDigitAssociationTool::~TileCellDigitAssociationTool() {
}

StatusCode TileCellDigitAssociationTool::initialize()
{
  CHECK( m_digitGetter.initialize()) ;
  CHECK( m_digitGetter.setProperty("SGKey",m_tileDigitKey) );
  CHECK( detStore()->retrieve(m_tilehwid) );
  return StatusCode::SUCCESS;
}

StatusCode TileCellDigitAssociationTool::reset(const CaloCell& p){
    m_nDigitReturned=0;
    
    CHECK( m_digitGetter.reset(false) );
    
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
        log<<MSG::DEBUG<<"TileCellDigitAssociationTool: Invalid hash on gain1"<<
                endmsg;
        m_adcId1.clear();
    }
    
    if (hash2 != TileHWID::NOT_VALID_HASH) {
        m_adcId2 = m_tilehwid->adc_id(hash2,gain2);
    }
    else{ 
      MsgStream log(msgSvc(), name());
        log<<MSG::DEBUG<<"TileCellDigitAssociationTool: Invalid hash on gain2"<<
                endmsg;
        m_adcId2.clear();
    }
    
    return StatusCode::SUCCESS;
}

const TileDigits* TileCellDigitAssociationTool::next(){
    if(m_nDigitReturned>1)return 0;
    
    const TileDigits* tiledigit=
        (const TileDigits*) m_digitGetter.nextUntyped();
    while(tiledigit){
        HWIdentifier rId=tiledigit->adc_HWID();
        if(m_adcId1.is_valid()){
            if(m_adcId1==rId){
                ++m_nDigitReturned;
                m_adcId1.clear();
                return tiledigit;
            }
        }
        if(m_adcId2.is_valid()){
            if(m_adcId2==rId){
                ++m_nDigitReturned;
                m_adcId2.clear();
                return tiledigit;
            }
        }
        tiledigit=(const TileDigits*) m_digitGetter.nextUntyped();
    }    
    
    return 0;    
}


}
