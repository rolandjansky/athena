/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellDetailsFillerTool.h
 * Author: Stephen Cole <scole@niu.edu>
 *
 * Created on January 28, 2011, 3:12 PM
 */

#ifndef TILECELLDETAILSFILLERTOOL_H
#define	TILECELLDETAILSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileCell.h"
#include "GaudiKernel/ToolHandle.h"
#include "TileConditions/ITileBadChanTool.h"

class TileID;
class TileHWID;
class TileCablingService;

namespace D3PD {

    class TileCellDetailsFillerTool : public BlockFillerTool<CaloCell> {
    public:
        /*
         @brief Standard Gaudi tool constructor.
         @param type The name of the tool type.
         @param name The tool name.
         @param parent The tool's Gaudi parent.*/
        TileCellDetailsFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);


        /// Standard Gaudi initialize method.
        virtual StatusCode initialize();

        //Book variables for this block.
        virtual StatusCode book();

        /*
         @brief Fill one block --- type-safe version.
         @param p The input object.

         This is called once per object.  The caller
         is responsible for arranging that all the pointers for booked variables
         are set appropriately upon entry.*/
        virtual StatusCode fill (const CaloCell& p);

        // Method to dump all cell information to screen
        void DumpCellInfo();


    private:

        const TileID* m_tileid;
        const TileHWID* m_tilehwid;
        const TileCablingService* m_cabling;

        ToolHandle<ITileBadChanTool> m_tileBadChanTool; //Tile bad channel tool

        bool m_saveCellDetails; //switching on/off cell details storage
        bool m_savePosition; // switching on/off cell position storage

        bool m_run2;

        // variables to be in ntuple.
        
           //----***Removed, redundant with EventCommonD3PDMaker.FourMomFillerTool***----
           /// - Cell Energy in MeV
           /// - Eta position
           /// - Phi position
           //----******----
        
           /// - Number of Tile cells
           /// - Cell time in ns
           /// - X position
           /// - Y position
           /// - Z position
           /// - Difference of PMTs measurements:
           /// - PMTs Energy difference       : (E1-E2).
           /// - PMTs Time difference         :  as (T1-T2)/2
           /// - Module number    1-64        : 1-64 module number by phi
           /// - Partition Number 1-4         : 1=LBA; 2=LBC; 3=EBA; 4= EBC
           /// - tower(eta)       0-15        : tower number by pseudorapidity with 0.1 increment in eta
           /// - sample           0-15        : 0 = A, 1 = B = BC = C, 2 = D, 3 = special gap scin cells E1-E4; >3 indiv. tile used in Cs calib
           /// - gain             0-1         : low-high gain; -1 PMT not available
           /// - chan             0-47        : channel number
           /// - PMT              1-48        : PMT number
           /// - Status           32-bit word : Bad channel word (0=good, bits correspond to specific hw or calib problems )
           /// - Quality Factor               : The unsigned quality from TileRawChannel
           /// - Quality Bits                 : Bit mask containing channel quality info. See TileCell.h
        
        float* m_timeCells;
        float* m_xCells;
        float* m_yCells;
        float* m_zCells;
        float* m_tdiffCells;
        float* m_ediffCells;
        uint16_t* m_moduleCells;
        uint16_t* m_partitionCells;
        uint16_t* m_towerCells;
        uint16_t* m_sampleCells;
        short* m_gain1Cells;
        short* m_gain2Cells;
        short* m_chan1Cells;
        short* m_chan2Cells;
        short* m_pmt1Cells;
        short* m_pmt2Cells;
        uint16_t* m_status1Cells;
        uint16_t* m_status2Cells;
        uint16_t* m_qual1Cells;
        uint16_t* m_qual2Cells;
        uint8_t* m_qbits1;
        uint8_t* m_qbits2;

        bool m_notRealE1run2[2][64];
        enum CELL_CHANNEL {E1_CHANNEL = 12};

    };

}
#endif	/* TILECELLDETAILSFILLERTOOL_H */

