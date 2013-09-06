///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv_p1.h 
// Header file for class MuonContainerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNV_P1_H 
#define RECTPCNV_MUONCONTAINERCNV_P1_H 

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// muonEvent includes
#include "muonEvent/MuonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p1.h"
#include "RecTPCnv/MuonCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< 
            Analysis::MuonContainer, 
            MuonContainer_p1, 
            MuonCnv_p1 
       > MuonContainerCnvBase_p1;

class MuonContainerCnv_p1 : public MuonContainerCnvBase_p1
{
  public:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    MuonContainerCnv_p1(){}
    virtual void  persToTrans(const MuonContainer_p1* persColl, Analysis::MuonContainer* transColl, MsgStream &log) ;
    void setKey ( const std::string key) { m_cnv.setKey( key ); }
    void setEventStore( const StoreGateSvc_t storeGate ) { m_cnv.setEventStore( storeGate ); }  
  private:
    MuonCnv_p1 m_cnv;
};

inline void  MuonContainerCnv_p1::persToTrans(const MuonContainer_p1* persColl, Analysis::MuonContainer* transColl, MsgStream &log)
{
   MuonContainer_p1::const_iterator it;
   transColl->clear();
   transColl->reserve(persColl->size());
   for (it = persColl->begin(); it != persColl->end(); ++it) {
      transColl->push_back(m_cnv.createTransient(&(*it), log));
   }
}

#endif //> RECTPCNV_MUONCONTAINERCNV_P1_H
