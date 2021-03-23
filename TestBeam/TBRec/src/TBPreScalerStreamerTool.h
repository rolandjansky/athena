/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBPRESCALERSTREAMERTOOL_H
#define TBREC_TBPRESCALERSTREAMERTOOL_H

#include "TBEventStreamerTool.h"

// This is needed for migration to CLHEP 1.9
// (Forward declarations of CLHEP classes will give rise to an error...)
// instead of ...
//class RanluxEngine;
 // use this:
#include "CLHEP/Random/RanluxEngine.h"

#include <string>
#include <map>
#include <vector>

class TBPreScalerStreamerTool : public TBEventStreamerTool
{
 public:

  TBPreScalerStreamerTool(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  virtual ~TBPreScalerStreamerTool();

  virtual StatusCode initializeTool() override;

  virtual StatusCode accept() override;

 protected:

  ////////////////
  // Properties //
  ////////////////

  double m_acceptFraction;

  /////////////////////
  // Internal Stores //
  /////////////////////

  CLHEP::RanluxEngine* m_randomizer;

  //////////////////////
  // Internal Helpers //
  //////////////////////

};
#endif
