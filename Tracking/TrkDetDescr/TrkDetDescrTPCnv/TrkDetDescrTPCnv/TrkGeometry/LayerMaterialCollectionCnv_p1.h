/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialCollectionCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALCOLLECTION_CNV_P1_TRK_H
#define LAYERMATERIALCOLLECTION_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/LayerMaterialCollection.h"
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialCollection_p1.h"


class MsgStream;

class LayerMaterialCollectionCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::LayerMaterialCollection, Trk::LayerMaterialCollection_p1 >
{
  public:
    
    /** Standard constructor */
    LayerMaterialCollectionCnv_p1() 
    {}
    
    /** persistent to transient */
    virtual void persToTrans(const Trk::LayerMaterialCollection_p1* persVect, Trk::LayerMaterialCollection* transVect, MsgStream &mlog) {
       transVect->clear();
       transVect->reserve( persVect->size() );
       // convert vector entries one by one
       Trk::LayerMaterialCollection_p1::const_iterator it   = persVect->begin();
       Trk::LayerMaterialCollection_p1::const_iterator iEnd = persVect->end();
       for ( ; it != iEnd;  ++it ) {
           transVect->push_back( dynamic_cast<Trk::LayerMaterialProperties*>( createTransFromPStore( (ITPConverterFor<Trk::LayerMaterialProperties>**)0, *it, mlog ) ) );
           }
     }
         
     /** transient to persistent */
     virtual void transToPers(const Trk::LayerMaterialCollection* transVect, Trk::LayerMaterialCollection_p1* persVect, MsgStream &mlog) {
       persVect->clear();
       persVect->resize( transVect->size() );
       // convert vector entries one by one
       Trk::LayerMaterialCollection::const_iterator it = transVect->begin();
       Trk::LayerMaterialCollection::const_iterator iEnd = transVect->end();
       for( ; it != iEnd;  ++it ) {
           persVect->push_back( toPersistent( (ITPConverterFor<Trk::LayerMaterialProperties>**)0, *it, mlog )  );
       }
     }


};

#endif // MATERIAKSTEPCOLLECTION_CNV_P1_TRK_H
