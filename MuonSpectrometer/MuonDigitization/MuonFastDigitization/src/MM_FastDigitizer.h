/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_MM_FASTDIGITIZER_H
#define MUONDIGITIZATION_MM_FASTDIGITIZER_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CLHEP/Random/RandomEngine.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <string>

namespace CLHEP {
  class HepRandomEngine;
}


class TTree;
class TFile;

class MM_FastDigitizer : public AthAlgorithm {

 public:

  MM_FastDigitizer(const std::string& name, ISvcLocator* pSvcLocator);
  ~MM_FastDigitizer()=default;
    
  StatusCode initialize()override;
  StatusCode execute()override;
  StatusCode finalize() override;  
  unsigned int cardinality() const override {return 1;}

  float RadsToDegrees(float Radians) const;
 
 private:
  CLHEP::HepRandomEngine* getRandomEngine(const std::string& streamName, const EventContext& ctx) const;
    
  TFile* m_file{nullptr};
  TTree* m_ntuple{nullptr};
  float  m_dlx{0.};  //  local position simhit in GeoModel frame
  float  m_dly{0.};
  float  m_dlz{0.};
  float  m_sulx{0.}; // local position from REin tracking surface frame
  float  m_suly{0.};
  float  m_tsulx{0.}; // local position simhit in tracking surface frame
  float  m_tsuly{0.};
  float  m_tsulz{0.};
  float  m_stsulx{0.}; // local position simhit in tracking surface frame
  float  m_stsuly{0.};
  float  m_stsulz{0.};
  float  m_ang{0.}; // local angel
  float  m_shift{0.}; // m_shift due to bunch offset 
  float  m_suresx{0.}; // residuals g4 - RE stripPosition in tracking frame
  float  m_suresy{0.};
  float  m_err{0.};    // error
  float  m_res{0.};    // residual + pull
  float  m_pull{0.};
  int    m_is{0};   // simulation Identifier: issmall 
  int    m_seta{0}; // eta
  int    m_sphi{0}; // phi 
  int    m_sml{0}; // multi layer
  int    m_sl{0}; // layer
  int    m_ss{0};  // strip
  int    m_ieta{0}; // offline id fields
  int    m_iphi{0};
  int    m_iml{0};
  int    m_il{0};
  int    m_ich{0};  // strip number of Id
  int    m_istr{0}; // strip number from RE
  int    m_exitcode{0}; // flag reason why prd not made
  int    m_mode{0}; // flag digitization strategy
  int    m_pdg{0}; // m_pdg ID
  int    m_trkid{0}; // track number in G4
  float  m_gpx{0.}; // global position of the simhit
  float  m_gpy{0.};
  float  m_gpz{0.};
  float  m_gpr{0.}; // radial position pos.perp()
  float  m_gpp{0.}; // phi position  pos.phi()
  float  m_dgpx{0.}; // same for readout element center
  float  m_dgpy{0.};
  float  m_dgpz{0.};
  float  m_dgpr{0.};
  float  m_dgpp{0.};
  float  m_tofCorrection{0.};
  float  m_bunchTime{0.};
  float  m_globalHitTime{0.};
  float  m_e{0.};
  float  m_edep{0.};
  float  m_surfcentx{0.};
  float  m_surfcenty{0.};
  float  m_surfcentz{0.};

 protected:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<Muon::IMuonClusterOnTrackCreator> m_muonClusterCreator{this,"ClusterCreator","Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"};
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service used in Muon digitization"};
  
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};
  

  
  Gaudi::Property<std::string> m_rndmEngineName{this, "RndmEngine", "" , "Random engine name"};// name of random engine
  SG::ReadHandleKey<MMSimHitCollection> m_inputObjectName{this, "InputObjectName", "MM_Hits", "name of the input object"}; // name of the input objects
  
  
  SG::WriteHandleKey<MuonSimDataCollection> m_sdoName{this, "SDOname", "MMfast_SDO"}; // name of the output SDO collection
  SG::WriteHandleKey<Muon::MMPrepDataContainer> m_prepDataKey{this, "PrdKey", "MM_Measurements"} ;
  Gaudi::Property<bool>   m_useTimeShift{this, "UseTimeShift", true, "Use time shift"};
  Gaudi::Property<double> m_energyThreshold{this, "EnergyThreshold", 50., "Minimal energy to produce a PRD" };
  Gaudi::Property<bool>   m_checkIds{this, "CheckIds", false, "Turn on validity checking of Identifiers"};
  Gaudi::Property<int>    m_maskMultiplet{this, "MaskMultiplet",0, "0: all, 1: first, 2: second, 3: both" };
  Gaudi::Property<bool>   m_microTPC{this, "MicroTPC", true, "Turn on microTPC mode"};
 
};

#endif // MUONDIGITIZATION_MM_DIGITIZER_H


