/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DIGITIZATION_ISCTFASTDIGITIZATIONTOOL_H
#define SCT_DIGITIZATION_ISCTFASTDIGITIZATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

static const InterfaceID IID_ISCT_FastDigitizationTool ("ISCT_FastDigitizationTool",1,0);

class ISCT_FastDigitizationTool : virtual public IAlgTool
{

   public:

     static const InterfaceID& interfaceID();

     ///alternative interface which uses the PileUpMergeSvc to obtain all
     ///the required SubEvents.
     virtual StatusCode processAllSubEvents() = 0;

};

inline const InterfaceID& ISCT_FastDigitizationTool::interfaceID()
{
  return IID_ISCT_FastDigitizationTool;
}

#endif // ISCTFASTDIGITIZATIONTOOL_H
