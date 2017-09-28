/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoAllTE.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHORS:   John Alison   johnda@uchicago.edu
// 
// ************************************************

#ifndef TRIGBJETHYPOALLTE_H
#define TRIGBJETHYPOALLTE_H

#include "TrigInterfaces/AllTEAlgo.h"

class TriggerElement;

class TrigBjetHypoAllTE: public HLT::AllTEAlgo {

 public:

  TrigBjetHypoAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigBjetHypoAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltEndEvent();

 private:

  const xAOD::BTagging* getBTaggingPtr(HLT::TriggerElement* btagTE);
  const xAOD::Jet* getJetPtr(const xAOD::BTagging* btagInfo);
  bool  eventPassesTrigger();
  void  clearCounters();

 private:

  /** @brief vectors with Et thresholds, btagging cut, and multiplicity requirement */
  /** @brief  These are all required to be the same length and a given index is used */
  /** @brief  to configure a "triggerRequirement" that is ANDed with the others  */
  std::vector<double> m_EtThresholds;
  std::vector<double> m_BTagMin;
  std::vector<double> m_BTagMax;
  std::vector<int>    m_Multiplicities;

  /** @brief vectors of vectors with Et thresholds, btagging cut, and multiplicity requirement */
  /** @brief  These are all required to be the same length.  */
  /** @brief  A given index is used to configure a list of "triggerRequirement"s */
  /** @brief  The requirements within the list are ANDed, and the lists are ORed */
  std::vector<std::vector<double> > m_EtThresholdsOR;
  std::vector<std::vector<double> > m_BTagMinOR;
  std::vector<std::vector<double> > m_BTagMaxOR;
  std::vector<std::vector<double> > m_MultiplicitiesOR;
  
  /** @brief string corresponding to the trigger level in which the algorithm is running. */
  //  std::string m_instance;

  /** @brief Name of the input btagging objects. */
  std::string m_btaggingKey;

  /** @brief Name of the btagging variable. */
  std::string m_tagger;

  /** @brief to check the beam spot flag status. */
  bool m_useBeamSpotFlag;

  /** @brief Overide the requirement that the BS is valid. */
  /** @brief Used to not apply the correction to the GSC chains */
  bool m_overRideBeamSpotValid;

  /** @brief Used for monitoring. */
  /** @ brief -1 - invalid value */
  /** @ brief  0 - Fail Event Cut  */
  /** @ brief  1 - Accept Event  */
  /** @ brief  2 - Fail BS   */
  /** @ brief  3 - Too few input TEs  */
  /** @ brief  4 - Too many input TEs  */
  /** @ brief  5 - No input bjets  */
  /** @ brief  6 - Failed to get btagging pointer  */
  /** @ brief  7 - Failed to get jet pointer  */
  float m_cutCode = -1;


  /** @ brief Beamspot Code */
  /** @ brief -1 - invalid value */
  /** @ brief  0 - had valid BS */
  /** @ brief  1 - no valid BS (reject event) */
  /** @ brief  2 - no valid BS (do not reject event) */
  /** @ brief  3 - cannot read BS information */
  /** @ brief  4 - do not use BS information */
  float m_BSCode = -1;

  /** @ brief Number of input jets */  
  float m_nInputJets = -1;

  /** @ brief Basic unit of trigger requirment  */
  /** @ brief Contains Et threhold / min & max btagging values and multiplicity requirement   */
  struct triggerRequirement{
    float          m_EtThreshold;
    float          m_btagMin;
    float          m_btagMax;
    unsigned int   m_multiplicity;

    unsigned int   m_count;
    triggerRequirement(float EtThreshold, float btagMin,  float btagMax, unsigned int multiplicity)  : 
      m_EtThreshold(EtThreshold), 
      m_btagMin(btagMin), 
      m_btagMax(btagMax), 
      m_multiplicity(multiplicity),  
      m_count(0) 
    { }

    bool pass(){
      return bool(m_count >= m_multiplicity);
    }
    
    void countJet(float btagEt, float btagWeight){
      if((btagEt > m_EtThreshold) && (btagWeight > m_btagMin) && (btagWeight < m_btagMax))
	++m_count;
    }
    
  };
  
  /** @ brief the triggerRequirement in this list are added  */
  std::vector<triggerRequirement>               m_triggerReqsAND;

  /** @ brief The inner list of trigger requirements are ANDed */
  /** @ brief And the result of each of these is ORed  */
  std::vector<std::vector<triggerRequirement> > m_triggerReqsOR;

};

#endif


