/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_IROISUNPACKINGTOOL_H
#define L1DECODER_IROISUNPACKINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}

/**
 * Interface for RoI unpacking tools
 */
class IRoIsUnpackingTool : virtual public IAlgTool { 
public: 
  DeclareInterfaceID(IRoIsUnpackingTool, 1, 0);
    
  /// Invoked when there is a potential change of the configuration. Typically start().
  virtual StatusCode updateConfiguration() = 0; 
  
  /**
   * Unpack RoIB result depending on implementaton (e.g. EM, Jet, ...)
   *
   * In addition to the implementation specific collection, a collection of decision objects 
   * is created with each decision tagged by the chain ID it relates to. The mapping of threshold IDs 
   * and the chains is provided externally at each call (may be refactored later as this is pure config information).
   */
  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const = 0;
};

#endif
