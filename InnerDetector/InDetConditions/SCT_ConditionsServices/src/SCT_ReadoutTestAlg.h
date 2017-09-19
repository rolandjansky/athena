/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_ReadoutTestAlg.h
*
* @brief Header file for the SCT_ReadoutTestAlg class 
*  in package SCT_ConditionsServices
*
* @author Carl Gwilliam <gwilliam@mail.cern.ch>
**/

#ifndef SCT_ReadoutTestAlg_H
#define SCT_ReadoutTestAlg_H 

//Gaudi
#include "GaudiKernel/ToolHandle.h"

// C++
#include <string>
#include <vector>

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "SCT_ConditionsServices/ISCT_ReadoutTool.h"

// Forward declarations
class StatusCode;
class SCT_chip;

/**
 * @class SCT_ReadoutTool
 * Example class to show calling the SCT_ReadoutTool
**/

class SCT_ReadoutTestAlg : public AthAlgorithm {
 public:
  SCT_ReadoutTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~SCT_ReadoutTestAlg();

  /** Usual framework methods for an Algorithm*/
  StatusCode                          initialize();
  StatusCode                          execute();
  StatusCode                          finalize();
   
 private:
  ToolHandle<ISCT_ReadoutTool>        m_readout{this, "SCT_ReadoutTool", "SCT_ReadoutTool", "Handle to the SCT_ReadoutTool"};      //!< Handle to the SCT_ReadoutTool
  std::vector<SCT_Chip*>              m_chips;        //!< List of chips for that module
  std::vector<std::string>            m_chipConfigs;  //!< The configuration of the chips in that module (from job options)
  int                                 m_moduleId;     //!< The Module identifier (from job options)
  bool                                m_link0ok;      //!< Status of link 0 (from job options)
  bool                                m_link1ok;      //!< Status of link 1 (from job options)

  /** Function to convert the configuration string into an int*/
  int                                 bin2dec(const char *bin);
  /** Initialise a chip given its id and configuration string*/
  SCT_Chip*                           initialiseChip(short id, std::string configString);

}; //end of class

#endif
