/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   SGTileDigitsGetterTool.h
 * Author: Stephen Cole <scole@niu.edu>
 *
 * Created on February 28, 2011, 10:36 AM
 */

#ifndef SGTileDigitsGetterTool_H
#define	SGTileDigitsGetterTool_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TileEvent/TileDigitsContainer.h"


namespace D3PD {

    class SGTileDigitsGetterTool:
    public SGCollectionGetterTool<TileDigitsContainer> {
    public:
        typedef D3PD::SGCollectionGetterTool<TileDigitsContainer> Base;
        /**
        * @brief Standard Gaudi tool constructor.
        * @param type The name of the tool type.
        * @param name The tool name.
        * @param parent The tool's Gaudi parent.
        */
        SGTileDigitsGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
        virtual ~SGTileDigitsGetterTool();

        StatusCode initialize();
        size_t sizeHint(bool allowMissing=false);
        StatusCode reset(bool allowMissing=false);
        const void* nextUntyped();
        const std::type_info& typeinfo() const;
        const std::type_info& elementTypeinfo() const;

    private:
        TileDigitsContainer::const_iterator m_evtItr,m_evtEnd;
        TileDigitsCollection::const_iterator m_digItr,m_digEnd;

    };


}

#endif	/* SGTileDigitsGetterTool_H */

