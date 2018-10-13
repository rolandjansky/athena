/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef NSW_TRIGRAWDATA_H
#define NSW_TRIGRAWDATA_H

#include "DataModel/DataVector.h"
#include "SGTools/Class_DEF.h"

#include "MuonRDO/NSW_TrigRawDataSegment.h"

namespace Muon {
class NSW_TrigRawData : public DataVector<Muon::NSW_TrigRawDataSegment>
{

 public:
  NSW_TrigRawData();

  ~NSW_TrigRawData() { };

 private:


};
}

CLASS_DEF(Muon::NSW_TrigRawData,140250087,1)

#endif




