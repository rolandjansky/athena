/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ComputeStaveServices_H
#define ComputeStaveServices_H

#include "InDetServMatGeoModel/StaveServices.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class ComputeStaveServices {
public:

  ComputeStaveServices() {}

  StaveServices compute( DetType::Type, DetType::Part, int layerNumber, int nModulesPerStave, int nChipsPerModule,
                         MsgStream& msg) const;
  int computeLVGaugeSerial( DetType::Type, DetType::Part, int layerNumber, 
			    int nModules, double moduleCurrent, double moduleVoltage,
			    double poweringLoss, double lossInCable, double cableLen,
                            MsgStream& msg) const;
};

#endif
