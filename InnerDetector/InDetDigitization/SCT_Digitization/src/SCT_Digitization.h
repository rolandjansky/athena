/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_Digitization.h  Header file for SCT_Digitization class.
 *  @author Szymon.Gadomski@cern.ch, Awatif.Belymam@cern.ch, Davide.Costanzo@cern.ch,
 *          tgcornel@nikhef.nl, Grant.Gorfine@cern.ch, Paul.Bell@cern.ch,
 *          Jorgen.Dalmau@cern.ch, Kondo.Gnanvo@cern.ch, and others
 * Version 23/08/2007 Kondo.Gnanvo@cern.ch
 *          Conversion of the processors into AlgTool
 */

// Multiple inclusion protection
#ifndef SCT_DIGITIZATION_SCT_DIGITIZATION_H
#define SCT_DIGITIZATION_SCT_DIGITIZATION_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
// Gaudi
#include "GaudiKernel/ToolHandle.h"

class SCT_DigitizationTool;
/** Top algorithm class for SCT digitization */
class SCT_Digitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  SCT_Digitization(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~SCT_Digitization() = default;
    
  /** Basic algorithm methods */
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  ToolHandle<SCT_DigitizationTool> m_sctDigitizationTool{this, "DigitizationTool", "SCT_DigitizationTool", "SCT_DigitizationTool name"};

};

#endif // SCT_DIGITIZATION_SCT_DIGITIZATION_H
