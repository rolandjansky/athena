// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigAnalysisExample.h 604985 2014-07-04 15:20:52Z krasznaa $
#ifndef TRIGANALYSISEXAMPLE_H
#define TRIGANALYSISEXAMPLE_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

class TrigAnalysisExample : public AthAlgorithm {

public:
   TrigAnalysisExample( const std::string& name, ISvcLocator* pSvcLocator );

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
   ToolHandle< Trig::TrigDecisionTool > m_trigDec;

}; // class TrigAnalysisExample

#endif // TRIGANALYSISEXAMPLE_H
