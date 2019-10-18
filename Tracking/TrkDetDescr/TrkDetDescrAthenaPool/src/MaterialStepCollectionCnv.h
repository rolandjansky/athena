/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCollectionCnv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALSTEPCOLLECTION_CNV_H
#define MATERIALSTEPCOLLECTION_CNV_H

// ATLAS persistency way:
#include "GaudiKernel/MsgStream.h"


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// the transient side
#include "TrkGeometry/MaterialStepCollection.h"
// the perisistent side
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCollectionCnv_p1.h"

// useful typedefs to not change the code only once in case of schema evolution
typedef Trk::MaterialStepCollection_p1    MaterialStepCollection_PERS;
typedef T_AthenaPoolCustomCnvWithKey<Trk::MaterialStepCollection, MaterialStepCollection_PERS> MaterialStepCollectionCnvBase;


class MaterialStepCollectionCnv : public MaterialStepCollectionCnvBase {
 
 friend class CnvFactory<MaterialStepCollectionCnv>;
 
 protected:
 
public:
  MaterialStepCollectionCnv( ISvcLocator *svcloc );


protected:
  virtual MaterialStepCollection_PERS*
  createPersistentWithKey ( Trk::MaterialStepCollection *transCont,
                            const std::string& key) override;

  virtual Trk::MaterialStepCollection*
  createTransientWithKey (const std::string& key) override;

  
 private:
  IMessageSvc*          m_msgSvc;      //!< MsgStream svc
  MaterialStepCollectionCnv_p1        m_TPConverter;

};//end of class definitions

#endif // MATERIALSTEPCOLLECTION_CNV_H
