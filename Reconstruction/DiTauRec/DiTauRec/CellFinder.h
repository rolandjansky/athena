/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_CELLFINDER_H
#define DITAUREC_CELLFINDER_H

#include "DiTauToolBase.h"

#include "GaudiKernel/ToolHandle.h"


class CellFinder : public DiTauToolBase {
 public:

  CellFinder(const std::string& type,
	     const std::string& name,
	     const IInterface * parent);

  virtual ~CellFinder();

  virtual StatusCode initialize() override;

  virtual StatusCode execute(DiTauCandidateData * data,
			     const EventContext& ctx) const override;


 private:
  bool m_bWriteJetCells;
  bool m_bWriteSubjetCells;
  std::string m_ClusterContainerName;
  std::string m_CellContainerName;
  float m_Rsubjet;

};

#endif  // DITAUREC_CELLFINDER_H

