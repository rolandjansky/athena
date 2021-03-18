/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_IROISUNPACKINGTOOL_H
#define L1DECODER_IROISUNPACKINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}

/**
 * Interface for RoI unpacking tools
 */
class IRoIsUnpackingTool : virtual public IAlgTool { 
public: 
  DeclareInterfaceID(IRoIsUnpackingTool, 1, 0);
    

  /**
   * Invoked when there is a potential change of the configuration. Typically beginRun
   * it obtains the information about the L1 item -> HLT seeding relation in a form of a map where the key is the HLT chain name and value is the L1 item
   **/
  virtual StatusCode updateConfiguration( ) = 0; 
  
  /**
   * Unpack RoIB result depending on implementaton (e.g. EM, Jet, ...)
   *
   * A collection of decision objects is created in event store with each decision tagged by the chain ID it relates to.
   * In addition, an implementation specific "RecRoI" collection is produced.
   */
  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const = 0;

  /**
   * Unpack specific sub-system RoIs (Muon, eFexEM, gFexJet, ...) linked to L1TriggerResult depending on implementation
   *
   * A collection of decision objects is created in event store with each decision tagged by the chain ID it relates to.
   */
  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const xAOD::TrigComposite& /*l1TriggerResult*/,
                            const HLT::IDSet& /*activeChains*/) const = 0;
};

#endif
