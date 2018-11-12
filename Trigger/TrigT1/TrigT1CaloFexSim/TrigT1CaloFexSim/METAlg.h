/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/IdentifierHash.h"
#include "TH1.h"
#include "TH2.h"

class METAlg{
  
 public:
  struct MET{
    float phi;
    float et;
    
  };
  
  static StatusCode BuildMET(const xAOD::JGTowerContainer*towers,METAlg::MET* met,std::vector<float> noise, bool useNegTowers);
  static StatusCode SubtractRho_MET(const xAOD::JGTowerContainer* towers, METAlg::MET* met, bool useRMS, bool useMedian, bool useNegTowers);
  static StatusCode Softkiller_MET(const xAOD::JGTowerContainer* towers, METAlg::MET* met, bool useNegTowers);
  static StatusCode JwoJ_MET(const xAOD::JGTowerContainer* towers, METAlg::MET* met, bool useNegTowers);
  static StatusCode Pufit_MET(const xAOD::JGTowerContainer* towers, METAlg::MET* met, bool useNegTowers);
};
