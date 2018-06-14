/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIDIGITIZATION_ISISMEAREDDIGITIZATIONTOOL_H
#define SIDIGITIZATION_ISISMEAREDDIGITIZATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

static const InterfaceID IID_ISiSmearedDigitizationTool ("ISiSmearedDigitizationTool",1,0);

class ISiSmearedDigitizationTool : virtual public IAlgTool
{

   public:

     static const InterfaceID& interfaceID();

     ///alternative interface which uses the PileUpMergeSvc to obtain all
     ///the required SubEvents.
     virtual StatusCode processAllSubEvents() = 0;

};

inline const InterfaceID& ISiSmearedDigitizationTool::interfaceID()
{
  return IID_ISiSmearedDigitizationTool;
}

#endif // SIDIGITIZATION_ISISMEAREDDIGITIZATIONTOOL_H
