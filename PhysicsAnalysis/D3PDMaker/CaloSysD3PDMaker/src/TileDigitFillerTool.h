/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileDigitFillerTool.h
 * Author: Stephen Cole <scole@niu.edu>
 *
 * Created on February 22, 2011, 4:06 PM
 */

#ifndef TILEDIGITFILLERTOOL_H
#define	TILEDIGITFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "GaudiKernel/ToolHandle.h"
#include "TileConditions/ITileBadChanTool.h"

class TileID;
class TileHWID;
class TileCablingService;

namespace D3PD {

    class TileDigitFillerTool : public BlockFillerTool<TileDigits> {
    public:
        /**
         * @brief Standard Gaudi tool constructor.
         * @param type The name of the tool type.
         * @param name The tool name.
         * @param parent The tool's Gaudi parent.
         */
        TileDigitFillerTool(const std::string& type,
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
        virtual StatusCode fill (const TileDigits& p);
    private:
        const TileID* m_tileid;
        const TileHWID* m_tilehwid;

        bool m_SaveHWid;
        bool m_SaveOffId;

        char* m_section;
        char* m_side;
        char* m_tower;

        char *m_ros;
        char *m_drawer;
        char *m_channel;
        char *m_gain;
        std::vector<short> *m_samples;

    };

}
#endif	/* TILEDIGITFILLERTOOL_H */

