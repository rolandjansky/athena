/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellDetailsFillerTool.cxx
 * Author: Stephen Cole <scole@niu.edu>
 * 
 * Created on January 28, 2011, 3:12 PM
 */

#include "TileCellDetailsFillerTool.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileCell.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD{


    TileCellDetailsFillerTool::TileCellDetailsFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent):
      BlockFillerTool<CaloCell>(type,name,parent),
      m_tileid (0),
      m_tilehwid(0),
      m_cabling(0),
      m_tileBadChanTool("TileBadChanTool"),
      m_run2plus(false),
      m_notRealE1run2{{}}
    {
        declareProperty("SaveCellDetails",    m_saveCellDetails = true);
        declareProperty("SavePositionInfo",   m_savePosition = true );
        declareProperty("TileBadChanTool",    m_tileBadChanTool);

        TileCellDetailsFillerTool::book().ignore(); // Avoid coverity warnings
    }


     /**
      * @ brief Standard Gaudi initialize method.
      */
    StatusCode TileCellDetailsFillerTool::initialize()
    {
      StoreGateSvc* detStore = 0;
      CHECK( service("DetectorStore",detStore) );
      CHECK( detStore->retrieve(m_tileid) );
      CHECK( detStore->retrieve(m_tilehwid) );
      CHECK( m_tileBadChanTool.retrieve() );
      m_cabling = TileCablingService::getInstance();
      m_run2plus = m_cabling->isRun2PlusCabling();

      if (m_run2plus) {
        for (int ros = 3; ros < 5; ++ros) {
          for (int drawer = 0; drawer < 64; ++drawer) {
            int drawer2 = m_cabling->E1_merged_with_run2plus(ros, drawer);
            m_notRealE1run2[ros - 3][drawer2] = (drawer2 != 0);
          }
        }
      }

      return StatusCode::SUCCESS;
    }


    StatusCode TileCellDetailsFillerTool::book(){
        if  (m_savePosition) {
            CHECK(addVariable("X",m_xCells));
            CHECK(addVariable("Y",m_yCells));
            CHECK(addVariable("Z",m_zCells));
            CHECK(addVariable("PMT1",m_pmt1Cells));
            CHECK(addVariable("PMT2",m_pmt2Cells));
        }

        if (m_saveCellDetails){
            CHECK(addVariable("Time",m_timeCells));
            CHECK(addVariable("Timediff",m_tdiffCells));
            CHECK(addVariable("Ediff",m_ediffCells));
            CHECK(addVariable("Gain1",m_gain1Cells));
            CHECK(addVariable("Gain2",m_gain2Cells));
            CHECK(addVariable("Module",m_moduleCells));
            CHECK(addVariable("Partition",m_partitionCells));
            CHECK(addVariable("Tower",m_towerCells));
            CHECK(addVariable("Sample",m_sampleCells));
            CHECK(addVariable("Chan1",m_chan1Cells));
            CHECK(addVariable("Chan2",m_chan2Cells));
            CHECK(addVariable("Status1",m_status1Cells));
            CHECK(addVariable("Status2",m_status2Cells));
            CHECK(addVariable("Qual1",m_qual1Cells));
            CHECK(addVariable("Qual2",m_qual2Cells));
            CHECK(addVariable("QBits1",m_qbits1));
            CHECK(addVariable("QBits2",m_qbits2));
        }


        return StatusCode::SUCCESS;
    }

    StatusCode TileCellDetailsFillerTool::fill(const CaloCell& p){
        MsgStream log( msgSvc(), name() );
        log << MSG::DEBUG << " in TileCellDetailsFillerTool::fill()" << endmsg;
        const CaloCell* cell=&p;
        const TileCell* tilecell = dynamic_cast<const TileCell*> (cell);
        if(tilecell){
            Identifier id  = tilecell->ID();
            if ( log.level() < MSG::DEBUG ) log << MSG::VERBOSE << " cell_id " << m_tileid->to_string(id,-2) << endmsg;
            int partition(0);
            uint32_t bad1(0),bad2(0);
            long chan1(-1),chan2(-1),pmt1(-1),pmt2(-1);
            long gain1 = tilecell->gain1();
            long gain2 = tilecell->gain2();
            long qual1 = tilecell->qual1();
            long qual2 = tilecell->qual2();
            int module = m_tileid->module(id);

            const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
            IdentifierHash hash1 = (gain1<0) ? TileHWID::NOT_VALID_HASH : caloDDE->onl1();
            IdentifierHash hash2 = (gain2<0) ? TileHWID::NOT_VALID_HASH : caloDDE->onl2();
            if (m_savePosition) {
                *m_xCells=tilecell->x();
                *m_yCells=tilecell->y();
                *m_zCells=tilecell->z();
                *m_pmt1Cells=pmt1;
                *m_pmt2Cells=pmt2;
            }
            if (m_saveCellDetails) {
                if (hash1 != TileHWID::NOT_VALID_HASH) {
                    HWIdentifier adc_id = m_tilehwid->adc_id(hash1,gain1);
                    if ( log.level() < MSG::DEBUG ) log << MSG::VERBOSE << " adc_id1 " << m_tilehwid->to_string(adc_id) << " hash " << hash1 << " " << gain1 << endmsg;
                    partition = m_tilehwid->ros(adc_id);
                    chan1 = m_tilehwid->channel(adc_id);
                    if (m_run2plus && partition > 2 && chan1 == E1_CHANNEL && m_notRealE1run2[partition - 3][module]) chan1 = -E1_CHANNEL;
                    pmt1  = m_cabling->channel2hole(partition,chan1);
                    bad1 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
                }
                if (hash2 != TileHWID::NOT_VALID_HASH) {
                    HWIdentifier adc_id = m_tilehwid->adc_id(hash2,gain2);
                    if ( log.level() < MSG::DEBUG ) log << MSG::VERBOSE << " adc_id2 " << m_tilehwid->to_string(adc_id) << " hash " << hash2 << " " << gain2 << endmsg;
                    if (hash1 == TileHWID::NOT_VALID_HASH) partition = m_tilehwid->ros(adc_id);
                    chan2 = m_tilehwid->channel(adc_id);
                    pmt2  = m_cabling->channel2hole(partition,chan2);
                    bad2 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
                }



                *m_moduleCells=module+1; // Note the +1 !!!
                *m_partitionCells=partition;
                *m_towerCells=m_tileid->tower(id);
                *m_sampleCells=m_tileid->sample(id);
                *m_qbits1=tilecell->qbit1();
                *m_qbits2=tilecell->qbit2();
                if(gain1>=0 && ((*m_qbits1) & TileCell::MASK_BADCH)!=0 && 
                        qual1!=qual2 && (qual1!=255 || qual2!=255))
                    gain1= !gain1;
                if(gain2>=0 && ((*m_qbits2) & TileCell::MASK_BADCH)!=0 && 
                        qual1!=qual2 && (qual1!=255 || qual2!=255))
                    gain2= !gain2;
                *m_gain1Cells=gain1;
                *m_gain2Cells=gain2;
                *m_chan1Cells=chan1;
                *m_chan2Cells=chan2;
                if(tilecell->badch1())
                    *m_status1Cells=bad1+10;
                else *m_status1Cells=bad1;
                if(tilecell->badch2())
                    *m_status2Cells=bad2+10;
                else *m_status2Cells=bad2;
                *m_qual1Cells=qual1;
                *m_qual2Cells=qual2;
                *m_timeCells=tilecell->time();
                *m_ediffCells=tilecell->eneDiff();
                *m_tdiffCells=tilecell->timeDiff();
            }
            if ( log.level() < MSG::DEBUG )DumpCellInfo(); // Dump cell info only if in VERBOSE mode
        }
        else log << MSG::WARNING << " Cell of id "<< cell->ID()
                <<" : TileHelper is_tile, but TileCell pointer is 0 "
                << endmsg;


        return StatusCode::SUCCESS;
    }

    void TileCellDetailsFillerTool::DumpCellInfo(){
        MsgStream log( msgSvc(), name() );
        log << MSG::INFO << "Dumping Cell info:" << endmsg;
        if (m_savePosition) {
            log << MSG::INFO << "   X         :" <<*m_xCells << endmsg;
            log << MSG::INFO << "   Y         :" <<*m_yCells << endmsg;
            log << MSG::INFO << "   Z         :" <<*m_zCells << endmsg;
            log << MSG::INFO << "   PMT1      :" <<*m_pmt1Cells << endmsg;
            log << MSG::INFO << "   PMT2      :" <<*m_pmt2Cells << endmsg;
        }
        if (m_saveCellDetails) {
            log << MSG::INFO << "   Time      :" <<*m_timeCells << endmsg;
            log << MSG::INFO << "   Ediff     :" <<*m_ediffCells << endmsg;
            log << MSG::INFO << "   Tdiff     :" <<*m_tdiffCells << endmsg;
            log << MSG::INFO << "   Module    :" <<*m_moduleCells << endmsg;
            log << MSG::INFO << "   Partition :" <<*m_partitionCells << endmsg;
            log << MSG::INFO << "   Tower     :" <<*m_towerCells << endmsg;
            log << MSG::INFO << "   Sample    :" <<*m_sampleCells << endmsg;
            log << MSG::INFO << "   Gain1     :" <<*m_gain1Cells << endmsg;
            log << MSG::INFO << "   Gain2     :" <<*m_gain2Cells << endmsg;
            log << MSG::INFO << "   Chan1     :" <<*m_chan1Cells << endmsg;
            log << MSG::INFO << "   Chan2     :" <<*m_chan2Cells << endmsg;
            log << MSG::INFO << "   Status1   :" <<*m_status1Cells << endmsg;
            log << MSG::INFO << "   Status2   :" <<*m_status2Cells << endmsg;
            log << MSG::INFO << "   QF1       :" <<*m_qual1Cells << endmsg;
            log << MSG::INFO << "   QF2       :" <<*m_qual2Cells << endmsg;
        }
    }

}

