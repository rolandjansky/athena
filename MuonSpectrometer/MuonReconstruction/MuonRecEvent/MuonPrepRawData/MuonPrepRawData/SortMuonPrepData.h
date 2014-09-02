/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTMUONPREPDATA_H
#define SORTMUONPREPDATA_H

#include "MuonPrepRawData/MdtPrepData.h"

namespace Muon {

  class SortMuonPrepData {
    public:
  
      bool operator()( const Trk::PrepRawData* prd1, const Trk::PrepRawData* prd2 ) const {

        if( prd1->identify() == prd2->identify() ) {
          const MdtPrepData* mdt1 = dynamic_cast<const MdtPrepData*>(prd1);
          if( mdt1 ) {
            const MdtPrepData* mdt2 = static_cast<const MdtPrepData*>(prd2);
            return mdt1->tdc() < mdt2->tdc();
          }
        }

        return prd1->identify() < prd2->identify();

      }
  };

}

#endif
