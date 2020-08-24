/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HIPileupTool.h

#ifndef HIEVENTUTILS_IHIPILEUPTOOL_H
#define HIEVENTUTILS_IHIPILEUPTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

//R.Longo - 12/10/2019 - Replacing PATCore/TAccept (inherited from 21.0 HI-equalization)
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

namespace HI
{

  class IHIPileupTool : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE( HI::IHIPileupTool )

  public:

    virtual const asg::AcceptInfo& getAcceptInfo(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const = 0;
    //Added by R.Longo and S.Tapia on 14-10-2019 to bypass an error in including 21.0 missing commits
    virtual bool   is_pileup(const xAOD::HIEventShapeContainer&, const xAOD::ZdcModuleContainer& ) const = 0;

  };
}
#endif
