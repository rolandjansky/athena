/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHIHYPO_TRIGHIJETBASESUBTRACTION_H
#define TRIGHIHYPO_TRIGHIJETBASESUBTRACTION_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "TrigCaloEvent/TrigT2Jet.h"


namespace HLT {
  class TriggerElement;
}
using namespace HLT;

/**
 * @brief Algorithms making correcting of jet Et depending of centrality of HI event
 * The centrality is estimated by the ammount of total energy.
 **/
class TrigHIJetBaseSubtraction : public virtual FexAlgo{
public:
  TrigHIJetBaseSubtraction(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~TrigHIJetBaseSubtraction();

  virtual ErrorCode hltInitialize();
  virtual ErrorCode hltFinalize();  
  virtual ErrorCode hltExecute(const TriggerElement* input, 
			       TriggerElement* output);  
  virtual ErrorCode hltEndEvent();
private:

  std::string m_label;
  double m_lvl1_sumEt; 
  double m_tower_background;
  double m_jet_background;
  double m_jet_weighted_background;
  int m_ieta;
  
  //Configured by python
  float m_coneRadius;
  float m_CaloArea;
  std::vector<float> m_BgEtaReg;
  std::vector<float> m_BgWeights;
  int m_Bgnbin_eta;  
  
  //monitoring variable
  double m_Jet_Et;
  double m_Jet_Eta;
  double m_SumEt;
  double m_Background;

  
  
  /**
   * @brief returns jet with corrected Et
   * @warning when no correction is needed then 0 pointer is returned
   * This is the woking horse of this algoritm
   **/
  TrigT2Jet* corrected(const TrigT2Jet*, double ) const;

};

#endif // TRIGHIHYPO_TRIGHIJETBASESUBTRACTION_H
