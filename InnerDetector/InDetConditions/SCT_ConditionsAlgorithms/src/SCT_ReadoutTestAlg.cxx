/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ReadoutTestAlg.cxx
 *
 * @brief Implementation file for the SCT_ReadoutTestAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Carl Gwilliam <gwilliam@hep.ph.liv.ac.uk>
 **/

// Athena includes
#include "SCT_ReadoutTestAlg.h"

#include "Identifier/Identifier.h"
#include "SCT_ConditionsData/SCT_Chip.h"

// Constructor
SCT_ReadoutTestAlg::SCT_ReadoutTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator)
{
}

// Initialize
StatusCode SCT_ReadoutTestAlg::initialize() {  
  ATH_MSG_INFO("Calling initialize");

  // Retrive readout tool
  ATH_CHECK(m_readout.retrieve());

  // Initalise chips configured in job options and add to vector
  std::vector<std::string>::const_iterator itr{m_chipConfigs.begin()};
  std::vector<std::string>::const_iterator end{m_chipConfigs.end()};

  for (short ichip{0}; itr != end; ++itr, ++ichip) {
    SCT_Chip* chip{initialiseChip(ichip, *itr)};
    m_chips.push_back(chip);
  } 

  return StatusCode::SUCCESS;
}

// Execute
StatusCode SCT_ReadoutTestAlg::execute() {
  //This method is only used to test the readout tool, and only used within this package,
  // so the INFO level messages have no impact on performance of the tool when used by clients

  ATH_MSG_INFO("Calling execute");

  ATH_MSG_INFO( "Chips before readout ..." );
  for (const SCT_Chip* chip: m_chips) ATH_MSG_INFO(*chip);

  // Determin readout for this module
  ATH_CHECK(m_readout->determineReadout(Identifier{m_moduleId.value()}, m_chips, m_link0ok.value(), m_link1ok.value()));
  
  ATH_MSG_INFO("Chips after readout ...");
  for (const SCT_Chip* chip: m_chips) ATH_MSG_INFO(*chip);

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_ReadoutTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");

  // Free up the memory associated to the chips
  for (const SCT_Chip* chip: m_chips) delete chip;

  return StatusCode::SUCCESS;
}

// Connvert a binary number to decimal
short SCT_ReadoutTestAlg::bin2dec(const char *bin)
{
  short b, k, m, n;
  short len;
  short sum{0};
  
  len = strlen(bin) - 1;
  for (k = 0; k <= len; k++) {
    n = (bin[k] - '0'); // char to numeric value
    if ((n > 1) or (n < 0)) {
      ATH_MSG_ERROR("ERROR! BINARY has only 1 and 0!");
      return 0;
    }
      
    for (b = 1, m = len; m > k; m--) {
      // 1 2 4 8 16 32 64 ... place-values, reversed here
      b *= 2;
    }

    // sum it up
    sum = sum + n * b;
  }
  return sum;
}

// Initalise chip from id and config string (all channels are initially good)
SCT_Chip* SCT_ReadoutTestAlg::initialiseChip(short id, std::string configString) {
  // Opposite convention for LSB
  std::reverse(configString.begin(), configString.end());
  short config{bin2dec(configString.c_str())};
  const int minus1{-1};
  return new SCT_Chip(id, config, minus1, minus1, minus1, minus1);
}
