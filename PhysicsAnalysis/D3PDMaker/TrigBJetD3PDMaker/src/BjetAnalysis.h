// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGBJETD3PDMAKER_BJETANALYSIS_H
#define TRIGBJETD3PDMAKER_BJETANALYSIS_H

// STL include(s):
#include <string>

// ROOT include(s):
#include <Rtypes.h>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// Forward declaration(s):
class TH1F;
namespace Trig {
   class ChainGroup;
}

class BjetAnalysis : public AthAlgorithm {

public:
   BjetAnalysis(const std::string &name, ISvcLocator *pSvcLocator); 

   virtual StatusCode initialize();
   virtual StatusCode execute(); 

private:
   TH1F *makeTH1F( const std::string &name, const std::string &title, Int_t nbinsx,
                   Double_t xlow, Double_t xhigh );
   Float_t deltaPhi( Float_t dphi );

   ServiceHandle< ITHistSvc > m_histSvc;
   ToolHandle< Trig::TrigDecisionTool > m_trigDec;
        
   std::string m_file;

   std::string m_L2chain;
   std::string m_EFchain;
}; // class BjetAnalysis

#endif // TRIGBJETD3PDMAKER_BJETANALYSIS_H
