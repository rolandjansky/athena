/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMISSINGET_ITRIGMISSINGETHYPOTOOL_H
#define TRIGMISSINGET_ITRIGMISSINGETHYPOTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


class ITrigMissingETHypoTool : virtual public::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigMissingETHypoTool, 1, 0);


  virtual ~ITrigMissingETHypoTool(){};
  virtual StatusCode decide(const xAOD::TrigMissingETContainer*, bool&) const = 0;
  virtual const HLT::Identifier& getId() const = 0; 


};
#endif
