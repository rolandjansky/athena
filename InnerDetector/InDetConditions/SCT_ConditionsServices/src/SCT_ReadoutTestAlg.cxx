/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ReadoutTestAlg.cxx
 *
 * @brief Implementation file for the SCT_ReadoutTestAlg class 
 * in package SCT_ConditionsService
 *
 * @author Carl Gwilliam <gwilliam@hep.ph.liv.ac.uk>
 **/

#include "SCT_ReadoutTestAlg.h"

// C++
#include <vector>

// Athena includes
#include "Identifier/Identifier.h"

// Local includes
#include "SCT_ReadoutTool.h"
#include "SCT_Chip.h"

SCT_ReadoutTestAlg::SCT_ReadoutTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_readout("SCT_ReadoutTool", this),
  m_moduleId(168497152),
  m_link0ok(true),
  m_link1ok(true)
{
  // Constructor
  declareProperty("ChipConfigurationList", m_chipConfigs);
  declareProperty("ModuleId",              m_moduleId);
  declareProperty("Link0Status",           m_link0ok);
  declareProperty("Link1Status",           m_link1ok);
}

// Destructor
SCT_ReadoutTestAlg::~SCT_ReadoutTestAlg()
{ 
  ATH_MSG_INFO( "Calling destructor" );
}

// Initialize
StatusCode SCT_ReadoutTestAlg::initialize()
{  
  ATH_MSG_INFO( "Calling initialize" );

  // Retrive readout tool
  ATH_CHECK(m_readout.retrieve());

  // Initalise chips configured in job options and add to vector
  std::vector<std::string>::const_iterator itr(m_chipConfigs.begin());
  std::vector<std::string>::const_iterator end(m_chipConfigs.end());

  for (short ichip(0); itr != end; ++itr, ++ichip) {
    SCT_Chip* chip = initialiseChip(ichip, *itr);
    m_chips.push_back(chip);
  } 

  return StatusCode::SUCCESS;
}

// Execute
StatusCode SCT_ReadoutTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients

  ATH_MSG_INFO( "Calling execute" );

  std::vector<SCT_Chip*>::const_iterator itr(m_chips.begin());
  std::vector<SCT_Chip*>::const_iterator end(m_chips.end());

  ATH_MSG_INFO( "Chips before readout ..." );
  for (;itr != end; ++itr)  msg(MSG::INFO) << *(*itr) << endmsg;

  // Determin readout for this module
  ATH_CHECK(m_readout->determineReadout(Identifier(m_moduleId), m_chips, m_link0ok, m_link1ok));
  
  ATH_MSG_INFO( "Chips after readout ..." );
  for (itr = m_chips.begin();itr != end; ++itr) msg(MSG::INFO) << *(*itr) << endmsg;

  return StatusCode::SUCCESS;
}


// Finalize
StatusCode SCT_ReadoutTestAlg::finalize(){
  msg(MSG::INFO) << "Calling finalize" << endmsg;

  // Free up the memory associated to the chips
  std::vector<SCT_Chip*>::const_iterator itr(m_chips.begin());
  std::vector<SCT_Chip*>::const_iterator end(m_chips.end());

  while (itr != end) {
    delete *itr;
    ++itr;
  }

  return StatusCode::SUCCESS;
}

// Connvert a binary number to decimal
int SCT_ReadoutTestAlg::bin2dec(const char *bin)
{
  int  b, k, m, n;
  int  len, sum = 0;
  
  len = strlen(bin) - 1;
  for (k = 0; k <= len; k++) {
      n = (bin[k] - '0'); // char to numeric value
      if ((n > 1) || (n < 0)) {
        msg(MSG::ERROR) << "ERROR! BINARY has only 1 and 0!" << endmsg;
        return 0;
      }
      
      for(b = 1, m = len; m > k; m--) {
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
  short config = bin2dec(configString.c_str());
  int minus1 = -1;
  return new SCT_Chip(id, config, minus1, minus1, minus1, minus1);
}

