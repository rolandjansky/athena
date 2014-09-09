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
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialMap_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"

class MsgStream;

class LayerMaterialMapCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::LayerMaterialMap, Trk::LayerMaterialMap_p1 >
{
  public:
    
    /** Standard constructor */
    LayerMaterialMapCnv_p1(){}

    /** Destructor constructor */
    ~LayerMaterialMapCnv_p1(){}
    
    /** persistent to transient */
    virtual void persToTrans(const Trk::LayerMaterialMap_p1* persMap, Trk::LayerMaterialMap* transMap, MsgStream &mlog) {
       transMap->clear();
       
       // update the element table
       Trk::ElementTable* eTable = new Trk::ElementTable();
       m_eTableCnv.persToTrans((&persMap->elementTable), eTable, mlog);
       Trk::SharedObject<const Trk::ElementTable> soeTable(eTable);
       transMap->updateElementTable(soeTable);
       
       // convert vector entries one by one
       std::vector<TPObjRef>::const_iterator it   = persMap->lmVector.begin();
       std::vector<TPObjRef>::const_iterator iEnd = persMap->lmVector.end();
       for (size_t iidx=0 ; it != iEnd;  ++it, ++iidx ) {
           int layIdex = persMap->idxVector[iidx];
           Trk::LayerMaterialProperties* tProperties = createTransFromPStore( (ITPConverterFor<Trk::LayerMaterialProperties>**)0, *it, mlog );
           tProperties->updateElementTable(soeTable);
           (*transMap)[Trk::LayerIndex(layIdex)] = tProperties;
       }
     }
         
     /** transient to persistent */
     virtual void transToPers(const Trk::LayerMaterialMap* transMap, Trk::LayerMaterialMap_p1* persMap, MsgStream &mlog) {
       persMap->lmVector.clear();
       persMap->lmVector.reserve( transMap->size() );
       persMap->idxVector.clear();
       persMap->idxVector.reserve( transMap->size() );
       // write out the ElementTable
       if (transMap->elementTable())   
           m_eTableCnv.transToPers((transMap->elementTable()), &persMap->elementTable,mlog);
       // convert vector entries one by one
       for(auto& it : (*transMap) ) {
           persMap->idxVector.push_back( it.first.value() );
           persMap->lmVector.push_back( toPersistent( (ITPConverterFor<Trk::LayerMaterialProperties>**)0, it.second, mlog )  );
       }
     }
     
 protected:
   ElementTableCnv_p1     m_eTableCnv;

};

#endif // MATERIAKSTEPCOLLECTION_CNV_P1_TRK_H
