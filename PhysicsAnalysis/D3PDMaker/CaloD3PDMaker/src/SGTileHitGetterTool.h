/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   SGTileHitGetterTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on January 13, 2012, 1:11 PM
 */

#ifndef SGTILEHITGETTERTOOL_H
#define	SGTILEHITGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TileSimEvent/TileHitVector.h"


namespace D3PD {

    class SGTileHitGetterTool:
    public SGCollectionGetterTool<TileHitVector> {
    public:
        typedef D3PD::SGCollectionGetterTool<TileHitVector> Base;
        /**
        * @brief Standard Gaudi tool constructor.
        * @param type The name of the tool type.
        * @param name The tool name.
        * @param parent The tool's Gaudi parent.
        */
        SGTileHitGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
        virtual ~SGTileHitGetterTool();

        StatusCode initialize();
        size_t sizeHint(bool allowMissing=false);
        StatusCode reset(bool allowMissing=false);
        const void* nextUntyped();
        const std::type_info& typeinfo() const;
        const std::type_info& elementTypeinfo() const;

    private:
        TileHitVector::const_iterator m_evtItr,m_evtEnd;

    };


}

#endif	/* SGTILEHITGETTERTOOL_H */

