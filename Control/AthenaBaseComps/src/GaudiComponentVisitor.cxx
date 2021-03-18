/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaBaseComps/GaudiComponentVisitor.h"
#include "AthenaBaseComps/GaudiComponentVisitor.icc"

namespace xGaudi {
   const std::regex ComponentVisitorBase::s_noFilter{};

   template class ComponentVisitor<IAlgTool>;                        // ToolVisitor
   template class ComponentVisitor<Gaudi::Algorithm, NoVisitTester>; // AlgorithmVisitor
}
