/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef NSW_TRIGRAWDATACONTAINER_H
#define NSW_TRIGRAWDATACONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "MuonRDO/NSW_TrigRawData.h"

namespace Muon {
  class NSW_TrigRawDataContainer : public DataVector<Muon::NSW_TrigRawData> {

 public:
  NSW_TrigRawDataContainer();

  ~NSW_TrigRawDataContainer() { };

 private:


};
}

CLASS_DEF(Muon::NSW_TrigRawDataContainer,1172141273,1)

#endif   /// NSW_TRIGRAWDATACONTAINER_H





