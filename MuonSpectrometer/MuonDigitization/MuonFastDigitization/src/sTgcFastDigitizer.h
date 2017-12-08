/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_sTgcFASTDIGITIZER_H
#define MUONDIGITIZATION_sTgcFASTDIGITIZER_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandleKey.h"

//Random
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGauss.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

class TTree;
class TFile;

class sTgcIdHelper;
namespace MuonGM {
  class MuonDetectorManager;
}

class IAtRndmGenSvc;
class ActiveStoreSvc;
class MuonSimDataCollection;

class sTgcFastDigitizer : public AthAlgorithm {

 public:

  sTgcFastDigitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~sTgcFastDigitizer();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used

 private:
  const MuonGM::MuonDetectorManager* m_detManager;
  const sTgcIdHelper* m_idHelper;

  int m_channelTypes; // 1 -> strips, 2 -> strips+wires, 3 -> strips/wires/pads

  TFile* m_file;
  //TTree* m_ntuple_SimHit;
  TTree* m_ntuple;
  float  slx; // local position simhit in G4
  float  sly;
  float  slz;
  float  dlx;  //  local position simhit in GeoModel frame
  float  dly;
  float  dlz;
  float  sulx; // local position from REin tracking surface frame
  float  suly;
  float  tsulx; // local position simhit in tracking surface frame
  float  tsuly;
  float  tsulz;
  float  resx; // residuals in local G4 - GeoModel
  float  resy;
  float  resz;
  float  suresx; // residuals g4 - RE stripPosition in tracking frame
  float  suresy;
  float  errx;   // error on x and y
  float  erry;
  float  res;    // residual + pull
  float  pull;
  int    is;   // simulation Identifier: issmall 
  int    seta; // eta
  int    sphi; // phi 
  int    sml; // multi layer
  int    sl; // layer
  int    ss;  // side
  int    stype;  // channel type
  int    ieta; // offline id fields
  int    iphi;
  int    iml;
  int    il;
  int    ich;  // strip number of Id
  int    istr; // strip number from RE
  int    itype; // channel type
  int    ipadeta; // channel type
  int    ipadphi; // channel type
  int    exitcode; // flag reason why prd not made
  int    mode; // flag digitization mode
  int    pdg; // pdg ID
  int    trkid; // track number in G4
  int    bct; // bc tag
  float  tb; // bunchTime
  float  tj; // time jitter
  float  tg4; // globalHitTime in G4
  float  ttof; // tofCorrection assuming IP;
  float  gpx; // global position of the simhit
  float  gpy;
  float  gpz;
  float  gpr; // radial position pos.perp()
  float  gpp; // phi position  pos.phi()
  float  dgpx; // same for readout element center
  float  dgpy;
  float  dgpz;
  float  dgpr;
  float  dgpp;
  float  edep;
  float  e;
  float  as;
  float  at;
  float  surfcentx;
  float  surfcenty;
  float  surfcentz;

  double getResolution(float inAngle_space) const;
  uint16_t bcTagging(float digittime, int channelType) const;
  float timeJitter(float inAngle_time) const;

  /**
     Reads parameters for intrinsic time response from timejitter.dat.
  */
  bool  readFileOfTimeJitter();


 protected:
  ToolHandle <Muon::MuonIdHelperTool> m_idHelperTool;      // IdHelperTool
  ToolHandle <Muon::IMuonClusterOnTrackCreator> m_muonClusterCreator;
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine
  std::string m_inputObjectName; // name of the input objects
  SG::WriteHandleKey<MuonSimDataCollection> m_sdoName; // name of the output SDO collection
  double m_timeWindowOffsetWire;
  double m_timeWindowOffsetStrip;
  double m_timeWindowWire;
  double m_timeWindowStrip;
  double m_bunchCrossingTime;
  double m_energyThreshold;
  double m_energyDepositThreshold;
  bool   m_checkIds;
  std::vector<std::vector<float> > m_vecAngle_Time;

};

#endif // MUONDIGITIZATION_sTgcDIGITIZER_H


