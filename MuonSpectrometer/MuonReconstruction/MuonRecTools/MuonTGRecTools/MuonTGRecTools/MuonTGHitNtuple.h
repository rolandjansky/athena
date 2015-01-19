/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGRECTOOLS_MUONTGHITNTUPLE_H
#define MUONTGRECTOOLS_MUONTGHITNTUPLE_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataLink.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "GaudiKernel/ITHistSvc.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"
#include "MuonHolesOnTrackTool.h"
#include "GeneratorObjects/McEventCollection.h"

// Amg
#include "EventPrimitives/EventPrimitives.h"

#include <string.h>

class StoreGateSvc;
class TFile;
class TDirectory;
class TTree;
class MdtHitIdHelper;
class RpcHitIdHelper;
class RPCSimHit;
class CscHitIdHelper;
class CSCSimHit;
class TgcHitIdHelper;
class TGCSimHit;

namespace HepPDT
{
  class ParticleDataTable;
}

namespace Muon{

// limits
#define maxSim 100
#define maxHit 1000
#define maxRec 100


class MuonTGHitNtuple : public AthAlgorithm
{
 public:

  /** Constructor  */
  MuonTGHitNtuple(const std::string &name, ISvcLocator *pSvcLocator);


  /** Virtual destructor */
  virtual ~MuonTGHitNtuple() {};

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
   
 private:

  void fillFatras() const;
  void fillSimNtuple() const;
  void fillRecNtuple(const TrackCollection*) const;
  void fillHoles(const TrackCollection*) const;
  
  const TrackCollection* holesFromSim() const;
  const Trk::TrackStateOnSurface* createHole(const Trk::TrackParameters*,const Trk::Layer*, bool) const;
  bool layerMatch(Identifier id1, Identifier id2) const;
  const std::vector< std::pair<const Trk::Layer*, std::vector<Identifier> > >* getOrderedSimLayers(int index, Amg::Vector3D mom) const;
  Identifier getRpcId(const RPCSimHit*) const;
  Identifier getCscId(const CSCSimHit*) const;
  Identifier getTgcId(const TGCSimHit*) const;
  
  // reconstruction id helper
  const MdtIdHelper* m_mdtIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  const CscIdHelper* m_cscIdHelper;

  // simulation id helper
  MdtHitIdHelper* mdtHelper;
  RpcHitIdHelper* rpcHelper;
  CscHitIdHelper* cscHelper;
  TgcHitIdHelper* tgcHelper;


  const MuonGM::MuonDetectorManager* m_muonMgr;

  ToolHandle<Trk::ITrackHoleSearchTool>    m_holesTool;
  ToolHandle<Muon::IMuonTGMeasTool>   m_measTool;
  mutable const Trk::TrackingGeometry* m_trackingGeometry;
  std::string  m_trackingGeometryName;  
  ToolHandle<Trk::IExtrapolator>      m_extrapolator;

  StoreGateSvc                    *m_detStore;       

  ActiveStoreSvc*                  m_activeStore;
  StoreGateSvc                    *m_StoreGate;         //!< transient event store providing the tracks
  const TrackCollection           *m_inputTracks;            
  std::string                      m_inputTrackCollection;
  const TrackCollection           *m_inputFatrasTracks;            
  std::string                      m_inputFatrasTrackCollection;

  std::string                      m_mcEventCollection;
  
  /** Pointer to the particle properties svc */
  ServiceHandle<IPartPropSvc>        m_particlePropSvc;
  /** ParticleDataTable needed to get connection pdg_code <-> charge */
  const HepPDT::ParticleDataTable*   m_particleDataTable;

  bool                             m_processHoles; 
  bool                             m_identifyHoles; 
  bool                             m_processFatras; 
//  Max Nber of Hits in to save
  int m_MaxNberOfHits;

  TFile* m_outFile;
  TTree* m_ptree;
  ITHistSvc*                       m_THistSvc; 
 
  std::string                      m_treeLocation;   
  // Ntuple Variables
  
