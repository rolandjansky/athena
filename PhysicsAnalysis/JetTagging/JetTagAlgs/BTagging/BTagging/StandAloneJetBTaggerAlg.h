/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_STANDALONEJETBTAGGERTOOL_HH
#define BTAGGING_STANDALONEJETBTAGGERTOOL_HH
///////////////////////////////////////////
///
/// \class StanAloneJetBTaggerAlg
/// StandAlone tool to run and add btagging information.
////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


namespace Analysis{

class IJetBTaggerTool;

class  StandAloneJetBTaggerAlg: 
  public AthAlgorithm
   { 
  public:
  
    /** Constructors and destructors */
    StandAloneJetBTaggerAlg(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~StandAloneJetBTaggerAlg();
    
    /** Main routines specific to an ATHENA algorithm */
    virtual StatusCode initialize();
    virtual StatusCode execute();

  private:
  
    std::string m_JetCollectionName;
    std::string m_suffix;

    ToolHandle< IJetBTaggerTool > m_JetBTaggerTool; 

    // Utility function to be used in case of container overwriting.
    // Note that it do not need to be defined here, as it can only be called from code in StandAloneJetBTaggerAlg.cxx.
    template< class CONTAINER, class AUXSTORE >
      StatusCode overwrite(const std::string& key) const;
};

}

#endif
