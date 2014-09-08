// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetTileCellFillerTool.h 541743 2013-03-20 17:46:41Z ssnyder $
/**
 * @file JetD3PDMaker/src/JetTileCellFillerTool.h
 * @author Stephen Cole < scole@cern.ch >
 * @date 2 June 2011
 * @brief Block filler tool for Jet Associated Calorimeter cells
 */

#ifndef JETD3PDMAKER_JetTileCellFillerTool_H
#define JETD3PDMAKER_JetTileCellFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TileConditions/ITileBadChanTool.h"
class Jet;
class TileID;
class TileHWID;
class TileCablingService;

namespace D3PD {


    /**
     * @brief Block filler tool for Jet EM Fraction.
     */
    class JetTileCellFillerTool : public BlockFillerTool<Jet>{
    public:
        /**
         * @brief Standard Gaudi tool constructor.
         * @param type The name of the tool type.
         * @param name The tool name.
         * @param parent The tool's Gaudi parent.
         */
        JetTileCellFillerTool (const std::string& type,
                const std::string& name,
                const IInterface* parent);


        /// Standard Gaudi initialize method.
        virtual StatusCode initialize();

        /// Book variables for this block.
        virtual StatusCode book();


        /**
         * @brief Fill one block --- type-safe version.
         * @param p The input object.
         *
         * This is called once per object.  The caller
         * is responsible for arranging that all the pointers for booked variables
         * are set appropriately upon entry.
         */
        virtual StatusCode fill (const Jet& p);
    
    private:
        float* m_jetTime;
        float* m_clusEnergy;
           
        
        const TileID* m_tileid;
        const TileHWID* m_tilehwid;
        const TileCablingService* m_cabling;

        ToolHandle<ITileBadChanTool> m_tileBadChanTool; //Tile bad channel tool

        // variables to be in ntuple.
        /// - Total event energy deposited in Tile
        /// - Number of Tile cells
        /// - Cell Energy in MeV
        /// - Cell time in ns
        /// - Eta position
        /// - Phi position
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
        /// - Quality Factor
        /// - Quality bits     See TileCell.h for bit descriptions
        
        std::vector<float>* m_eCells;
        std::vector<float>* m_timeCells;
        std::vector<float>* m_etaCells;
        std::vector<float>* m_phiCells;
        std::vector<float>* m_xCells;
        std::vector<float>* m_yCells;
        std::vector<float>* m_zCells;
        std::vector<float>* m_tdiffCells;
        std::vector<float>* m_ediffCells;
        std::vector<int>* m_moduleCells;
        std::vector<int>* m_partitionCells;
        std::vector<int>* m_towerCells;
        std::vector<int>* m_sampleCells;
        std::vector<int>* m_gain1Cells;
        std::vector<int>* m_gain2Cells;
        std::vector<int>* m_chan1Cells;
        std::vector<int>* m_chan2Cells;
        std::vector<int>* m_pmt1Cells;
        std::vector<int>* m_pmt2Cells;
        std::vector<uint32_t>* m_status1Cells;
        std::vector<uint32_t>* m_status2Cells;
        std::vector<int>* m_qual1Cells;
        std::vector<int>* m_qual2Cells;
        std::vector<int>* m_qbit1Cells;
        std::vector<int>* m_qbit2Cells;
        
        
        

    };


} // namespace D3PD


#endif /*JETD3PDMAKER_JetTileCellFillerTool_H*/
