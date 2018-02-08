/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LAR_RDO_ANALYSIS_H
#define LAR_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "LArRawEvent/LArTTL1Container.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class LArRDOAnalysis : public AthAlgorithm {

public:
  LArRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArRDOAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  SG::ReadHandleKey<LArRawChannelContainer> m_inputRawChannelKey;
  SG::ReadHandleKey<LArTTL1Container> m_inputTTL1HADKey;
  SG::ReadHandleKey<LArTTL1Container> m_inputTTL1EMKey;
  SG::ReadHandleKey<LArDigitContainer> m_inputDigitKey;

  // LAR RAW CHANNELS
  std::vector<unsigned long long>* m_larID;
  std::vector<int>* m_energy;
  std::vector<int>* m_time;
  std::vector<uint16_t>* m_qual;
  std::vector<uint16_t>* m_prov;
  std::vector<int>* m_gain;
  // LAR TTL1
  std::vector<unsigned long long>* m_hadOnID;
  std::vector<unsigned long long>* m_hadOffID;
  std::vector<float>* m_hadSamples;
  std::vector<unsigned long long>* m_emOnID;
  std::vector<unsigned long long>* m_emOffID;
  std::vector<float>* m_emSamples;
  // LAR DIGITS
  std::vector<unsigned long long>* m_digiID;
  std::vector<int>* m_digiGain;
  std::vector<short>* m_digiSamples;

  // HISTOGRAMS
  TH1* m_h_larID;
  TH1* m_h_energy;
  TH1* m_h_time;
  TH1* m_h_qual;
  TH1* m_h_prov;
  TH1* m_h_gain;
  TH1* m_h_hadOnID;
  TH1* m_h_hadOffID;
  TH1* m_h_hadSamples;
  TH1* m_h_emOnID;
  TH1* m_h_emOffID;
  TH1* m_h_emSamples;
  TH1* m_h_digiID;
  TH1* m_h_digiGain;
  TH1* m_h_digiSamples;

  TTree *m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;

};

#endif // LAR_RDO_ANALYSIS_H
