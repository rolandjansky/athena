// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGEMULATIONTOOL_H
#define L1DECODER_CTPUNPACKINGEMULATIONTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "./ICTPUnpackingTool.h"
#include "./CTPUnpackingTool.h"

namespace ROIB {
  class RoIBResult;
}



class CTPUnpackingEmulationTool  : public AthAlgTool, virtual public ::ICTPUnpackingTool{

public:

  CTPUnpackingEmulationTool( const std::string& type,
		    const std::string& name, 
		    const IInterface* parent );

  virtual ~CTPUnpackingEmulationTool();


  /*
    @brief The method decodes CTP bits content of the RoIBResult and fills the list of chains which are activated by those bits
    The mapping ctpToChain is expected to contain the CTP bit number mapping to the chain identifiers
    @warning if the mapping is empty it means an empty menu. This condition is NOT checked and not reported.
    @warning if none of CTP bits is set this is also an error condition, this is the event should not have been passed to HLT
   */
  StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

 // Athena algtool's Hooks
  StatusCode  initialize() override;
  StatusCode  finalize() override;
 
  
 

private:

  std::string              m_inputFileName;
  // @brief returns names oc the chains (iner vector) to activate for each event (outer vector) 
  //  std::vector < std::set< size_t > > m_events; // list of CTPID enabled, per event
  std::vector < HLT::IDVec > m_events;
  StatusCode parseInputFile() ;

}; 


#endif // L1DECODER_CTPUNPACKINGTOOLEMULATION_H
