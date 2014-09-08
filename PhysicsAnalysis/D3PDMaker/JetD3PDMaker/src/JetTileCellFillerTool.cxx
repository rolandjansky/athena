/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetTileCellFillerTool.cxx 587210 2014-03-11 21:14:27Z ssnyder $
/**
 * @file JetD3PDMaker/src/JetTileCellFillerTool.cxx
 * @author Stephen Cole < stephen.cole@cern.ch >
 * @date 2 June 2011
 * @brief Block filler tool for Jet Associated Calorimeter cells
 */

#include "JetTileCellFillerTool.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetConstituentIterator.h"
//#include "JetUtils/JetCaloHelper.h"

#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"

#include <iostream>

#include "Navigation/NavigationToken.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h" 

#include "TileEvent/TileCell.h"
#include "TileEvent/TileContainer.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetTileCellFillerTool::JetTileCellFillerTool(const std::string& type,
        const std::string& name,
        const IInterface* parent):
        BlockFillerTool<Jet> (type, name, parent),
        m_tileBadChanTool("TileBadChanTool")
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode JetTileCellFillerTool::initialize()
{
  StoreGateSvc* detStore = 0;
  CHECK( service("DetectorStore",detStore) );
  CHECK( detStore->retrieve(m_tileid) );
  CHECK( detStore->retrieve(m_tilehwid) );
  CHECK( m_tileBadChanTool.retrieve() );
  m_cabling = TileCablingService::getInstance();

  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode JetTileCellFillerTool::book(){    
    CHECK(addVariable("tile_X",m_xCells));
    CHECK(addVariable("tile_Y",m_yCells));
    CHECK(addVariable("tile_Z",m_zCells));
    CHECK(addVariable("tile_Eta",m_etaCells));
    CHECK(addVariable("tile_Phi",m_phiCells));
   
    CHECK(addVariable("tile_Time",m_timeCells));
    CHECK(addVariable("tile_Timediff",m_tdiffCells));
    CHECK(addVariable("tile_E",m_eCells));
    CHECK(addVariable("tile_Ediff",m_ediffCells));
    CHECK(addVariable("tile_Gain1",m_gain1Cells));
    CHECK(addVariable("tile_Gain2",m_gain2Cells));
    CHECK(addVariable("tile_Module",m_moduleCells));
    CHECK(addVariable("tile_Partition",m_partitionCells));
    CHECK(addVariable("tile_Tower",m_towerCells));
    CHECK(addVariable("tile_Sample",m_sampleCells));
    CHECK(addVariable("tile_Chan1",m_chan1Cells));
    CHECK(addVariable("tile_Chan2",m_chan2Cells));
    CHECK(addVariable("tile_PMT1",m_pmt1Cells));
    CHECK(addVariable("tile_PMT2",m_pmt2Cells));
    CHECK(addVariable("tile_Status1",m_status1Cells));
    CHECK(addVariable("tile_Status2",m_status2Cells));
    CHECK(addVariable("tile_Qual1",m_qual1Cells));
    CHECK(addVariable("tile_Qual2",m_qual2Cells));
    CHECK(addVariable("tile_Qbits1",m_qbit1Cells));
    CHECK(addVariable("tile_Qbits2",m_qbit2Cells));
    
    CHECK(addVariable("nontile_jetTime",m_jetTime));
    CHECK(addVariable("nontile_clusEnergy",m_clusEnergy));

    return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */

StatusCode JetTileCellFillerTool::fill(const Jet& p){   
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in JetTileCellFillerTool::fill()" << endmsg; 
    NavigationToken<CaloCell,double> cellToken;
    NavigationToken<CaloCluster,double> ClusToken;
    p.fillToken(cellToken,double(1.0));
    p.fillToken(ClusToken,double(1.0));
    
    
    typedef NavigationToken<CaloCluster,double>::const_iterator Clus_cItr;
    typedef CaloCluster::cell_iterator Cell_Itr;
    Clus_cItr ClusEnd=ClusToken.end();
    float ClEnergy=0, ClEnergy2=0,tClEnergy2=0;
    for(Clus_cItr it=ClusToken.begin();it!=ClusEnd;++it){
        const CaloCluster *cluster=*it;
        Cell_Itr cBegin=cluster->cell_begin();
        Cell_Itr cEnd=cluster->cell_end();
        for(Cell_Itr itr=cBegin;itr<cEnd;++itr){
            const CaloCell *cell=*itr;
            if(cell->caloDDE()->getSubCalo()==CaloCell_ID::TILE){
                const TileCell *tilecell =(TileCell*)cell;
                Identifier id  = tilecell->ID();
                m_xCells->push_back(tilecell->x());
                m_yCells->push_back(tilecell->y());
                m_zCells->push_back(tilecell->z());
                m_etaCells->push_back(tilecell->eta());
                m_phiCells->push_back(tilecell->phi());
                int partition(0);
                uint32_t bad1(0),bad2(0);
                long chan1(-1),chan2(-1),pmt1(-1),pmt2(-1);
                long gain1 = tilecell->gain1();
                long gain2 = tilecell->gain2();
                long qual1 = tilecell->qual1();
                long qual2 = tilecell->qual2();
                long qbits1 = tilecell->qbit1();
                long qbits2 = tilecell->qbit2();
                const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
                IdentifierHash hash1 = caloDDE->onl1();
                IdentifierHash hash2 = caloDDE->onl2();
                if (hash1 != TileHWID::NOT_VALID_HASH) {
                    HWIdentifier adc_id = m_tilehwid->adc_id(hash1,gain1);
                    partition = m_tilehwid->ros(adc_id);
                    chan1 = m_tilehwid->channel(adc_id);
                    pmt1  = m_cabling->channel2hole(partition,chan1);
                    bad1 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
                }
                if (hash2 != TileHWID::NOT_VALID_HASH) {
                    HWIdentifier adc_id = m_tilehwid->adc_id(hash2,gain2);
                    if (hash1 == TileHWID::NOT_VALID_HASH) 
                        partition = m_tilehwid->ros(adc_id);
                    chan2 = m_tilehwid->channel(adc_id);
                    pmt2  = m_cabling->channel2hole(partition,chan2);
                    bad2 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
                }
                m_moduleCells->push_back(m_tileid->module(id)+1); // Note the +1 !!!
                m_partitionCells->push_back(partition);
                m_towerCells->push_back(m_tileid->tower(id));
                m_sampleCells->push_back(m_tileid->sample(id));
                m_gain1Cells->push_back(gain1);
                m_gain2Cells->push_back(gain2);
                m_chan1Cells->push_back(chan1);
                m_chan2Cells->push_back(chan2);
                m_pmt1Cells->push_back(pmt1);
                m_pmt2Cells->push_back(pmt2);
                m_status1Cells->push_back(bad1);
                m_status2Cells->push_back(bad2);
                m_qual1Cells->push_back(qual1);
                m_qual2Cells->push_back(qual2);
                m_qbit1Cells->push_back(qbits1);
                m_qbit2Cells->push_back(qbits2);
                m_eCells->push_back(tilecell->energy());
                m_timeCells->push_back(tilecell->time());
                m_ediffCells->push_back(tilecell->eneDiff());
                m_tdiffCells->push_back(tilecell->timeDiff());
            }
            else{
                ClEnergy+=cell->energy();
                ClEnergy2+=cell->energy()*cell->energy();
                tClEnergy2+=cell->time()*cell->energy()*cell->energy();
            }
        }
    }
    *m_jetTime=tClEnergy2/ClEnergy2;
    *m_clusEnergy=ClEnergy;
    
//    *m_n_tilecells=ntiles;
    return StatusCode::SUCCESS;
}


} // namespace D3PD
