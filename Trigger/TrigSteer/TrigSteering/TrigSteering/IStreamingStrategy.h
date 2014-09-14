/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LvlConverter
 *
 * @brief Base class for the Streaming strategies
 *
 * @author Tomasz Bold
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERING_ISTREAMINGSTRATEGY_H
#define TRIGSTEERING_ISTREAMINGSTRATEGY_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "EventInfo/TriggerInfo.h"






namespace HLT {

  static const InterfaceID IID_IStreamingStrategy("HLT::IStreamingStrategy", 1, 0); //!< std Gaudi interface

  /**
   * @class IStreamingStrategy
   * Interface to the base class for all possible streaming strategies which HLT steering can eb implementing     
   */
  class IStreamingStrategy : public virtual IAlgTool {
  public:
    /**
     * @brief Modifies vector of streamtags for the event 
     * This has to be be reimplemented by each strategy implementation.
     */
    virtual void apply(std::vector<TriggerInfo::StreamTag>&) const = 0; 
    static const InterfaceID& interfaceID() { return IID_IStreamingStrategy; } //!< Gaudi interface method
  
  };
}

#endif
