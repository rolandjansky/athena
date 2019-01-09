/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ReadoutTestAlg.h
 *
 * @brief Header file for the SCT_ReadoutTestAlg class 
 *  in package SCT_ConditionsAlgorithms
 *
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 **/

#ifndef SCT_ReadoutTestAlg_H
#define SCT_ReadoutTestAlg_H 

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"

#include "SCT_ConditionsTools/ISCT_ReadoutTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

// C++
#include <string>
#include <vector>

// Forward declarations
class SCT_chip;

/**
 * @class SCT_ReadoutTool
 * Example class to show calling the SCT_ReadoutTool
 **/

class SCT_ReadoutTestAlg : public AthAlgorithm {
 public:
  SCT_ReadoutTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ReadoutTestAlg() = default;

  /** Usual framework methods for an Algorithm*/
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_ReadoutTool> m_readout{this, "SCT_ReadoutTool", "SCT_ReadoutTool", "Handle to the SCT_ReadoutTool"};

  //!< List of chips for that module. *Modified in execute* This should be OK because this is a test alg.
  std::vector<SCT_Chip*> m_chips;

  StringArrayProperty m_chipConfigs{this, "ChipConfigurationList", {}, "The configuration of the chips in that module (from job options)"};
  IntegerProperty m_moduleId{this, "ModuleId", 168497152, "The Module identifier (from job options)"};
  BooleanProperty m_link0ok{this, "Link0Status", true, "Status of link 0 (from job options)"};
  BooleanProperty m_link1ok{this, "Link1Status", true, "Status of link 1 (from job options)"};

  /** Function to convert the configuration string into an int*/
  short bin2dec(const char *bin);
  /** Initialise a chip given its id and configuration string*/
  SCT_Chip* initialiseChip(short id, std::string configString);

}; //end of class

#endif // SCT_ReadoutTestAlg_H
