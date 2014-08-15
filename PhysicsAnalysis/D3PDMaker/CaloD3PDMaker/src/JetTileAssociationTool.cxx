/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   JetTileAssociationTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on November 1, 2011, 9:51 PM
 */

#include "JetTileAssociationTool.h"
#include "TileEvent/TileCell.h"
#include "GaudiKernel/MsgStream.h"

using namespace std;

namespace D3PD{

JetTileAssociationTool::JetTileAssociationTool(const string& type, 
        const string& name, const IInterface* parent) : Base(type,name,parent) {
}

JetTileAssociationTool::~JetTileAssociationTool() {
}

StatusCode JetTileAssociationTool::reset(const Jet& p){
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in JetTileAssociationTool::reset()" << 
            endmsg;
//    NavigationToken<CaloCell,double> cellToken;
    m_cellToken.clear();
    p.fillToken(m_cellToken,double(1.0));
    
    m_itr=m_cellToken.begin();
    m_cEnd=m_cellToken.end();
    
    return StatusCode::SUCCESS;
}

const CaloCell* JetTileAssociationTool::next(){
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in JetTileAssociationTool::next()" << 
            endmsg;
    while(m_itr!=m_cEnd){
        const CaloCell* cell=*m_itr;
        if(!cell){
            log << MSG::ERROR << "JetTileAssociationTool::next():: Null " << 
                    "passed as address of CaloCell constituent of a jet"<<
                    endmsg;
            ++m_itr;
            continue;
        }
        const CaloDetDescrElement* dde=cell->caloDDE();
        const TileCell* tilecell=0;
        if(!dde) 
            tilecell=dynamic_cast<const TileCell*>(cell);
        else if(dde->getSubCalo()==CaloCell_ID::TILE)
            tilecell=(const TileCell*)cell;
        
        if(tilecell){
            ++m_itr;
            return cell;
        }
        ++m_itr;
    }
    return 0;
}

}
