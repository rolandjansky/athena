/*
  Copyright (C) 2021-2020 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef RpcBis78_TRIGRAWDATACONTAINER_H
#define RpcBis78_TRIGRAWDATACONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "MuonRDO/RpcBis78_TrigRawData.h"

/*
  This class is for BIS78 trigger simulation.
  BIS78 trigger goes to TGC sector logic.
  This is different from other BIS chambers of phase 2, in this case the
  trigger is in the barrel logic. A different class may be needed.
*/

namespace Muon {
  class RpcBis78_TrigRawDataContainer : public DataVector<Muon::RpcBis78_TrigRawData> {

 public:
  RpcBis78_TrigRawDataContainer()=default;
  ~RpcBis78_TrigRawDataContainer() = default;

 private:


};
}

CLASS_DEF(Muon::RpcBis78_TrigRawDataContainer,1340556290,1)

#endif   /// RpcBis78_TRIGRAWDATACONTAINER_H





