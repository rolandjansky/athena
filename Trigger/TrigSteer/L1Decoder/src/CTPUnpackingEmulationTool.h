/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGEMULATIONTOOL_H
#define L1DECODER_CTPUNPACKINGEMULATIONTOOL_H

#include "L1Decoder/CTPUnpackingToolBase.h"

#include "TrigCompositeUtils/HLTIdentifier.h"

class CTPUnpackingEmulationTool  : public CTPUnpackingToolBase {

public:

  CTPUnpackingEmulationTool( const std::string& type,
                             const std::string& name, 
                             const IInterface* parent );

  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

  virtual StatusCode initialize() override;

private:

  ///@{ @name Properties

  Gaudi::Property<std::string> m_inputFileName{this, "InputFilename", "CTPEmulation.dat", "Fake CTP RoIb input filename"};

  ///@}

  // @brief returns names oc the chains (iner vector) to activate for each event (outer vector) 
  //  std::vector < std::set< size_t > > m_events; // list of CTPID enabled, per event
  std::vector < HLT::IDVec > m_events;
  StatusCode parseInputFile() ;

}; 


#endif
