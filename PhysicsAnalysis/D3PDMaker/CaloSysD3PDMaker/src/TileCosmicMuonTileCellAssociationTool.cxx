/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCosmicMuonTileCellAssociationTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on November 3, 2011, 3:48 PM
 */

#include "TileCosmicMuonTileCellAssociationTool.h"
#include "CaloIdentifier/TileID.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"

using namespace std;

namespace D3PD{

TileCosmicMuonTileCellAssociationTool::TileCosmicMuonTileCellAssociationTool(const string& type, 
        const string& name, const IInterface* parent)
  : Base(type,name,parent),
    m_tileid(0)
{
}

TileCosmicMuonTileCellAssociationTool::~TileCosmicMuonTileCellAssociationTool() {
}

StatusCode TileCosmicMuonTileCellAssociationTool::reset(const TileCosmicMuon& p)
{
    const CaloCellContainer* cellcoll = 0;

    CHECK( evtStore()->retrieve(cellcoll,"AllCalo") );
    CHECK( detStore()->retrieve(m_tileid) );
    
    m_itr=cellcoll->begin();
    m_cEnd=cellcoll->end();
    
    m_tileHash.clear();
    
    for(int i=0; i<p.GetTrackNCells();++i)
        m_tileHash.push_back(p.GetTrackCellHash(i));
    
    m_tileItr=m_tileHash.begin();
    
    return StatusCode::SUCCESS;
}

const CaloCell* TileCosmicMuonTileCellAssociationTool::next(){
    while(m_itr!=m_cEnd){
        const CaloCell* cell=*m_itr;
        if(cell->caloDDE()->getSubCalo()==CaloCell_ID::TILE){
            while(m_tileItr!=m_tileHash.end()){
                if(cell->ID()==m_tileid->cell_id(*m_tileItr)){
                    ++m_itr;
                    ++m_tileItr;
                    return cell;
                }
                ++m_tileItr;
            }
        }
        m_tileItr=m_tileHash.begin();
        ++m_itr;
    }
    return 0;    
}

}
