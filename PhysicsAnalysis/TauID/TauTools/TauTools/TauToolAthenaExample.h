/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUTOOLATHENAEXAMPLE_H
#define TAUTOOLATHENAEXAMPLE_H

////////////////////////////////////////////////////////////////////////////////// 
/// Name    : TauToolAthenaExample.h
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Authors : Dugan O'Neil and Jyothsna Rani
/// Created : September 2007
///
/// DESCRIPTION:
/// Top-level example of how to call tauTools from Athena
/// TruthHandler, RecoHandler and MathcingTools classes present in TauTools package have 
/// a very general interface, requiring only containers of tauJets or truth as input. 
/// So, we provide this example of how to access event information and pass it to tauTools from Athena. Other
/// examples which do the same thing for other analysis formats should also 
/// be available in this CVS package.
///
/// INSTRUCTIONS: Posted on TauTools Wiki page
///               https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TauAnalysis
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TauTools/TauExamples.h"

#include "AthenaBaseComps/AthAlgorithm.h"


class TauToolAthenaExample : public AthAlgorithm  {

 public:

   TauToolAthenaExample(const std::string& name, ISvcLocator* pSvcLocator);
   ~TauToolAthenaExample();
   
   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();
   StatusCode clear();

 private:

  ////////////////////////////////////////////////////////////////////////
  // 0: Instantiate TauExamples class through ToolHandle
  ////////////////////////////////////////////////////////////////////////
  /** instance of the tauexamples class which will be used */
  TauExamples tauexample;
  
  /** Cone matching cut */
  float m_deltaRMatchCut;

  /** name of the AOD truth particle container to retrieve from StoreGate or Transient tree */
  std::string m_truthParticleContainerName;
  
  /** name of the AOD taujet container to retrieve from StoreGate or Transient tree */
  std::string m_taujetContainerName;
  
  int m_event_number;
   
};

#endif // TAUTOOLATHENAEXAMPLE_H
