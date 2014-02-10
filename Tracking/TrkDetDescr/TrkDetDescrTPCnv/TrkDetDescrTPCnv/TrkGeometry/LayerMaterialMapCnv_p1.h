/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMapCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALMAP_CNV_P1_TRK_H
#define LAYERMATERIALMAP_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialMap_p1.h"


class MsgStream;

class LayerMaterialMapCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::LayerMaterialMap, Trk::LayerMaterialMap_p1 >
{
  public:
    
    /** Standard constructor */
    LayerMaterialMapCnv_p1() 
    {}
    
    /** persistent to transient */
    virtual void persToTrans(const Trk::LayerMaterialMap_p1* persMap, Trk::LayerMaterialMap* transMap, MsgStream &mlog) {
       transMap->clear();
       std::cout << "[TrkDetDescrTPCnv READ ] Persistent LayerMaterialMap of size : " <<  persMap->lmVector.size() << std::endl;
       
       // convert vector entries one by one
       std::vector<TPObjRef>::const_iterator it   = persMap->lmVector.begin();
       std::vector<TPObjRef>::const_iterator iEnd = persMap->lmVector.end();
       for (size_t iidx=0 ; it != iEnd;  ++it, ++iidx ) {
           int layIdex = persMap->idxVector[iidx];
           Trk::LayerMaterialProperties* tProperties = dynamic_cast<Trk::LayerMaterialProperties*>( createTransFromPStore( (ITPConverterFor<Trk::LayerMaterialProperties>**)0, *it, mlog ) );
           (*transMap)[Trk::LayerIndex(layIdex)] = tProperties;
       }
       
       std::cout << "[TrkDetDescrTPCnv READ ] Transient LayerMaterialMap of size : " <<  transMap->size() << std::endl;
       
     }
         
     /** transient to persistent */
     virtual void transToPers(const Trk::LayerMaterialMap* transMap, Trk::LayerMaterialMap_p1* persMap, MsgStream &mlog) {
       std::cout << "[TrkDetDescrTPCnv WRITE ] Transient LayerMaterialMap of size : " <<  transMap->size() << std::endl;
       persMap->lmVector.clear();
       persMap->lmVector.reserve( transMap->size() );
       persMap->idxVector.clear();
       persMap->idxVector.reserve( transMap->size() );
       // convert vector entries one by one
       Trk::LayerMaterialMap::const_iterator it   = transMap->begin();
       Trk::LayerMaterialMap::const_iterator iEnd = transMap->end();
       for( ; it != iEnd;  ++it ) {
           persMap->idxVector.push_back( (*it).first.value() );
           persMap->lmVector.push_back( toPersistent( (ITPConverterFor<Trk::LayerMaterialProperties>**)0, (*it).second, mlog )  );
       }
       std::cout << "[TrkDetDescrTPCnv WRITE ] Persistent LayerMaterialMap of size : " <<  persMap->idxVector.size() << std::endl;
     }


};

#endif // MATERIAKSTEPCOLLECTION_CNV_P1_TRK_H
