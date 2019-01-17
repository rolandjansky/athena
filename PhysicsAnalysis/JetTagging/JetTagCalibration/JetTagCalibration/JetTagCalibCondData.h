/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "JetTagCalibration/parse_json.h"
#include "JetTagCalibration/NNLayerConfig.h"
#include "JetTagCalibration/LightweightNeuralNetwork.h"
#include "JetTagCalibration/Exceptions.h"

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
  void resize(const std::vector<std::string> sizeHisto);
  void clear();
  void printAliasesStatus() const; 
  void printHistosStatus() const; 
  std::string getChannelAlias(const std::string& originalChannel) const;
  void addHisto(const unsigned int indexTagger, const std::string& name, TObject *);
  void deleteHistos();
  void addDL1NN(const std::string& tagger, const std::string& channel, const lwt::JSONConfig& );
  void addChannelAlias(const std::string& channel, const std::string& alias);
  TH1* retrieveHistogram(const std::string& folder, const std::string& channel, const std::string& hname) const; 
  lwt::JSONConfig retrieveDL1NN(std::string& tagger, const std::string& channel) const;
  template <class T> T* retrieveTObject(const std::string& folder, const std::string& channel, const std::string& hname) const;
  
  std::string channelName(const std::string& fullHistoName) const;
  std::string histoName(const std::string& fullHistoName) const ;
  std::string fullHistoName(const std::string& channel, const std::string& histoName) const;

private:
  std::vector< std::map<std::string, TObject*> > m_histos;
  std::map< std::string, std::string > m_channelAliasesMap;
  std::vector< std::string> m_taggers;

  //DL1 NN Json config
  std::map< std::string, std::map< std::string, lwt::JSONConfig >> m_DL1_NNConfig;
};
}
CLASS_DEF(Analysis::JetTagCalibCondData, 232300155, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(Analysis::JetTagCalibCondData, 197330919);
#include "JetTagCalibration/JetTagCalibCondData.icc"
#endif // JETTAGCALIBCONDDATA_H
