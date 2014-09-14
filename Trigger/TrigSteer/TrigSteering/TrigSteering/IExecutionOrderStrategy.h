/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LvlConverter
 *
 * @brief Base class defining the interface for the modification of chains ordering strategy
 *
 * @author Tomasz Bold
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERING_IEXECUTIONORDERSTRATEGY_H
#define TRIGSTEERING_IEXECUTIONORDERSTRATEGY_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigSteering/SteeringChain.h"






namespace HLT {

  static const InterfaceID IID_IExecutionOrderStrategy("HLT::IExecutionOrderStrategy", 1, 0); //!< std Gaudi interface

  /**
   * @class IExecutionOrderStrategy
   * Interface to the base class for all possible chains execution orders
   */
  class IExecutionOrderStrategy : public virtual IAlgTool {
  public:

    /**
     * @brief Prepares sorting functions (in case there is expensive checks on configuration needed to decide which chain should be run first) 
     * This has to be be reimplemented by each strategy implementation.
     */
    virtual StatusCode prepare(std::vector<HLT::SteeringChain*>& all_configured_chains) = 0; 

    /**
     * @brief Reorders chains in the vector.
     * This has to be be reimplemented by each strategy implementation.
     */
    virtual void apply(std::vector<HLT::SteeringChain*>& active_chains) const = 0; 

    static const InterfaceID& interfaceID() { return IID_IExecutionOrderStrategy; } //!< Gaudi interface method
  
  };
}

#endif
