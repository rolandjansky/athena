/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSeeding_IPrescalingTool_h
#define HLTSeeding_IPrescalingTool_h

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "GaudiKernel/IAlgTool.h"

class EventContext;


/**
 * @class Implementations of this interface are responsible for 
 * applying scaler decisions on the set of active chains
 **/
class IPrescalingTool : virtual public IAlgTool { 
public: 
  DeclareInterfaceID( IPrescalingTool, 1, 0 );

  /**
   * @brief the interface needed to do the prescaling
   * @arg initialyActive set of chains initiated by the L1 seeds
   * @arg remainActive set of chains that remain active (relevant) after prescaling   
   **/
  virtual StatusCode prescaleChains( const EventContext& ctx,
                                     const HLT::IDVec& initialyActive,
                                     HLT::IDVec& remainActive ) const = 0;
  
  

};


#endif
