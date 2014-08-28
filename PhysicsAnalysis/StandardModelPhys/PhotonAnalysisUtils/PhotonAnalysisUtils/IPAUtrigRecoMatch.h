/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------RECO TRIGGER MATCHING TOOL FOR PAU--------------------------
////////////////////////////////////////////////////////////////////////////////// 
/// Author  : Xabier Anduaga, Martin Tripiana
/// Created : July 2009
/// * See the Twiki for details : https://twiki.cern.ch/twiki/bin/view/Main/TriggerHandlerForPhotonAnalysisUtils
////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef PHOTONANALYSISUTILS_ITRIG_RECO_MATCH_H
#define PHOTONANALYSISUTILS_ITRIG_RECO_MATCH_H


#include "GaudiKernel/IAlgTool.h"
//photons
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"                                         
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigParticle/TrigElectron.h"

#include "PhotonAnalysisUtils/PAUegamma.h"
#include "PhotonAnalysisUtils/PAUnamespace.h"

namespace Analysis {
  class Muon;
  class Photon;
  class Electron;
}

static const InterfaceID IID_IPAUtrigRecoMatch("IPAUtrigRecoMatch", 1, 0);
        
class IPAUtrigRecoMatch : virtual public IAlgTool {
 public:
  
  /** Virtual destructor */
  virtual ~IPAUtrigRecoMatch(){};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() 
    {       return IID_IPAUtrigRecoMatch;    };

  virtual const egamma* getBestEFmatch(const PAU::egamma* )= 0;  
  virtual const TrigEMCluster* getBestL2match(const PAU::egamma* )= 0;
  virtual const EmTau_ROI* getBestL1match(const PAU::egamma* )= 0; 

  virtual const egamma* getBestEFmatch(const Analysis::Electron* )= 0;  
  virtual const egamma* getBestEFmatch_ph(const Analysis::Electron* )= 0;  
  virtual const TrigElectron* getBestL2match(const Analysis::Electron* )= 0;
  virtual const TrigEMCluster* getBestL2match_ph(const Analysis::Electron* )= 0;
  virtual const EmTau_ROI* getBestL1match(const Analysis::Electron* )= 0; 
    
  //check if best matched trigger objects actually caused the trigger
  virtual unsigned int causedTrigger(const EmTau_ROI*, const std::vector<std::string>)   =0;
  virtual unsigned int causedTrigger(const TrigEMCluster* , std::vector<std::string> )   =0;
  virtual unsigned int causedTrigger(const egamma* , std::vector<std::string> )          =0; 
  virtual unsigned int causedTrigger(const TrigElectron* , std::vector<std::string> )    =0;
 
};

#endif 
