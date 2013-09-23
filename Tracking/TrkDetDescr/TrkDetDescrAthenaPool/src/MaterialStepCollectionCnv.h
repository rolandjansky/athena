/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCollectionCnv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALSTEPCOLLECTION_CNV_H
#define MATERIALSTEPCOLLECTION_CNV_H

// ATLAS persistency way:
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// the transient side
#include "TrkGeometry/MaterialStepCollection.h"
// the perisistent side
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCollectionCnv_p1.h"

// useful typedefs to not change the code only once in case of schema evolution
typedef Trk::MaterialStepCollection_p1    MaterialStepCollection_PERS;
typedef T_AthenaPoolCustomCnv<Trk::MaterialStepCollection, MaterialStepCollection_PERS> MaterialStepCollectionCnvBase;


class MaterialStepCollectionCnv : public MaterialStepCollectionCnvBase {
 
 friend class CnvFactory<MaterialStepCollectionCnv>;
 
 protected:
 
  MaterialStepCollectionCnv( ISvcLocator *svcloc );
  ~MaterialStepCollectionCnv();
  
  virtual StatusCode initialize();
 
  virtual MaterialStepCollection_PERS* createPersistent( Trk::MaterialStepCollection *transCont);
  virtual Trk::MaterialStepCollection* createTransient();
 
 private:
  void                  updateLog();    //!< This method modifies m_log to indicate the current key being converted
  IMessageSvc*          m_msgSvc;      //!< MsgStream svc
  MsgStream             m_log;         //!< MsgStream

  MaterialStepCollectionCnv_p1        m_TPConverter;

};//end of class definitions

#endif // MATERIALSTEPCOLLECTION_CNV_H
