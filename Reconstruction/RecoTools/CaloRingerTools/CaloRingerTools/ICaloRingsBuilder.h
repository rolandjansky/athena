/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICaloRingsBuilder.h 670609 2015-05-28 14:45:21Z wsfreund $
#ifndef CALORINGERTOOLS_ICALORINGSBUILDER
#define CALORINGERTOOLS_ICALORINGSBUILDER

/**
   @class ICaloRingsBuilder
   @brief Interface for tool CaloRingsBuilder

   @author Werner S. Freund <wsfreund@cern.ch>

   $Revision: 670609 $
   $$$
*/


#include "GaudiKernel/IAlgTool.h"
#include "AthLinks/ElementLink.h"
#include "StoreGate/WriteHandle.h"

// xAOD includes:
#include "xAODBase/IParticle.h"
#include "xAODCaloRings/RingSetConf.h"
#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/RingSetContainer.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"

namespace Ringer {

static const InterfaceID IID_ICaloRingsBuilder("ICaloRingsBuilder", 1, 0);

class ICaloRingsBuilder : virtual public IAlgTool
{
 public:

  /** @brief Virtual destructor*/
  virtual ~ICaloRingsBuilder() {};

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute container creation method*/
  virtual StatusCode preExecute( xAOD::CaloRingsContainer* crCont
                               , xAOD::RingSetContainer* rsCont
                               , const std::size_t nReserve = 0) = 0;
  /**
   * @brief build CaloRings for IParticle
   **/
  virtual StatusCode execute(const xAOD::IParticle &particle,
      ElementLink<xAOD::CaloRingsContainer> &clRingsLink) = 0;
  /**
   * @brief build CaloRings for CaloCluster
   **/
  virtual StatusCode execute(const xAOD::CaloCluster &cluster,
      ElementLink<xAOD::CaloRingsContainer> &clRingsLink) = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

  /// Extra methods:
  /// @{
  /**
   * @brief Retrieve Raw CaloRingsConfiguration Collection
   **/
  virtual const xAOD::RingSetConf::RawConfCollection &rawConf() = 0;
  /**
   * @brief Retrieve whether it was set used shower shape barycenter
   **/
  virtual bool useShowerShapeBarycenter() = 0;
  virtual std::size_t nRingSets() const = 0;
  virtual const SG::WriteHandleKey<xAOD::CaloRingsContainer>& crContName() const = 0;
  virtual const SG::WriteHandleKey<xAOD::RingSetContainer>& rsContName() const = 0;
  /// @}

};

inline const InterfaceID& ICaloRingsBuilder::interfaceID()
{
  return IID_ICaloRingsBuilder;
}

} // namespace Ringer

#endif
