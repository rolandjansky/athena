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
//using namespace HLT;

/**
 * @brief Algorithms making correcting of jet Et depending of centrality of HI event
 * The centrality is estimated by the ammount of total energy.
 **/
class TrigHIJetBaseSubtraction : public virtual HLT::FexAlgo{
public:
  TrigHIJetBaseSubtraction(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~TrigHIJetBaseSubtraction();

  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltFinalize();  
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, 
			       HLT::TriggerElement* output);  
  virtual HLT::ErrorCode hltEndEvent();
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