  // sim tracks
  mutable int m_nSim;
  mutable int m_sim_pdg[maxSim];        // PDG code
  mutable int m_sim_index[maxSim];       // index in event record
  mutable float m_sim_theta[maxSim];
  mutable float m_sim_phi[maxSim];
  mutable float m_sim_mom[maxSim][4];  // momentum+energy
  mutable float m_sim_pos[maxSim][3];       // position
  
  // sim hits
  mutable int m_nSimHit;
  mutable unsigned int m_sim_hit_id[maxHit];
  mutable int m_sim_hit_index[maxHit];      // link to mother particle
  mutable int m_sim_hit_techn[maxHit];      // technology (MDT/RPC/TGC/CSC = 0/1/2/3)
  mutable int m_sim_hit_station_name[maxHit];    // muon station
  mutable int m_sim_hit_station_eta[maxHit];    // muon station
  mutable int m_sim_hit_station_phi[maxHit];    // muon station
  mutable float m_sim_hit_pos[maxHit][3];   // position
  mutable float m_sim_hit_residual[maxHit]; // residual (w.r.t. ideal track)
  mutable int m_sim_hit_match[maxHit];      // match to rec.track
  mutable int m_sim_hole_match[maxHit];     // match to rec.hole
  // this is to adjust the CSC mean energy deposit
  mutable float m_sim_hit_eloss[maxHit];
  mutable float m_sim_hit_length[maxHit];

  // reconstructed tracks
  mutable int m_nRec;
  mutable int m_rec_author[maxRec];
  mutable int m_rec_match[maxRec];              // match with sim track
  mutable float m_rec_theta[maxRec];
  mutable float m_rec_phi[maxRec];
  mutable float m_rec_mom[maxRec];        

  // hits on reconstructed track
  mutable int m_nHit;
  mutable unsigned int m_hit_id[maxHit];
  mutable int m_hit_techn[maxHit];
  mutable int m_hit_match[maxHit];          // match to sim hit 
  mutable int m_hit_track[maxHit];          // reconstructed track index
  mutable int m_hit_station_name[maxHit];    // muon station
  mutable int m_hit_station_eta[maxHit];    // muon station
  mutable int m_hit_station_phi[maxHit];    // muon station

  // holes on reconstructed track
  mutable int m_nHole;
  mutable unsigned int m_hole_id[maxHit];
  mutable int m_hole_techn[maxHit];
  mutable int m_hole_match[maxHit];          // match to sim hit 
  mutable int m_hole_track[maxHit];          // reconstructed track index
  mutable int m_hole_station_name[maxHit];    // muon station
  mutable int m_hole_station_eta[maxHit];    // muon station
  mutable int m_hole_station_phi[maxHit];    // muon station
  mutable float m_hole_residual[maxHit];       // residual for matched holes 

  // fatras sim/tracks

  //mutable std::vector<const Trk::RIO_OnTrack*>  m_sim_rios;     // for visualization only 
  mutable int m_nFSim;
  mutable unsigned int m_fsim_hit_id[maxHit];
  mutable int m_fsim_hit_techn[maxHit];
  mutable int m_fsim_hit_station_name[maxHit];    // muon station
  mutable int m_fsim_hit_station_eta[maxHit];    // muon station
  mutable int m_fsim_hit_station_phi[maxHit];    // muon station
  mutable float m_fsim_hit_pos[maxHit][3];  // position
  mutable float m_fsim_hit_residual[maxHit];       // residual for matched holes 
  mutable int m_fsim_hit_index[maxHit];       // index of parent track 
  // this is to adjust the CSC mean energy deposit
  mutable float m_fsim_hit_eloss[maxHit];
  mutable float m_fsim_hit_length[maxHit];

  mutable int m_nFHit;
  mutable unsigned int m_fhit_id[maxHit];
  mutable int m_fhit_techn[maxHit];
  mutable int m_fhit_station_name[maxHit];    // muon station
  mutable int m_fhit_station_eta[maxHit];    // muon station
  mutable int m_fhit_station_phi[maxHit];    // muon station
  mutable float m_fhit_residual[maxHit];       // residual for matched holes 
   
};
}
#endif     // MUONTGRECTOOLS_MUONTGHITNTUPLE_H


