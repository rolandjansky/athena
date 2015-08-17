/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_ITRTFASTDIGITIZATIONTOOL_H
#define TRT_DIGITIZATION_ITRTFASTDIGITIZATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

static const InterfaceID IID_ITRTFastDigitizationTool ("ITRTFastDigitizationTool",1,0);

class ITRTFastDigitizationTool : virtual public IAlgTool
{

   public:

     static const InterfaceID& interfaceID();

     ///alternative interface which uses the PileUpMergeSvc to obtain all
     ///the required SubEvents.
     virtual StatusCode processAllSubEvents() = 0;

};

inline const InterfaceID& ITRTFastDigitizationTool::interfaceID()
{
  return IID_ITRTFastDigitizationTool;
}

#endif // ITRTFASTDIGITIZATIONTOOL_H
