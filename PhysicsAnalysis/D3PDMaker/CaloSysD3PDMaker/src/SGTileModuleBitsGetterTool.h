/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   SGTileModuleBitsGetterTool.h
 * Author: stephen
 *
 * Created on March 15, 2012, 2:52 PM
 */

#ifndef SGTILEMODULEBITSGETTERTOOL_H
#define	SGTILEMODULEBITSGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "SGTileRawChannelGetterTool.h" //requires the template specialization

namespace D3PD{

    class SGTileModuleBitsGetterTool:
    public SGCollectionGetterTool<TileRawChannelContainer> {
    public:
        typedef D3PD::SGCollectionGetterTool<TileRawChannelContainer> Base;
        /**
        * @brief Standard Gaudi tool constructor.
        * @param type The name of the tool type.
        * @param name The tool name.
        * @param parent The tool's Gaudi parent.
        */
        SGTileModuleBitsGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
        virtual ~SGTileModuleBitsGetterTool();

        StatusCode initialize();
        size_t sizeHint(bool allowMissing=false);
        StatusCode reset(bool allowMissing=false);
        const void* nextUntyped();
        const std::type_info& typeinfo() const;
        const std::type_info& elementTypeinfo() const;

    private:
        TileRawChannelContainer::const_iterator m_evtItr,m_evtEnd;
    };

}

#endif	/* SGTILEMODULEBITSGETTERTOOL_H */

