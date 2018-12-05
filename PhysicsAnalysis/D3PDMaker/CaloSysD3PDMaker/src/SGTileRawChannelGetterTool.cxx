/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileRawChannelGetterTool.cxx
 * Author: Stephen Cole <scole@niu.edu>
 * 
 * Created on March 1, 2011, 9:28 AM
 */

#include "SGTileRawChannelGetterTool.h"
#include "TileEvent/TileRawChannel.h"

namespace D3PD{
    SGTileRawChannelGetterTool::SGTileRawChannelGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent):
                           Base(type,name,parent) {
    }


    SGTileRawChannelGetterTool::~SGTileRawChannelGetterTool() {
    }

    StatusCode SGTileRawChannelGetterTool::initialize(){
        CHECK(Base::initialize());
        return StatusCode::SUCCESS;
    }

    const std::type_info& SGTileRawChannelGetterTool::typeinfo() const {
        return typeid(TileRawChannelContainer);
    }

    const std::type_info& SGTileRawChannelGetterTool::elementTypeinfo() const {
        return typeid(TileRawChannel);
    }

    size_t SGTileRawChannelGetterTool::sizeHint(bool allowMissing){
        const TileRawChannelContainer* tdc=get(allowMissing);
        if(!tdc)return 0;
        const TileRawChannelCollection* col=*(tdc->begin());
        size_t rval=0;
        size_t digSize=sizeof(TileRawChannel);
        TileRawChannelCollection::const_iterator it;
        for(it=col->begin();it<col->end();++it)++rval;
        return rval*digSize;
    }

    StatusCode SGTileRawChannelGetterTool::reset(bool allowMissing){
        const TileRawChannelContainer* tdc=get(allowMissing);
        if(!tdc){
            m_evtItr=m_evtEnd;
            m_digItr=m_digEnd;
            return StatusCode::FAILURE;
        }
        m_evtItr=tdc->begin();
        m_evtEnd=tdc->end();
        if(m_evtItr==m_evtEnd){
            m_digItr=m_digEnd;
            return StatusCode::SUCCESS;
        }
        const TileRawChannelCollection* col=*m_evtItr;
        m_digItr=col->begin();
        m_digEnd=col->end();

        return StatusCode::SUCCESS;
    }

    const void* SGTileRawChannelGetterTool::nextUntyped(){
        const TileRawChannel* tileR=0;
        const TileRawChannelCollection* col=0;
        do{
            if(m_evtItr==m_evtEnd)return 0;
            if(m_digItr==m_digEnd){
                tileR=0;
                ++m_evtItr;
                if(m_evtItr!=m_evtEnd){
                    col=*m_evtItr;
                    m_digItr=col->begin();
                    m_digEnd=col->end();
                }
            }
            else {
                tileR=*m_digItr;
                ++m_digItr;
            }
        }while(!tileR);

    //    if(m_evtItr==m_evtEnd)return 0;
    //    col=*m_evtItr;
    //    ++m_evtItr;

        return tileR;
    }

}
