// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGNAVTOOLS_TRIGNAVIGATIONTHINNINGTOOL_H
#define TRIGNAVTOOLS_TRIGNAVIGATIONTHINNINGTOOL_H

#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"

class TrigNavigationThinningTool : public AthAlgTool, public DerivationFramework::IThinningTool {
public:
  TrigNavigationThinningTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigNavigationThinningTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode doThinning() const;

private:  
  ServiceHandle<IThinningSvc> m_thinningSvc;
  ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool;
  std::string m_resultKey;
  bool m_actInPlace;
};

#endif
