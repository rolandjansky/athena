/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------RECO TRIGGER MATCHING TOOL FOR PAU--------------------------
////////////////////////////////////////////////////////////////////////////////// 
/// Author  : Xabier Anduaga, Martin Tripiana
/// Created : July 2009
/// * See the Twiki for details : https://twiki.cern.ch/twiki/bin/view/Main/TriggerHandlerForPhotonAnalysisUtils
////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_TRIG_RECO_MATCH_H
#define PHOTONANALYSISUTILS_TRIG_RECO_MATCH_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "PhotonAnalysisUtils/IPAUtrigRecoMatch.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "PhotonAnalysisUtils/PAUutils.h"


namespace Analysis {
  class Muon;
  class Photon;
  class Electron;
}

class PAUtrigRecoMatch : public AthAlgTool, virtual public IPAUtrigRecoMatch {
 public:
  /** AlgTool like constructor */   
  PAUtrigRecoMatch(const std::string&,const std::string&,const IInterface*);   

   /**Virtual destructor*/
   virtual ~PAUtrigRecoMatch();
  
    StatusCode initialize();
    StatusCode finalize();

    const egamma* getBestEFmatch(const PAU::egamma* );  
    const TrigEMCluster* getBestL2match(const PAU::egamma* );
    const EmTau_ROI* getBestL1match(const PAU::egamma* ); 

    const egamma* getBestEFmatch(const Analysis::Electron* );  
    const egamma* getBestEFmatch_ph(const Analysis::Electron* );  
    const TrigElectron* getBestL2match(const Analysis::Electron* );
    const TrigEMCluster* getBestL2match_ph(const Analysis::Electron* );
    const EmTau_ROI* getBestL1match(const Analysis::Electron* ); 
    
    //check if best matched trigger objects actually caused the trigger
    unsigned int causedTrigger(const EmTau_ROI*, const std::vector<std::string> );
    
    unsigned int causedTrigger(const TrigEMCluster* , std::vector<std::string> );
    unsigned int causedTrigger(const egamma* , std::vector<std::string> );
    
    unsigned int causedTrigger(const TrigElectron* , std::vector<std::string> );
 private:
    
    float m_trigHLTRecoDeltaR;
    float m_trigL1RecoDeltaR;
    // job options configurable
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
    ToolHandle<TrigMatchTool> m_matchTool;
}; // end of class PAUtrigRecoMatch

#endif
