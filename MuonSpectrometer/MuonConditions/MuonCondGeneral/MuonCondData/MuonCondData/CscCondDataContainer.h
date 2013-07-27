/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCONDDATACONTAINER_H 
#define CSCCONDDATACONTAINER_H
/**
        @class CscCondDataContainer

        @author lampen@physics.arizona.edu

        @brief Contains complete set of all data from cool data base.

        Holds conditions constants from the cool database. Designed for internal use by CscCoolStrSvc.
**/

#include "DataModel/DataVector.h"
//#include "CLIDSvc/CLASS_DEF.h"
#include "MuonCondData/CscCondDataCollectionBase.h"

namespace MuonCalib {
    class CscCondDataContainer : public DataVector< CscCondDataCollectionBase > 
  {
    public:
      CscCondDataContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
        : DataVector<CscCondDataCollectionBase>(own) {}
      virtual ~CscCondDataContainer() {};
  };

}

//CLASS_DEF(MuonCalib::CscCondDataContainer, 1173850174, 1)

#endif
