/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  float  m_dlx;  //  local position simhit in GeoModel frame
  float  m_dly;
  float  m_dlz;
  float  m_sulx; // local position from REin tracking surface frame
  float  m_suly;
  float  m_tsulx; // local position simhit in tracking surface frame
  float  m_tsuly;
  float  m_tsulz;
  float  m_stsulx; // local position simhit in tracking surface frame
  float  m_stsuly;
  float  m_stsulz;
  float  m_ang; // local angel
  float  m_shift; // m_shift due to bunch offset 
  float  m_suresx; // residuals g4 - RE stripPosition in tracking frame
  float  m_suresy;
  float  m_err;    // error
  float  m_res;    // residual + pull
  float  m_pull;
  int    m_is;   // simulation Identifier: issmall 
  int    m_seta; // eta
  int    m_sphi; // phi 
  int    m_sml; // multi layer
  int    m_sl; // layer
  int    m_ss;  // strip
  int    m_ieta; // offline id fields
  int    m_iphi;
  int    m_iml;
  int    m_il;
  int    m_ich;  // strip number of Id
  int    m_istr; // strip number from RE
  int    m_exitcode; // flag reason why prd not made
  int    m_mode; // flag digitization strategy
  int    m_pdg; // m_pdg ID
  int    m_trkid; // track number in G4
  float  m_gpx; // global position of the simhit
  float  m_gpy;
  float  m_gpz;
  float  m_gpr; // radial position pos.perp()
  float  m_gpp; // phi position  pos.phi()
  float  m_dgpx; // same for readout element center
  float  m_dgpy;
  float  m_dgpz;
  float  m_dgpr;
  float  m_dgpp;
  float  m_tofCorrection;
  float  m_bunchTime;
  float  m_globalHitTime;
  float  m_e;
  float  m_edep;
  float  m_surfcentx;
  float  m_surfcenty;
  float  m_surfcentz;

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
  bool   m_microTPC;
};

#endif // MUONDIGITIZATION_MM_DIGITIZER_H


