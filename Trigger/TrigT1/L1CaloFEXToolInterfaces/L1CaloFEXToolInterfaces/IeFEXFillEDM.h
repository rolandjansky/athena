/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           IeFEXFillEDM.h  -
//                          -------------------
//     begin                : 02 05 2021
//     email                :  nicholas.andrew.luongo@cern.ch
//***************************************************************************/

#ifndef IeFEXFillEDM_H
#define IeFEXFillEDM_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigger/eFexEMRoI.h"
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexTauRoI.h"
#include "xAODTrigger/eFexTauRoIContainer.h"

namespace LVL1 {
  
/*
Interface definition for eFEXFillEDM
*/

  static const InterfaceID IID_IeFEXFillEDM("LVL1::IeFEXFillEDM", 1, 0);

  class IeFEXFillEDM : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    virtual StatusCode execute() = 0;
    
    virtual void fillEmEDM(std::unique_ptr<xAOD::eFexEMRoIContainer> &container, uint8_t eFEXNumber, uint32_t tobWord) = 0;
    virtual void fillTauEDM(std::unique_ptr<xAOD::eFexTauRoIContainer> &container, uint8_t eFEXNumber, uint32_t tobWord) = 0;

  private:

  };

  inline const InterfaceID& LVL1::IeFEXFillEDM::interfaceID()
  {
    return IID_IeFEXFillEDM;
  }

} // end of namespace

#endif
