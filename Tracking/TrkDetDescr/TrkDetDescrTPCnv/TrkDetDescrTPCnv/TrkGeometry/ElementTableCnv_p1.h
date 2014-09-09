/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef ELEMENTTABLE_CNV_P1_H
#define ELEMENTTABLE_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/ElementTable.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTable_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"

class MsgStream;

class ElementTableCnv_p1 : public T_AthenaPoolTPCnvBase<Trk::ElementTable, Trk::ElementTable_p1>
{
    public:
        ElementTableCnv_p1() {}
        
        virtual void persToTrans( const Trk::ElementTable_p1 *persObj,
                                        Trk::ElementTable    *transObj,
                                        MsgStream            &log );
        virtual void transToPers( const Trk::ElementTable    *transObj,
                                        Trk::ElementTable_p1 *persObj,
                                        MsgStream            &log );
        
        MaterialCnv_p1 m_materialCnv;
                                        

};

#endif // ELEMENTTABLE_CNV_P1_H