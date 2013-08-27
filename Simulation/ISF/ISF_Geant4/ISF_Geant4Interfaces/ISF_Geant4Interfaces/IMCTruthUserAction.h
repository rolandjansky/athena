/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_Geant4Interfaces_IMCTruthUserAction_H
#define ISF_Geant4Interfaces_IMCTruthUserAction_H

#include "GaudiKernel/IAlgTool.h"

namespace iGeant4 {

  static const InterfaceID IID_IMCTruthUserAction("IMCTruthUserAction",1,0);

  class IMCTruthUserAction : virtual public IAlgTool {
  public:
    IMCTruthUserAction() {}
    static const InterfaceID& interfaceID();
    virtual ~IMCTruthUserAction(){}

  };

  inline const InterfaceID& IMCTruthUserAction::interfaceID()  
  { return IID_IMCTruthUserAction; }

}

#endif // ISF_Geant4Interfaces_IMCTruthUserAction_H
