/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAINTERFACES_iEGAMMAMVACALIBTOOL_H
#define EGAMMAINTERFACES_iEGAMMAMVACALIBTOOL_H

#include "GaudiKernel/IAlgTool.h"

/**
 * @class IegammaMVACalibTool
 * @brief A tool used by the egammaMVASvc to help manage the MVAs.
 **/
class IegammaMVACalibTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IegammaMVACalibTool, 1, 0);

  virtual ~IegammaMVACalibTool() override {};
}; 

#endif
