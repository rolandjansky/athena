/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONDIGITIZATIONTOOL_H 
#define IMUONDIGITIZATIONTOOL_H

#include "GaudiKernel/IAlgTool.h" 
#include "GaudiKernel/StatusCode.h"

static const InterfaceID IID_IMuonDigitizationTool ("IMuonDigitizationTool",1,0);

class IMuonDigitizationTool : virtual public IAlgTool
{

   public:

     static const InterfaceID& interfaceID();

     /**  @brief do the digitization
     */
     virtual StatusCode digitize() = 0;
};

inline const InterfaceID& IMuonDigitizationTool::interfaceID()
{
  return IID_IMuonDigitizationTool;
}

#endif // IMuonDIGITIZATIONTOOL_H


