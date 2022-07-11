/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARFLATFROMFILE_H
#define LARFLATFROMFILE_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArOnlineID;
class LArOnline_SuperCellID;

class LArFlatFromFile: public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  /// Constructor with parameters: 
  LArFlatFromFile( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArFlatFromFile()=default; 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override {return StatusCode::SUCCESS;}
  virtual StatusCode  finalize() override {return StatusCode::SUCCESS;}
  virtual StatusCode  stop() override;

 private: 
  /// Default constructor: 
  LArFlatFromFile();


  void singleFloatFlat(const char* blobName, const std::string& input, 
					  const std::string& outputName, const unsigned nGain);
  void ofcFlat(const std::string& input, const std::string& outputName);


  void errIfConnected(const HWIdentifier chid, const int gain, const std::string& objName, const std::string& message=0) const;

  unsigned m_hashMax{};
  const LArOnlineID* m_onlineID{};
  const LArOnline_SuperCellID* m_onlineSCID{};


  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  ///Input files
  Gaudi::Property<std::string> m_OFCInput{this, "OFCInput","","Input file name"};

  // SC switch
  Gaudi::Property<bool> m_isSC{this,"SuperCells",false,"Run for SC ?"};
  Gaudi::Property<unsigned> m_ngain{this,"NGains",1,"Number of gains to process"};
  Gaudi::Property<unsigned> m_nsample{this,"NSamples",4,"Number of samples to process"};

  bool m_forceStop{};
}; 

#endif
