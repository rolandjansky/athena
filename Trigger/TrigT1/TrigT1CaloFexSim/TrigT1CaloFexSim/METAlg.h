/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_METALG_H
#define TRIGT1CALOFEXSIM_METALG_H

/*
 * Class    : METAlg
 *
 * Authors  : Lin, Chiao-Ying (cylin@cern.ch); Myers, Ava (amyers@cern.ch)
 *
 * Date     : Oct 2018 - Updated class
 *
 * Includes algorithms for MET reconstruction and pileup suppression.
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

 static std::map<TString, std::shared_ptr<MET>> m_METMap;
  /**
   *@brief Calculate MET using a fixed 4 sigma noise cut
   */
  static StatusCode Baseline_MET(const xAOD::JGTowerContainer*towers, TString metname, std::vector<float> noise, bool useNegTowers);
  /**
   *@brief Calculates MET with pileup subtraction
   */
  static StatusCode SubtractRho_MET(const xAOD::JGTowerContainer* towers, TString metname, bool useRMS, bool useMedian, bool useNegTowers);
  /**
   *@brief Calculates MET with Softkiller
   */
  static StatusCode Softkiller_MET(const xAOD::JGTowerContainer* towers, TString metname, bool useNegTowers);
  /**
   *@brief Calculates MET with Jets without Jets
   */
  static StatusCode JwoJ_MET(const xAOD::JGTowerContainer* towers, TString metname, float pTcone_cut, bool useNegTowers);
  /**
   *@brief Calculates MET using PUfit
   */
  static StatusCode Pufit_MET(const xAOD::JGTowerContainer* towers, TString metname, bool useNegTowers);
 

};

#endif
