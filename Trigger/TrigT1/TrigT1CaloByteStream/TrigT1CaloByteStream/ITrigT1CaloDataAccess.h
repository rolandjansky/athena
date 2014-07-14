/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ITRIGT1CALODATAACCESS_H
#define TRIGT1CALOBYTESTREAM_ITRIGT1CALODATAACCESS_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"

#include "DataModel/DataVector.h"

namespace LVL1 {
  class TriggerTower;
  class JEMEtSums;
  class JetElement;
  class JetElementKey;
}

namespace LVL1BS {

static const InterfaceID IID_ITrigT1CaloDataAccess("LVL1BS::ITrigT1CaloDataAccess", 1, 0);

class ITrigT1CaloDataAccess : virtual public IAlgTool {

 public:
   static const InterfaceID& interfaceID();

   virtual StatusCode loadCollection(
                      DataVector<LVL1::TriggerTower>::const_iterator& beg,
                      DataVector<LVL1::TriggerTower>::const_iterator& end,
		      double etaMin, double etaMax,
		      double phiMin, double phiMax, const bool full) = 0;
   virtual StatusCode loadCollection(
                      DataVector<LVL1::JetElement>::const_iterator& beg,
                      DataVector<LVL1::JetElement>::const_iterator& end)=0;

      
};

inline const InterfaceID& ITrigT1CaloDataAccess::interfaceID()
{ 
  return IID_ITrigT1CaloDataAccess;
}

} // end of namespace

#endif 
