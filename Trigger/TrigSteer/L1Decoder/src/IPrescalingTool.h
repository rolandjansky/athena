#ifndef L1DEcoder_IPrescalingTool_h
#define L1DEcoder_IPrescalingTool_h

#include "DecisionHandling/HLTIdentifier.h"
#include "GaudiKernel/IAlgTool.h"

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
   * Internally implementations require to keep RNGs and therefore the interface is non-const.
   **/
  virtual StatusCode prescaleChains( const EventContext& ctx,  
				     const HLT::IDVec& initialyActive,
				     HLT::IDVec& remainActive ) = 0;
  
  

};


#endif
