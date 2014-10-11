/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEMAMBIGUITYTOOL_H
#define IEMAMBIGUITYTOOL_H

#include "GaudiKernel/AlgTool.h"

// #include "xAODEgamma/EgammaFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

/**
   @class IEMAmbiguityTool
   Interface for tool egamma/egammaTools/EMAmbiguityTool
   @author Thomas Koffas
   @author Bruno Lenzi
*/

class egammaRec;

static const InterfaceID IID_IEMAmbiguityTool("IEMAmbiguityTool", 1, 0);

class IEMAmbiguityTool : virtual public IAlgTool {

public:

  /** @brief Virtual destructor*/
  virtual ~IEMAmbiguityTool() {};

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode        initialize() = 0;
  /** @brief finalize method*/
  virtual StatusCode        finalize() = 0;

  /** @brief Return (future) author: electron, photon, ambiguous, undefined */
  virtual unsigned int ambiguityResolve(const egammaRec* egRec) const = 0;

};

inline const InterfaceID& IEMAmbiguityTool::interfaceID()
{
  return IID_IEMAmbiguityTool;
}

#endif // AMBIGUITYTOOL_H 
