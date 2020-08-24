/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
#define EGAMMAINTERFACES_IEGAMMABACKSHAPE_H

/*
 * @name   IegammaOQFlagsBuilder.h, (c) ATLAS Detector software 2009
 * @class IegammaOQFlagsBuilder
 * @brief Interface for the
 * Reconstruction/egamma/egammaTools/egammaOQFlagsBuilder
 *
 * @author Christos Anastopoulos
 */

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "GaudiKernel/EventContext.h"
// Forward declarations
#include "xAODEgamma/EgammaFwd.h"
class CaloDetDescrManager;
static const InterfaceID IID_IegammaOQFlagsBuilder("IegammaOQFlagsBuilder",
                                                   1,
                                                   0);

class IegammaOQFlagsBuilder : virtual public IAlgTool
{

public:
  /** @brief Virtual destructor */
  virtual ~IegammaOQFlagsBuilder(){};

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief AlgTool method.*/
  virtual StatusCode execute(const EventContext& ctx,
                             const CaloDetDescrManager& cmgr,
                             xAOD::Egamma& egamma) const = 0;
};

inline const InterfaceID&
IegammaOQFlagsBuilder::interfaceID()
{
  return IID_IegammaOQFlagsBuilder;
}

#endif // EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
