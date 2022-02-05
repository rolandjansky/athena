/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           IeFEXFormTOBs.h  -
//                              -------------------
//     begin                : 30 04 2021
//     email                :  nicholas.andrew.luongo@cern.ch
//  ***************************************************************************/

#ifndef IeFEXFormTOBs_H
#define IeFEXFormTOBs_H

#include "GaudiKernel/IAlgTool.h"

namespace LVL1 {

/*
Interface definition for eFEXFormTOBs
*/

  static const InterfaceID IID_IeFEXFormTOBs("LVL1::IeFEXFormTOBs", 1, 0);

  class IeFEXFormTOBs : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual uint32_t formTauTOBWord(int &, int &, int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &) = 0;

    virtual uint32_t formEmTOBWord(int &, int &, int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &) = 0;

  private:

  };

  inline const InterfaceID& LVL1::IeFEXFormTOBs::interfaceID()
  {
    return IID_IeFEXFormTOBs;
  }

} // end of namespace

#endif

