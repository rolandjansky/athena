/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_ILARPILEUPTOOL_H
#define LARDIGITIZATION_ILARPILEUPTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "PileUpTools/IPileUpTool.h"


class ILArPileUpTool : virtual public IAlgTool, virtual public IPileUpTool
{
//
// >>>>>>>> public method
//
 public:

  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ILArPileUpTool("ILArPileUpTool", 1 , 0);
    return IID_ILArPileUpTool;
  }

  virtual StatusCode fillMapFromHit(StoreGateSvc* seStore,float tbunch,bool isSignal)=0;

};

#endif
