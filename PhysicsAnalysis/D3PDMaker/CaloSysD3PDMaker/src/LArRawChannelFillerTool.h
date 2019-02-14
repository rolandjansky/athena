/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   LArRawChannelFillerTool.h
 * Author: Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 *
 * Created on May 30, 2012
 */

#ifndef LARRAWCHANNELFILLERTOOL_H
#define	LARRAWCHANNELFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

namespace D3PD{

    class LArRawChannelFillerTool: public BlockFillerTool<LArRawChannel> {
    public:
        typedef D3PD::BlockFillerTool<LArRawChannel> Base;
        /**
         * @brief Standard Gaudi tool constructor.
         * @param type The name of the tool type.
         * @param name The tool name.
         * @param parent The tool's Gaudi parent.
         */
        LArRawChannelFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);
        virtual ~LArRawChannelFillerTool();

        /// Standard Gaudi initialize method.
        StatusCode initialize();

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
        virtual StatusCode fill (const LArRawChannel& p);
    private:

        unsigned int* m_offId;
        unsigned int* m_onlId;
        float* m_energy;
        float* m_time;
        float* m_quality;
        SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
    };

}
#endif	/* TILERAWCHANNELFILLERTOOL_H */

