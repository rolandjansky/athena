/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_PAUDIPHOTONCONTAINER_H
#define PHOTONANALYSISUTILS_PAUDIPHOTONCONTAINER_H

#include "PhotonAnalysisUtils/PAUDiPhoton.h"
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"


namespace PAU {
  
  class DiPhotonContainer : public DataVector<PAU::DiPhoton> {
  public:

    DiPhotonContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<PAU::DiPhoton>(own) {};

    virtual ~DiPhotonContainer() {};

    inline void push_back(PAU::DiPhoton* dp){
      DataVector<PAU::DiPhoton>::push_back(dp);
    }
    inline void push_back(const PAU::DiPhoton* dp){
      DataVector<PAU::DiPhoton>::push_back(const_cast<PAU::DiPhoton*>(dp));
    }
  };

}

CLASS_DEF( PAU::DiPhotonContainer , 1100176984 , 1 )
SG_BASE(PAU::DiPhotonContainer, DataVector<PAU::DiPhoton> );

#endif // PHOTONANALYSISUTILS_PAUDIPHOTONCONTAINER_H
