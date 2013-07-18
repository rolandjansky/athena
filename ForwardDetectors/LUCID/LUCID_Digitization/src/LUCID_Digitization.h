/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITIZATION_H
#define LUCID_DIGITIZATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class LUCID_DigiTop
 *  @brief an algorithm to perform the digitization of LUCID SimHits
 */
class LUCID_DigiTop : public AthAlgorithm {
public:
  LUCID_DigiTop(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif
