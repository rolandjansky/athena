/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGCALIBRATION_JETTAGCALIBCONDALG_H
#define JETTAGCALIBRATION_JETTAGCALIBCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "JetTagCalibration/JetTagCalibCondData.h"

#include "TFile.h"
#include <map>
#include <string>
#include <utility>
#include <set>

class TH2;
class TObject;
class IPoolSvc;

namespace Analysis {

  /**
  @ class JetTagCalibCondAlg

  This class retrieves calibration histograms from COOL for the taggers.
  A single COOL folder is defined for all the taggers.
  Each tagger is subdivided in channels, each channel being associated to a jet type
   (Cone4Topo, Kt6, etc).

  Any further substructure (track categories, signal/background) is hidden to COOL 
  and is defined by the actual name of the histogram.

  This class holds the collection of maps between the histogram name and the pointer 
  to the histogram, the latter being kept up-to-date with the condition algorithm mecanism.
  All the taggers histograms are registered in initialize() method with the 
  registerHistogram() method. 

  The geometry tag corresponding to the calibrations, the type of physics events used, 
  the tracking algorithm, etc, are all defined by a DB tag.

  @author emmanuel.le.guirriec@cern.ch
  */

class JetTagCalibCondAlg : public AthAlgorithm {

 public:

  JetTagCalibCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~JetTagCalibCondAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  void registerHistogram(const std::string& tagger, const std::string& histoname);
  void registerHistogram(const std::string& tagger, const std::string& directory, const std::string& histoname);
  StatusCode createHistoMap(TFile* file, JetTagCalibCondData * histos);
  StatusCode objectTDirExists(const std::string& histname, TFile* file) const;
  std::vector<std::string> tokenize(const std::string& str, const std::string& delim);
  void smoothAndNormalizeHistogram(TH1* h, const std::string& hname);
  void smoothASH2D(TH2* input2D, int m1, int m2); 
  void initializeIPTag();
  void initializeIP2D();
  void initializeIP3D();
  void initializeSVEff(const std::string&);
  void initializeSV1();
  void initializeSV2();
  void initializeJetFitterNN();
  void initializeSoftMu();
  void initializeMV2(const std::string&);
  void initializeRNNIP();
  void initializeJetVertexCharge();
  void initializeMultiSV(const std::string&);
  void initializeDL1(const std::string&);

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKeyCalibPath", "/GLOBAL/BTagCalib/RUN12", "Key of input (raw) conditions folder of bration path"}; //   /GLOBAL/BTagCalib/Onl/RUN12 online, /GLOBAL/BTagCalib/RUN12 offline
  SG::WriteCondHandleKey<JetTagCalibCondData> m_writeKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of output (derived) JetTag calibration data"};

  ServiceHandle<ICondSvc> m_condSvc;
  IPoolSvc* m_poolsvc;


  std::vector< std::string > m_taggers;
  std::unordered_map<std::string,std::string> m_directoryMap;
  std::vector<std::vector<std::string> > m_taggersHists;
  std::vector< std::string > m_mappedAlias;
  std::vector< std::string > m_originalChannels;
  /* aliases for channels: 
  * to specifiy an alias, enter a string "channelA->channelAA" (or a list of strings)
  * this will force to use channelAA from the DB wherever channelA was mentioned */
  StringArrayProperty m_channelAliases;
  std::map< std::string, std::vector<std::string> >  m_channelAliasesMultiMap;

  //for IPTag
  bool m_IPTag_useCHypo;
  std::vector<std::string> m_IPTag_hypotheses; // hypotheses: b | u

  //for IP2DTag
  std::vector< std::string > m_IP2D_trackGradePartitions; //could be defined for each tagger

  //for SV1Tag
  bool  m_useDRJPVSV;

  //for SV2Tag
  bool m_usePtSV2;

  //for Likelihood tool
  int m_Likelihood_smoothNTimes;
  std::vector<std::string> m_Likelihood_vetoSmoothingOf;

  //for JetFitterNNTag
  std::string m_JetFitterNN_calibrationDirectory;
  std::string m_JetFitterNN_calibrationSubDirectory;
  bool m_JetFitterNN_useCombinedIPNN;
  int m_JetFitterNN_maximumRegisteredLayers;

  //for DL1Tag
  std::string m_DL1_file_name; // directory of DL1 NN calibration (starting from specific jet collection directory) in COOL db

  //for RNNIP
  // Initalize with a config name and path to a network If the
  // network path is empty we read from the calib db Note that this
  // won't support reading different local files for different jet
  // collections.
  std::map<std::string, std::string > m_RNNIP_network_cfg;

  
};
}
#endif // JETTAGCALIBRATION_JETTAGCALIBCONDALG_H
