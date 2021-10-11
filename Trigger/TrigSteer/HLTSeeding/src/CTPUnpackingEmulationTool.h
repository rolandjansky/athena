/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTSEEDING_CTPUNPACKINGEMULATIONTOOL_H
#define HLTSEEDING_CTPUNPACKINGEMULATIONTOOL_H

#include "CTPUnpackingToolBase.h"

#include "TrigCompositeUtils/HLTIdentifier.h"

class CTPUnpackingEmulationTool : public CTPUnpackingToolBase {

public:

  CTPUnpackingEmulationTool( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

  virtual StatusCode initialize() override;

  virtual bool isEmulated() const override {return true;}

private:

  Gaudi::Property<std::string> m_inputFileName{
    this, "InputFilename", "CTPEmulation.dat", "Fake CTP RoIB input filename"};

  /// Chain IDs (inner vector) to activate for each event (outer vector)
  std::vector<HLT::IDVec> m_events;

  /// Fill the m_events vector with configuration from an input file
  StatusCode parseInputFile();

};

#endif
