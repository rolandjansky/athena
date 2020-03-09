/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  virtual ~LUCID_DigiTop() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual bool isClonable() const override final { return true; }
private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "LUCID_PileUpTool", "LUCID_PileUpTool", ""};
};
#endif
