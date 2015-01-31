///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This is a copy from
// $Id: IMuonSmearingTool.h 299883 2014-03-28 17:34:16Z krasznaa $

#ifndef  METINTERFACE_IMETSYSTEMATICSTOOL_H
#define  METINTERFACE_IMETSYSTEMATICSTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"

// xAOD includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETAssociationMap.h"

class IMETSystematicsTool : public virtual asg::IAsgTool,
                            public virtual CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IMETSystematicsTool )

  public:
  virtual ~IMETSystematicsTool() {}

  //we don't inherit from CorrectionTool directly, but we are something close to that
  virtual CP::CorrectionCode applyCorrection(xAOD::MissingET& met, xAOD::MissingETAssociationMap * map) const = 0;
  virtual CP::CorrectionCode correctedCopy(const xAOD::MissingET& met, xAOD::MissingET*& outputmet,xAOD::MissingETAssociationMap * map) const = 0;
  //We don't want these for MET
  //virtual CP::CorrectionCode applyContainerCorrection(xAOD::MissingETContainer& inputs) const;
  //virtual CP::CorrectionCode applyContainerCorrection(xAOD::MissingETContainer& inputs, const xAOD::EventInfo& eInfo) const;

 
  virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet & set) = 0;
  virtual CP::SystematicSet affectingSystematics() const = 0;
  virtual CP::SystematicSet recommendedSystematics() const = 0;
  virtual bool isAffectedBySystematic(const CP::SystematicVariation & systematic) const = 0;

}; // class IMETSystematicsTool

#endif //  METINTERFACE_IMETSYSTEMATICSTOOL_H
