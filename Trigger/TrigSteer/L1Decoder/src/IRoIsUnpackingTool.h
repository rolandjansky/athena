/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_IROISUNPACKINGTOOL_H
#define L1DECODER_IROISUNPACKINGTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}

static const InterfaceID IID_IRoIsUnpackingTool("IRoIsUnpackingTool", 1, 0);

class IRoIsUnpackingTool
  : virtual public ::IAlgTool
{ 

 public: 


  /** Destructor: 
   */
  virtual ~IRoIsUnpackingTool();
  
  typedef HLT::IDtoIDVecMap ThresholdToIdentifiers;
  
  static const InterfaceID& interfaceID();

  /*
    @brief Invoked when there is a potential change of the configuration. Typically beginRun.
   */
  virtual StatusCode updateConfiguration() = 0; 

  
  /*
    @brief The methods reads the RoIB result object and unpacks fragment of it, depending of the implementation (i.e. EM, J..)
    In addition to the impl. specific collection a collection of decision objects is created with each decision tagged by the chain ID it relates to.
    The mapping of threshold IDs and the chains is provided externally at each call (may be refactored later as this is pure config information).
   */
  virtual StatusCode unpack(const EventContext& ctx,
			    const ROIB::RoIBResult& roib,
			    const HLT::IDSet& activeChains) const = 0;


  
protected:
  
  SG::WriteHandleKey<  TrigCompositeUtils::DecisionContainer> m_decisionsKey;
  std::vector<std::string>              m_thresholdToChainProperty;
  std::map<HLT::Identifier, HLT::IDVec> m_thresholdToChainMapping;


  StatusCode decodeMapping();
  void addChainsToDecision( HLT::Identifier thresholdId,
			    TrigCompositeUtils::Decision* d,
			    const HLT::IDSet& activeChains ) const;
}; 

inline const InterfaceID& IRoIsUnpackingTool::interfaceID() { 
   return IID_IRoIsUnpackingTool; 
}


#endif //> !L1DECODER_IROISUNPACKINGTOOL_H
