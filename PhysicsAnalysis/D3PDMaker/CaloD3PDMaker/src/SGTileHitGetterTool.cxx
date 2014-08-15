/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   SGTileHitGetterTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on January 13, 2012, 1:11 PM
 */

#include "SGTileHitGetterTool.h"
#include "TileSimEvent/TileHit.h"

namespace D3PD{

    SGTileHitGetterTool::SGTileHitGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent):
                           Base(type,name,parent) {
    }

    SGTileHitGetterTool::~SGTileHitGetterTool() {
    }

    StatusCode SGTileHitGetterTool::initialize(){
        CHECK(Base::initialize());
        return StatusCode::SUCCESS;
    }

    const std::type_info& SGTileHitGetterTool::typeinfo() const {
        return typeid(TileHitVector);
    }

    const std::type_info& SGTileHitGetterTool::elementTypeinfo() const {
        return typeid(TileHit);
    }

    size_t SGTileHitGetterTool::sizeHint(bool allowMissing){
        const TileHitVector* thv=get(allowMissing);
        if(!thv)return 0;
        size_t rval=0;
        size_t digSize=sizeof(TileHit);
        TileHitVector::const_iterator it;
        for(it=thv->begin();it<thv->end();++it)++rval;
        return rval*digSize;
    }

    StatusCode SGTileHitGetterTool::reset(bool allowMissing){
        const TileHitVector* thv=get(allowMissing);
        if(!thv){
            m_evtItr=m_evtEnd;
            return StatusCode::FAILURE;
        }
        m_evtItr=thv->begin();
        m_evtEnd=thv->end();
        if(m_evtItr==m_evtEnd) return StatusCode::SUCCESS;
        
        return StatusCode::SUCCESS;
    }

    const void* SGTileHitGetterTool::nextUntyped(){
        const TileHit* tileD=0;
        do{
            if(m_evtItr==m_evtEnd)return 0;
            tileD=&(*m_evtItr);
            ++m_evtItr;
        }while(!tileD);

        return tileD;
    }

}
