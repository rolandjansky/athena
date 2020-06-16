/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCLUSTERCORRDUMPER
#define CALOREC_CALOCLUSTERCORRDUMPER

/**
 * @class CaloClusterCorrDumper
 * @author Walter Lampl <walter.lampl@cern.ch>
 * @date March, 28th 2006
 *   Reworked Jun 2020 sss
 *
 * Dump out a set of @c ToolConstants objects from the conditions store.
 */


#include "CaloConditions/ToolConstants.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/CondHandleKeyArray.h"
#include <vector>
#include <string>


class CaloClusterCorrDumper : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  SG::ReadCondHandleKeyArray<CaloRec::ToolConstants> m_constants
  { this, "Constants", {}, "List of constants to dump." };
};

#endif // CALOREC_CALOCLUSTERCORRDUMPER
