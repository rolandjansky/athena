/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_MM_FASTDIGITIZER_H
#define MUONDIGITIZATION_MM_FASTDIGITIZER_H 

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

class MmIdHelper;
namespace MuonGM {
  class MuonDetectorManager;
}

//Random
namespace CLHEP{
  class HepRandomEngine;
}


class IAtRndmGenSvc;
class ActiveStoreSvc;
class MuonSimDataCollection;

class MM_FastDigitizer : public AthAlgorithm {

 public:

  MM_FastDigitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~MM_FastDigitizer();
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used
  
  float RadsToDegrees(float Radians);
 
 private:

  ActiveStoreSvc*             m_activeStore;

  const MuonGM::MuonDetectorManager* m_detManager;
  const MmIdHelper* m_idHelper;
  
  TFile* m_file;
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
  float  stsulx; // local position simhit in tracking surface frame
  float  stsuly;
  float  stsulz;
  float  ang; // local angel
  float  shift; // shift due to bunch offset 
  float  resx; // residuals in local G4 - GeoModel
  float  resy;
  float  resz;
  float  suresx; // residuals g4 - RE stripPosition in tracking frame
  float  suresy;
  float  err;    // error
  float  res;    // residual + pull
  float  pull;
  int    is;   // simulation Identifier: issmall 
  int    seta; // eta
  int    sphi; // phi 
  int    sml; // multi layer
  int    sl; // layer
  int    ss;  // strip
  int    ieta; // offline id fields
  int    iphi;
  int    iml;
  int    il;
  int    ich;  // strip number of Id
  int    istr; // strip number from RE
  int    exitcode; // flag reason why prd not made
  int    mode; // flag digitization strategy
  int    pdg; // pdg ID
  int    trkid; // track number in G4
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
  float  tofCorrection;
  float  bunchTime;
  float  globalHitTime;
  float  e;
  float  edep;
  float  surfcentx;
  float  surfcenty;
  float  surfcentz;

 protected:
  ToolHandle <Muon::MuonIdHelperTool> m_idHelperTool;  
  ToolHandle <Muon::IMuonClusterOnTrackCreator> m_muonClusterCreator;
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine
  std::string m_inputObjectName; // name of the input objects
  SG::WriteHandleKey<MuonSimDataCollection> m_sdoName; // name of the output SDO collection
  bool   m_useTimeShift;
  double m_energyThreshold;
  bool   m_checkIds;
  int    m_maskMultiplet;
};

#endif // MUONDIGITIZATION_MM_DIGITIZER_H


