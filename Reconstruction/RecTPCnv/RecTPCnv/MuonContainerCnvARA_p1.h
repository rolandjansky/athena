///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnvARA_p1.h 
// Header file for class MuonContainerCnvARA_p1
// Version of the converter that works with ARA.
// Author: snyder@bnl.gov
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNVARA_P1_H 
#define RECTPCNV_MUONCONTAINERCNVARA_P1_H 

// STL includes
#include <string>

// muonEvent includes
#include "muonEvent/MuonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p1.h"
#include "RecTPCnv/MuonCnvARA_p1.h"

typedef T_AthenaPoolTPCnvVector< 
            Analysis::MuonContainer, 
            MuonContainer_p1, 
            MuonCnvARA_p1 
       > MuonContainerCnvBase_p1;

class MuonContainerCnvARA_p1 : public MuonContainerCnvBase_p1
{
  public:
    virtual void  persToTrans(const MuonContainer_p1* persColl, Analysis::MuonContainer* transColl, MsgStream &log) ;
  private:
    MuonCnvARA_p1 m_cnv;
};


inline void  MuonContainerCnvARA_p1::persToTrans(const MuonContainer_p1* persColl, Analysis::MuonContainer* transColl, MsgStream &log)
{
   MuonContainer_p1::const_iterator it;
   transColl->clear();
   transColl->reserve(persColl->size());
   for (it = persColl->begin(); it != persColl->end(); ++it) {
      transColl->push_back(m_cnv.createTransient(&(*it), log));
   }
}

#endif //> RECTPCNV_MUONCONTAINERCNV_P1_H
