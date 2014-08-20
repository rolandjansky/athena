/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ELELEMENTTABLE_CNV_H
#define ELELEMENTTABLE_CNV_H

// ATLAS persistency way:
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// the transient side
#include "TrkGeometry/ElementTable.h"
// the perisistent side
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"

// useful typedefs to not change the code only once in case of schema evolution
typedef Trk::ElementTable_p1    ElementTable_PERS;
typedef T_AthenaPoolCustomCnv<Trk::ElementTable, ElementTable_PERS> ElementTableCnvBase;


class ElementTableCnv : public ElementTableCnvBase {
 
 friend class CnvFactory<ElementTableCnv>;
 
 protected:
 
  ElementTableCnv( ISvcLocator *svcloc );
  ~ElementTableCnv();
  
  virtual StatusCode initialize();
 
  virtual ElementTable_PERS* createPersistent( Trk::ElementTable *transCont);
  virtual Trk::ElementTable* createTransient();
 
 private:
  void                      updateLog();    //!< This method modifies m_log to indicate the current key being converted
  IMessageSvc*              m_msgSvc;      //!< MsgStream svc
  MsgStream                 m_log;         //!< MsgStream

  ElementTableCnv_p1        m_TPConverter;

};//end of class definitions

#endif // ELELEMENTTABLE_CNV_H
