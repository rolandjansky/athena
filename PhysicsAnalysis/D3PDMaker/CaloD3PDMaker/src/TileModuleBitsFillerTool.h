/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileModuleBitsFillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on March 15, 2012, 1:59 PM
 */

#ifndef TILEMODULEBITSFILLERTOOL_H
#define	TILEMODULEBITSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileRawChannelCollection.h"
#include "GaudiKernel/ToolHandle.h"
#include "TileConditions/ITileBadChanTool.h"


namespace D3PD{

    class TileModuleBitsFillerTool: public BlockFillerTool<TileRawChannelCollection> {
    public:
        typedef D3PD::BlockFillerTool<TileRawChannelCollection> Base;
        /**
         * @brief Standard Gaudi tool constructor.
         * @param type The name of the tool type.
         * @param name The tool name.
         * @param parent The tool's Gaudi parent.
         */
        TileModuleBitsFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);
        
        // Standard Gaudi initialize method.
        virtual StatusCode initialize();
        
        //Book variables for this block.
        virtual StatusCode book();

        /**
         * @brief Fill one block --- type-safe version.
         * @param p The input object.
         * 
         * This is called once per object.  The caller
         * is responsible for arranging that all the pointers for booked variables
         * are set appropriately upon entry.
         */
        virtual StatusCode fill (const TileRawChannelCollection& p);
    private:
                
        short* m_ID;
        short* m_FragGlobalCRC;
        short* m_FragDSPBCID;
        short* m_FragBCID;
        short* m_FragMemoryPar;
        short* m_FragSstrobe;
        short* m_FragDstrobe;
        short* m_FragHeaderBit;
        short* m_FragHeaderPar;
        short* m_FragSampleBit;
        short* m_FragSamplePar;
        short* m_FragFEChipMask;
        short* m_FragRODChipMask;

    };

}

#endif	/* TILEMODULEBITSFILLERTOOL_H */

