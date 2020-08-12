/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaSwTool.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMASWTOOL_H
#define EGAMMAINTERFACES_IEGAMMASWTOOL_H

/** @class IegammaSwTool
  Interface for the Reconstruction/egamma/egammaRec/egammaSwTool

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 01/09/2008
MODIFIED : Bruno Lenzi (02/2014): xAOD migration
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaEnums.h"

static const InterfaceID IID_IegammaSwTool("IegammaSwTool", 1, 0);

class IegammaSwTool : virtual public IAlgTool
{

public:
  /** @brief Virtual destructor*/
  virtual ~IegammaSwTool(){};

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute method*/
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloCluster* cluster) const = 0;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloCluster* cluster,
                             xAOD::EgammaParameters::EgammaType egType,
                             bool isBarrel) const = 0;
};

inline const InterfaceID&
IegammaSwTool::interfaceID()
{
  return IID_IegammaSwTool;
}

#endif
