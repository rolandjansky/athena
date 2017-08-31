/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   SGTileModuleBitsGetterTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on March 15, 2012, 2:52 PM
 */

#include "SGTileModuleBitsGetterTool.h"

namespace D3PD{
    SGTileModuleBitsGetterTool::SGTileModuleBitsGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent):
                           Base(type,name,parent) {
    }


    SGTileModuleBitsGetterTool::~SGTileModuleBitsGetterTool() {
    }

    StatusCode SGTileModuleBitsGetterTool::initialize(){
        MsgStream log(msgSvc(), name());
        log << MSG::INFO << " in SGTileModuleBitsGetterTool::initialize()" << endmsg;
        CHECK(Base::initialize());
        return StatusCode::SUCCESS;
    }

    const std::type_info& SGTileModuleBitsGetterTool::typeinfo() const {
        return typeid(TileRawChannelContainer);
    }

    const std::type_info& SGTileModuleBitsGetterTool::elementTypeinfo() const {
        return typeid(TileRawChannelCollection);
    }

    size_t SGTileModuleBitsGetterTool::sizeHint(bool allowMissing){
        const TileRawChannelContainer* tdc=get(allowMissing);
        if(!tdc)return 0;
        const TileRawChannelCollection* col=*(tdc->begin());
        size_t rval=0;
        size_t digSize=sizeof(TileRawChannel);
        TileRawChannelCollection::const_iterator it;
        for(it=col->begin();it<col->end();++it)++rval;
        return rval*digSize;
    }

    StatusCode SGTileModuleBitsGetterTool::reset(bool allowMissing){
        const TileRawChannelContainer* tdc=get(allowMissing);
        if(!tdc){
            m_evtItr=m_evtEnd;
            return StatusCode::FAILURE;
        }
        m_evtItr=tdc->begin();
        m_evtEnd=tdc->end();
        if(m_evtItr==m_evtEnd){
            return StatusCode::SUCCESS;
        }

        return StatusCode::SUCCESS;
    }

    const void* SGTileModuleBitsGetterTool::nextUntyped(){
        const TileRawChannelCollection* col=0;
        do{
            if(m_evtItr==m_evtEnd)return 0;            
            col=*m_evtItr;
            ++m_evtItr;
        }while(!col);

        return col;
    }

}
