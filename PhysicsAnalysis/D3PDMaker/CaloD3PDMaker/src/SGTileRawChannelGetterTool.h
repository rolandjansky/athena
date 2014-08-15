/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileRawChannelGetterTool.h
 * Author: Stephen Cole <scole@niu.edu>
 *
 * Created on March 1, 2011, 9:28 AM
 */

#ifndef SGTILERAWCHANNELGETTERTOOL_H
#define	SGTILERAWCHANNELGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TileEvent/TileRawChannelContainer.h"

namespace D3PD{

    class SGTileRawChannelGetterTool:
    public SGCollectionGetterTool<TileRawChannelContainer> {
    public:
        typedef D3PD::SGCollectionGetterTool<TileRawChannelContainer> Base;
        /**
        * @brief Standard Gaudi tool constructor.
        * @param type The name of the tool type.
        * @param name The tool name.
        * @param parent The tool's Gaudi parent.
        */
        SGTileRawChannelGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
        virtual ~SGTileRawChannelGetterTool();

        StatusCode initialize();
        size_t sizeHint(bool allowMissing=false);
        StatusCode reset(bool allowMissing=false);
        const void* nextUntyped();
        const std::type_info& typeinfo() const;
        const std::type_info& elementTypeinfo() const;

    private:
        TileRawChannelContainer::const_iterator m_evtItr,m_evtEnd;
        TileRawChannelCollection::const_iterator m_digItr,m_digEnd;
    };

// Template specialization for rel 16.5.0
//    template <>
//    struct Deref<TileRawChannelCollection*>{
//        typedef DataLinkVector<TileRawChannelCollection>::DataHolder DH;
//        typedef TileRawChannelCollection type;
//        static const type* deref (DH* p) { return p->getDataPtr(); }
//    };
//------------------------------------

// Template specialization for rel 16.6.2
    template <>
    struct Deref<TileRawChannelContainer,TileRawChannelCollection*>{
        typedef TileRawChannelContainer::const_iterator it;
        typedef TileRawChannelCollection type;
        static const type* deref (it& p) { return p->getDataPtr(); }
    };
//-------------------------------------

}
#endif	/* SGTILERAWCHANNELGETTERTOOL_H */

