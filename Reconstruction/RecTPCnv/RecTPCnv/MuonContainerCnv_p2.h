///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv_p2.h 
// Header file for class MuonContainerCnv_p2
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNV_P2_H 
#define RECTPCNV_MUONCONTAINERCNV_P2_H 

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
#include "RecTPCnv/MuonContainer_p2.h"
#include "RecTPCnv/MuonCnv_p2.h"

typedef T_AthenaPoolTPCnvVectorConst< 
            Analysis::MuonContainer, 
            MuonContainer_p2, 
            MuonCnv_p2 
       > MuonContainerCnvBase_p2;

class MuonContainerCnv_p2 : public MuonContainerCnvBase_p2
{
  public:
    using base_class::transToPers;
    using base_class::persToTrans;

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    MuonContainerCnv_p2() {}
    virtual void  persToTrans(const MuonContainer_p2* persColl, Analysis::MuonContainer* transColl, MsgStream &log) const;
    void setKey ( const std::string& key) { m_cnv.setKey( key ); }
    void setEventStore( const StoreGateSvc_t storeGate ) { m_cnv.setEventStore( storeGate ); }
  private:
    MuonCnv_p2 m_cnv;
};


inline void  MuonContainerCnv_p2::persToTrans(const MuonContainer_p2* persColl, Analysis::MuonContainer* transColl, MsgStream &log) const
{
   MuonContainer_p2::const_iterator it;
   transColl->clear();
   transColl->reserve(persColl->size());
   for (it = persColl->begin(); it != persColl->end(); ++it) {
      transColl->push_back(m_cnv.createTransientConst(&(*it), log));
   }
}

#endif //> RECTPCNV_MUONCONTAINERCNV_P2_H
