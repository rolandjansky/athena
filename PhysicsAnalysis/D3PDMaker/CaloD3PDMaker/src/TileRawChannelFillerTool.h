/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileRawChannelFillerTool.h
 * Author: Stephen Cole <scole@niu.edu>
 *
 * Created on March 1, 2011, 9:27 AM
 */

#ifndef TILERAWCHANNELFILLERTOOL_H
#define	TILERAWCHANNELFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileRawChannel.h"
#include "GaudiKernel/ToolHandle.h"
#include "TileConditions/ITileBadChanTool.h"

class TileHWID;
class TileCablingService;

namespace D3PD{

    class TileRawChannelFillerTool: public BlockFillerTool<TileRawChannel> {
    public:
        typedef D3PD::BlockFillerTool<TileRawChannel> Base;
        /**
         * @brief Standard Gaudi tool constructor.
         * @param type The name of the tool type.
         * @param name The tool name.
         * @param parent The tool's Gaudi parent.
         */
        TileRawChannelFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

        /// Standard Gaudi initialize method.
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
        virtual StatusCode fill (const TileRawChannel& p);
    private:
        //amplitude/time/quality and optionally ped as floats.
        const TileHWID* m_tilehwid;

        bool m_SaveHWid;
        bool m_SaveRawCh;

        char* m_ros;
        char* m_drawer;
        char* m_channel;
        char* m_gain;

        float* m_ped;
        std::vector<float>* m_amplitude;
        std::vector<float>* m_time;
        std::vector<float>* m_quality;

    };

}
#endif	/* TILERAWCHANNELFILLERTOOL_H */

