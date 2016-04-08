/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIDIGITIZATION_IPIXELFASTDIGITIZATIONTOOL_H
#define FASTSIDIGITIZATION_IPIXELFASTDIGITIZATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

static const InterfaceID IID_IPixelFastDigitizationTool ("IPixelFastDigitizationTool",1,0);

class IPixelFastDigitizationTool : virtual public IAlgTool
{

   public:

     static const InterfaceID& interfaceID();

     ///alternative interface which uses the PileUpMergeSvc to obtain all
     ///the required SubEvents.
     virtual StatusCode processAllSubEvents() = 0;

};

inline const InterfaceID& IPixelFastDigitizationTool::interfaceID()
{
  return IID_IPixelFastDigitizationTool;
}

#endif // FASTSIDIGITIZATION_IPIXELFASTDIGITIZATIONTOOL_H
