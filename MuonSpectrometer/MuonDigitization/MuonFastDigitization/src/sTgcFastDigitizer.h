/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_sTgcFASTDIGITIZER_H
#define MUONDIGITIZATION_sTgcFASTDIGITIZER_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandleKey.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGauss.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

class TTree;
class TFile;

namespace MuonGM {
  class MuonDetectorManager;
}

class MuonSimDataCollection;

class sTgcFastDigitizer : public AthAlgorithm {

 public:

  sTgcFastDigitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~sTgcFastDigitizer()=default;
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used

 private:
  const MuonGM::MuonDetectorManager* m_detManager;

  int m_channelTypes; // 1 -> strips, 2 -> strips+wires, 3 -> strips/wires/pads

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
  float  m_suresx; // residuals g4 - RE stripPosition in tracking frame
  float  m_suresy;
  float  m_errx;   // error on x and y
  float  m_erry;
  float  m_res;    // residual + pull
  float  m_pull;
  int    m_is;   // simulation Identifier: issmall 
  int    m_seta; // eta
  int    m_sphi; // phi 
  int    m_sml; // multi layer
  int    m_sl; // layer
  int    m_ss;  // side
  int    m_stype;  // channel type
  int    m_ieta; // offline id fields
  int    m_iphi;
  int    m_iml;
  int    m_il;
  int    m_ich;  // strip number of Id
  int    m_istr; // strip number from RE
  int    m_itype; // channel type
  int    m_ipadeta; // channel type
  int    m_ipadphi; // channel type
  int    m_exitcode; // flag reason why prd not made
  int    m_mode; // flag digitization m_mode
  int    m_pdg; // m_pdg ID
  int    m_trkid; // track number in G4
  int    m_bct; // bc tag
  float  m_tb; // bunchTime
  float  m_tj; // time jitter
  float  m_tg4; // globalHitTime in G4
  float  m_ttof; // tofCorrection assuming IP;
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
  float  m_edep;
  float  m_as;
  float  m_at;
  float  m_surfcentx;
  float  m_surfcenty;
  float  m_surfcentz;

  double getResolution(float inAngle_space) const;
  uint16_t bcTagging(float digittime, int channelType) const;
  float timeJitter(float inAngle_time) const;

  /**
     Reads parameters for intrinsic time response from timejitter.dat.
  */
  bool readFileOfTimeJitter();
  bool m_mergePrds;

 protected:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
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


