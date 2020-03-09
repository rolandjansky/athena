/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ELELEMENTTABLE_CNV_H
#define ELELEMENTTABLE_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

// the transient side
#include "TrkGeometry/ElementTable.h"
// the perisistent side
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"

// useful typedefs to not change the code only once in case of schema evolution
typedef Trk::ElementTable_p1    ElementTable_PERS;
typedef T_AthenaPoolCustomCnvWithKey<Trk::ElementTable, ElementTable_PERS> ElementTableCnvBase;


class ElementTableCnv : public ElementTableCnvBase {
 
 friend class CnvFactory<ElementTableCnv>;
 
 protected:
 
public:
  ElementTableCnv( ISvcLocator *svcloc );
protected:
  virtual ElementTable_PERS* createPersistentWithKey( Trk::ElementTable *transCont, const std::string& key) override;
  virtual Trk::ElementTable* createTransientWithKey (const std::string& key) override;
 

private:
  IMessageSvc*              m_msgSvc;      //!< MsgStream svc
  ElementTableCnv_p1        m_TPConverter;
};//end of class definitions


#endif // ELELEMENTTABLE_CNV_H
