/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   SGTileDigitsGetterTool.cxx
 * Author: Stephen Cole <scole@niu.edu>
 * 
 * Created on February 28, 2011, 10:36 AM
 */

#include "SGTileDigitsGetterTool.h"
#include "TileEvent/TileDigits.h"

namespace D3PD{

    SGTileDigitsGetterTool::SGTileDigitsGetterTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent):
                           Base(type,name,parent) {
    }

    SGTileDigitsGetterTool::~SGTileDigitsGetterTool() {
    }

    StatusCode SGTileDigitsGetterTool::initialize(){
        CHECK(Base::initialize());
        return StatusCode::SUCCESS;
    }

    const std::type_info& SGTileDigitsGetterTool::typeinfo() const {
        return typeid(TileDigitsContainer);
    }

    const std::type_info& SGTileDigitsGetterTool::elementTypeinfo() const {
        return typeid(TileDigits);
    }

    size_t SGTileDigitsGetterTool::sizeHint(bool allowMissing){
        const TileDigitsContainer* tdc=get(allowMissing);
        if(!tdc)return 0;
        const TileDigitsCollection* col=*(tdc->begin());
        size_t rval=0;
        size_t digSize=sizeof(TileDigits);
        TileDigitsCollection::const_iterator it;
        for(it=col->begin();it<col->end();++it)++rval;
        return rval*digSize;
    }

    StatusCode SGTileDigitsGetterTool::reset(bool allowMissing){
        const TileDigitsContainer* tdc=get(allowMissing);
        if(!tdc){
            m_evtItr=m_evtEnd;
            m_digItr=TileDigitsCollection::const_iterator();
            m_digEnd=TileDigitsCollection::const_iterator();
            return StatusCode::FAILURE;
        }
        m_evtItr=tdc->begin();
        m_evtEnd=tdc->end();
        if(m_evtItr==m_evtEnd){
            m_digItr=m_digEnd;
            return StatusCode::SUCCESS;
        }
        const TileDigitsCollection* col=*m_evtItr;
        m_digItr=col->begin();
        m_digEnd=col->end();

        return StatusCode::SUCCESS;
    }

    const void* SGTileDigitsGetterTool::nextUntyped(){
        const TileDigits* tileD=0;
        const TileDigitsCollection* col=0;
        do{
            if(m_evtItr==m_evtEnd)return 0;
            if(m_digItr==m_digEnd){
                tileD=0;
                ++m_evtItr;
                if(m_evtItr!=m_evtEnd){
                    col=*m_evtItr;
                    m_digItr=col->begin();
                    m_digEnd=col->end();
                }
            }
            else {
                tileD=*m_digItr;
                ++m_digItr;
            }
        }while(!tileD);

    //    if(m_evtItr==m_evtEnd)return 0;
    //    col=*m_evtItr;
    //    ++m_evtItr;

        return tileD;
    }

}
