/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Muon DetDescrCnv package
 -----------------------------------------
***************************************************************************/

#ifndef INDETMGRDETDESCRCNV_STGC_IDDETDESCRCNV_H
#define INDETMGRDETDESCRCNV_STGC_IDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 **  This class is a converter for the sTGC_IdHelper an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class sTGC_IDDetDescrCnv: public DetDescrConverter {
  friend class CnvFactory<sTGC_IDDetDescrCnv>;

 public:
  virtual long int   repSvcType() const;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  // Storage type and class ID (used by CnvFactory)
  static long  storageType();
  static const CLID& classID();

 protected:
  sTGC_IDDetDescrCnv(ISvcLocator* svcloc);

};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETMGRDETDESCRCNV_STGC_IDDETDESCRCNV_H
