/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCollectionCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIAKSTEPCOLLECTION_CNV_P1_TRK_H
#define MATERIAKSTEPCOLLECTION_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCollection_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"


class MsgStream;

class MaterialStepCollectionCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::MaterialStepCollection, Trk::MaterialStepCollection_p1 >
{
  public:
    
    /** Standard constructor */
    MaterialStepCollectionCnv_p1() 
    {}
    
    /** persistent to transient */
    virtual void persToTrans(const Trk::MaterialStepCollection_p1* persVect, Trk::MaterialStepCollection* transVect, MsgStream &log) {
       transVect->clear();
       transVect->reserve( persVect->size() );
       // convert vector entries one by one
       Trk::MaterialStepCollection_p1::const_iterator it   = persVect->begin();
       Trk::MaterialStepCollection_p1::const_iterator iEnd = persVect->end();
       for ( ; it != iEnd;  ++it ) {
           const Trk::MaterialStep_p1* ms = &(*it);
           transVect->push_back( m_materialStepCnv.createTransient(ms, log ) );
           }
     }
         
     /** transient to persistent */
     virtual void transToPers(const Trk::MaterialStepCollection* transVect, Trk::MaterialStepCollection_p1* persVect, MsgStream &log) {
       persVect->clear();
       persVect->resize( transVect->size() );
       // convert vector entries one by one
       unsigned int i=0;
       Trk::MaterialStepCollection::const_iterator it = transVect->begin();
       Trk::MaterialStepCollection::const_iterator iEnd = transVect->end();
       for( ; it != iEnd;  ++it ) {
           m_materialStepCnv.transToPers(*it, &(*persVect)[i++], log ) ;
       }
     }

    private :
        MaterialStepCnv_p1  m_materialStepCnv;
};

#endif // MATERIAKSTEPCOLLECTION_CNV_P1_TRK_H