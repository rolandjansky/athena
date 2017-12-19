/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaBaseTool.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMABASETOOL_H
#define EGAMMAINTERFACES_IEGAMMABASETOOL_H

/** @class IegammaBaseTool
  Interface for the Reconstruction/egamma/egammaTools/egammaBaseTool

  @author Frederic Derue derue@lpnhe.in2p3.fr
  @author Christos Anastopoulos

*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"
// Forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainerFwd.h"
#include "xAODEgamma/PhotonContainerFwd.h"

static const InterfaceID IID_IegammaBaseTool("IegammaBaseTool", 1, 0);

class IegammaBaseTool : virtual public IAlgTool
{

 public:

  /** @brief destructor*/
  virtual ~IegammaBaseTool() {};

  /** @brief AthAlgTool interface methods */
  static const InterfaceID& interfaceID();
       
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;
  /** @brief execute method*/
  virtual StatusCode execute(xAOD::Egamma* eg) = 0; 
  /** @brief execute method for tools that operate on containers*/
  virtual StatusCode contExecute(xAOD::ElectronContainer *electronContainer, 
				 xAOD::PhotonContainer *photonContainer) = 0; 

};

inline const InterfaceID& IegammaBaseTool::interfaceID()
{
  return IID_IegammaBaseTool;
}

#endif // EGAMMAINTERFACES_IEGAMMABASETOOL_H
