/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_IDVARCALCULATOR_H
#define DITAUREC_IDVARCALCULATOR_H

#include "DiTauToolBase.h"

#include "GaudiKernel/ToolHandle.h"


class IDVarCalculator : public DiTauToolBase {
 public:

  //-------------------------------------------------------------
  //! Constructor
  //-------------------------------------------------------------
  IDVarCalculator(const std::string& type,
		  const std::string& name,
		  const IInterface * parent);

  //-------------------------------------------------------------
  //! Destructor
  //-------------------------------------------------------------
  virtual ~IDVarCalculator();

  virtual StatusCode initialize() override;

  virtual StatusCode execute(DiTauCandidateData * data,
			     const EventContext& ctx) const override;


 private:
  bool m_useCells;

};

#endif // DITAUREC_IDVARCALCULATOR_H
