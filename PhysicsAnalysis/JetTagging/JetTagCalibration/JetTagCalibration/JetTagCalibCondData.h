/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagCalibCondData.h
 * @author emmanuel.le.guirriec@cern.ch
**/

#ifndef JETTAGCALIBCONDDATA_H
#define JETTAGCALIBCONDDATA_H

#include <vector>
#include <map>
#include "AthenaKernel/CLASS_DEF.h"
#include "lwtnn/parse_json.hh"
#include "lwtnn/NNLayerConfig.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/Exceptions.hh"
#include "MVAUtils/BDT.h"

class TObject;
class TH1;

/**
  * @class JetTagCalibCondData
  * Concrete class for JetTagging calibration condition data
**/

namespace lwt {
  class LightweightNeuralNetwork;
}

namespace Analysis {

class JetTagCalibCondData {
public:
  // Constructor
  JetTagCalibCondData();
  // Destructor
  ~JetTagCalibCondData();

  /** Resize the data members */
  void resize(const std::vector<std::string>& sizeHisto);
  void clear();
  void printAliasesStatus() const; 
  void printHistosStatus() const; 
  void printBdtsStatus() const;
  std::string getChannelAlias(const std::string& originalChannel) const;
  void addHisto(const unsigned int indexTagger, const std::string& name, TH1*);
  void deleteHistos();
  void deleteBdts();
  void addDL1NN(const std::string& tagger, const std::string& channel, const lwt::JSONConfig& );
  void addIPRNN(const std::string& tagger, const std::string& channel, const std::string& );
  void addBdt(const std::string& tagger, const std::string& name, MVAUtils::BDT *);
  void addInputVars(const std::string& tagger, const std::string& name, const std::vector<std::string> &input);
  void addChannelAlias(const std::string& channel, const std::string& alias);
  TH1* retrieveHistogram(const std::string& folder, const std::string& channel, const std::string& hname) const; 
  lwt::JSONConfig retrieveDL1NN(const std::string& tagger, const std::string& channel) const;
  std::string retrieveIPRNN(const std::string& tagger, const std::string& channel) const;
  MVAUtils::BDT* retrieveBdt(const std::string& tagger, const std::string& channel) const;
  std::vector<std::string> retrieveInputVars(const std::string& tagger, const std::string& channel, const std::string& hname) const;
  template <class T> T* retrieveTObject(const std::string& folder, const std::string& channel, const std::string& hname) const;
  
  std::string channelName(const std::string& fullHistoName) const;
  std::string histoName(const std::string& fullHistoName) const ;
  std::string fullHistoName(const std::string& channel, const std::string& histoName) const;

private:
  std::vector< std::map<std::string, TH1*> > m_histos;
  std::map< std::string, std::string > m_channelAliasesMap;
  std::vector< std::string> m_taggers;

  //MV2, MultiSV and SoftMuon BDTs
  std::map< std::string, std::map<std::string, MVAUtils::BDT*> > m_bdts;
  //MV2, MultiSV and SoftMuon input var
  std::map< std::string, std::map<std::string, std::vector<std::string>>> m_inputVars;
  //DL1 NN Json config
  std::map< std::string, std::map<std::string, lwt::JSONConfig >> m_DL1_NNConfig;
  //RNNIP string config
  std::map< std::string, std::map<std::string, std::string >> m_IP_RNNConfig;
};
}
CLASS_DEF(Analysis::JetTagCalibCondData, 232300155, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(Analysis::JetTagCalibCondData, 197330919);
#include "JetTagCalibration/JetTagCalibCondData.icc"
#endif // JETTAGCALIBCONDDATA_H
