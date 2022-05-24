/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for ProcessedIOVCollCnv
//  author Marjorie Shapiro <mdshapiro@lbl.gov>
//  This class uses TP separation
///////////////////////////////////////////////////////////////////

#ifndef LUMIBLOCKCOLLECTIONCNV__H
#define LUMIBLOCKCOLLECTIONCNV__H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p2.h"

// the latest persistent representation type of LumiBlockCollection
typedef  LumiBlockCollection_p2  LumiBlockCollection_PERS;
typedef  T_AthenaPoolCustomCnv<LumiBlockCollection, LumiBlockCollection_PERS >  LumiBlockCollectionCnvBase;

class LumiBlockCollectionCnv : public LumiBlockCollectionCnvBase 
{
  friend class CnvFactory<LumiBlockCollectionCnv >;
 protected:
public:
  LumiBlockCollectionCnv (ISvcLocator* svcloc) : LumiBlockCollectionCnvBase(svcloc) {}
protected:
  virtual LumiBlockCollection_PERS*  createPersistent (LumiBlockCollection* transCont);
  virtual LumiBlockCollection*     createTransient ();

 private:
  LumiBlockCollectionCnv_p1   m_converter_p1;
  LumiBlockCollectionCnv_p2   m_converter;

};

#endif  // LUMIBLOCKCOLLECTIONCNV__H

