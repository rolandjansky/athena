/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef BCM_RDO_ANALYSIS_H
#define BCM_RDO_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include <string>
#include <vector>
#include "TH1.h"

class TTree;
class TH1;

class BCM_RDOAnalysis : public AthAlgorithm {

 public:
  BCM_RDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~BCM_RDOAnalysis(){}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  // RDO
  std::vector<int>* m_word1;
  std::vector<int>* m_word2;
  std::vector<int>* m_chan;
  std::vector<int>* m_pulse1Pos;
  std::vector<int>* m_pulse1Width;
  std::vector<int>* m_pulse2Pos;
  std::vector<int>* m_pulse2Width;
  std::vector<int>* m_LVL1A;
  std::vector<int>* m_BCID;
  std::vector<int>* m_LVL1ID;
  std::vector<int>* m_err;
  // SDO
  std::vector<unsigned long long>* m_sdoID;
  std::vector<int>* m_sdoWord;
  std::vector<int>* m_barcode;
  std::vector<int>* m_eventIndex;
  std::vector<float>* m_charge;
  std::vector< std::vector<int> >* m_barcode_vec;
  std::vector< std::vector<int> >* m_eventIndex_vec;
  std::vector< std::vector<float> >* m_charge_vec;

  // HISTOGRAMS
  TH1* h_word1;
  TH1* h_word2;
  TH1* h_chan;
  TH1* h_pulse1Pos;
  TH1* h_pulse1Width;
  TH1* h_pulse2Pos;
  TH1* h_pulse2Width;

  TH1* h_sdoID;
  TH1* h_sdoWord;
  TH1* h_barcode;
  TH1* h_eventIndex;
  TH1* h_charge;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
};

#endif // BCM_RDO_ANALYSIS_H
