/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MakePixelTracklets_h
#define MakePixelTracklets_h
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"

#include "HepPDT/ParticleDataTable.hh"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/TileID.h"

#include "VxVertex/RecVertex.h"

#include "TruthHelper/GenAccessIO.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

//forward declaration
class PixelID;
class IBeamCondSvc;
class TTree;
class TH2F;
class TFile;
class TNtuple;
class TH1F;
class CaloDetDescrManager;
class CaloDetDescrElement;

class MakePixelTracklets : public AthAlgorithm {
  
  typedef InDet::PixelClusterContainer PixelClusterContainer;
  typedef InDet::PixelClusterCollection PixelClusterCollection;
  typedef InDet::PixelCluster PixelCluster;
  
 public:
  MakePixelTracklets (const std::string& name, ISvcLocator* pSvcLocator);
  ~MakePixelTracklets ();

  virtual StatusCode initialize();
  
  virtual StatusCode execute();
  
  virtual StatusCode finalize();
  
 private:
  struct ClusterLayer
    : public std::vector<PixelCluster>
  {
    std::map<int, int> used;
  };
  typedef ClusterLayer ClusterLayers_t[3];
  
  void NewVectors();
  void DeleteNewVectors();
  void ClearVectors();
  StatusCode RetrieveCaloClusters();
  void LayerClusters (ClusterLayers_t& clusterLayers);
  void ZVrtCollection(std::vector<double> & zVrts,
                      const Amg::Vector3D& v_ref,
                      const ClusterLayers_t& clusterLayers);
  int ZVrtFinder(std::vector<double> & zVrts);
  void MakeTracklets(bool trkoreff, int sumlayer,
                     ClusterLayers_t& clusterLayers,
                     const std::vector<PixelCluster>& clusterColl0, const std::vector<PixelCluster>& clusterColl1, std::vector<std::vector<PixelCluster> >& outputTracklets,
                     std::map<int,int>& threeLayerTrkMap);
  void RemoveDuplicate(int layer02or12, const std::vector<std::vector<PixelCluster> > &outputTracklets1, const std::vector<std::vector<PixelCluster> > &outputTracklets0, std::vector<std::vector<PixelCluster> > &cleanedTracklets1);
  void GangedOrOverlap(const std::vector<PixelCluster> & tracklet1, const std::vector<PixelCluster> & tracklet2, int & gangeLayer0, int & gangeLayer1, int & overlapLayer0, int & overlapLayer1);
  StatusCode FillNTuple (const ClusterLayers_t& clusterLayers,
                         std::vector<std::vector<PixelCluster> >& trackletsCol,
                         const std::vector<const HepMC::GenParticle*>& primaryGenParts,
                         const std::vector<std::vector<PixelCluster> >& genPartCol,
                         std::map<int,int>& threeLayerTrkMap);
  StatusCode RetrieveTruth (const ClusterLayers_t& clusterLayers,
                            std::vector<const HepMC::GenParticle*>& primaryGenParts,
                            std::vector<const HepMC::GenParticle*>& weakDecayParts,
                            std::vector<std::vector<PixelCluster> >& genPartCol,
                            std::vector<std::vector<PixelCluster> >& weakDecayCol);
  void TruthAssociation (const std::vector<std::vector<PixelCluster> >& trackletsCol,
                         const std::vector<const HepMC::GenParticle*>& primaryGenParts,
                         const std::vector<const HepMC::GenParticle*>& weakDecayParts,
                         const std::vector<std::vector<PixelCluster> >& genPartCol,
                         const std::vector<std::vector<PixelCluster> >& weakDecayCol);
  void CalEfficiency(int layer,
                     ClusterLayers_t& clusterLayers,
                     std::map<int,int>& threeLayerTrkMap);
  StatusCode AnaCaloClusters();
  
 private:
  const PixelID* m_pixelID;
  ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;
  std::string m_PixelClustersName;
  std::string m_multiTruthCollectionPixelName;
  std::string m_CaloClustersContainerName;
  std::string m_pixelTrackletsCollection;
  const CaloDetDescrManager* m_calo_dd_man;
  
  double m_etaCut;
  double m_phiCut;
  double m_etaMatch;
  double m_phiMatch;
  double m_etaMax;
  double m_matchR;
  double m_ptphi;
  double m_vrt_phi_cut;
  double m_vrt_z_cut;

  bool m_doHolesEfficiency;
  
  ServiceHandle<ITHistSvc> m_ths;

  const HepPDT::ParticleDataTable* m_particleDataTable;
  TruthHelper::GenAccessIO* m_tesIO;

 public:

  // histograms etc.
  std::string m_ntupleLoc;

  TNtuple* m_Ntuple;
  
 private:
  bool m_mc_available;
  bool m_doStartingPoint3;
  unsigned int m_event_id_run_number;
  unsigned int m_event_id_evt_number;
  unsigned int m_event_id_lumiBlock;
  unsigned int m_event_id_bc_id;

  //trackelts tree;
  TTree* m_trackletsTree;
  int m_NumVrtRec;
  int m_NumVrtFinder;
  double m_vz_finder;
  double m_vx;
  double m_vy;
  double m_vz;
  double m_sim_vx;
  double m_sim_vy;
  double m_sim_vz;
  int m_ntr;
  int m_ntrF;
  int m_ntrT;
  int m_n0;
  int m_n1;
  int m_n2;

  template <class T>
  struct TupleVec : public std::vector<T> {
    TupleVec() : p (this) {}
    std::vector<T>* p;
  };
  TupleVec<int> m_matched; //tracklets matched to truth or not, or how many times it matched
  //0 not matched at all, fakes, 1 matched once, 2 matched twice
  TupleVec<int> m_matchedNumber; //which mc particle it matched to
  TupleVec<int> m_matchedToWeakDecay; //1 is true
  TupleVec<int> m_startingPoint; //tracklets starting point, layer01 or layer12 or layer12,it's a sum of two layer number
  TupleVec<int> m_threeLayerTrk; //is this a three layer tracklets
  TupleVec<int> m_deleted; //tracklets labled of deleted because of duplicate
  TupleVec<int> m_deletedBecauseOf; //deleted because of duplicate which track number
  TupleVec<double> m_pt_tr;
  TupleVec<double> m_p_tr;
  TupleVec<double> m_px_tr;
  TupleVec<double> m_py_tr;
  TupleVec<double> m_pz_tr;
  //first layer cluster parameters
  TupleVec<double> m_eta0_tr;
  TupleVec<double> m_phi0_tr;
  //second layer cluster parameters
  TupleVec<double> m_eta1_tr;
  TupleVec<double> m_phi1_tr;
  //
  TupleVec<double> m_dedx0_tr;
  TupleVec<double> m_dedx1_tr;
  TupleVec<double> m_pathLength0_tr;
  TupleVec<double> m_pathLength1_tr;
  //
  TupleVec<int> m_csize0_tr;
  TupleVec<int> m_csize1_tr;
  TupleVec<double> m_ccol0_tr;
  TupleVec<double> m_ccol1_tr;
  TupleVec<double> m_crow0_tr;
  TupleVec<double> m_crow1_tr;
  TupleVec<double> m_crphi0_tr;
  TupleVec<double> m_crphi1_tr;
  TupleVec<double> m_cz0_tr;
  TupleVec<double> m_cz1_tr;
  
  //first layer for flip
  TupleVec<double> m_eta0F_tr;
  TupleVec<double> m_phi0F_tr;
  //second layer flip
  TupleVec<double> m_eta1F_tr;
  TupleVec<double> m_phi1F_tr;
  //difference between layer1 and layer 0
  TupleVec<double> m_deta_tr;
  TupleVec<double> m_dphi_tr;
  //difference between layer 1 and layer 0: flipper
  TupleVec<double> m_detaF_tr;
  TupleVec<double> m_dphiF_tr;
  //
  TupleVec<int> m_partid_tr;
  TupleVec<int> m_parentid_tr;
  //sometimes for one cluster there will be multip satisfying the cut
  TupleVec<int> m_multipTrks;
  //parameters for all the clusters in layer 0 and 1
  TupleVec<double> m_eta0_all;
  TupleVec<double> m_phi0_all;
  TupleVec<double> m_eta1_all;
  TupleVec<double> m_phi1_all;
  TupleVec<double> m_eta2_all;
  TupleVec<double> m_phi2_all;
  TupleVec<double> m_dedx0_all;
  TupleVec<double> m_dedx1_all;
  TupleVec<double> m_dedx2_all;
  TupleVec<double> m_pathLength0_all;
  TupleVec<double> m_pathLength1_all;
  TupleVec<double> m_pathLength2_all;
  TupleVec<int> m_csize0_all;
  TupleVec<int> m_csize1_all;
  TupleVec<double> m_ccol0_all;
  TupleVec<double> m_ccol1_all;
  TupleVec<double> m_crow0_all;
  TupleVec<double> m_crow1_all;
  TupleVec<double> m_crphi0_all;
  TupleVec<double> m_crphi1_all;
  TupleVec<double> m_cz0_all;
  TupleVec<double> m_cz1_all;
  
  //truth information
  TupleVec<double> m_etaT;
  TupleVec<double> m_phiT;
  TupleVec<double> m_ptT;
  TupleVec<double> m_pT;
  TupleVec<double> m_pxT;
  TupleVec<double> m_pyT;
  TupleVec<double> m_pzT;
  //ganged flag for layer 1 and layer 2 clusters within a tracklet
  TupleVec<int> m_ganged0_tr;
  TupleVec<int> m_ganged1_tr;

  TupleVec<double> m_x0_all;
  TupleVec<double> m_y0_all;
  TupleVec<double> m_z0_all;
  TupleVec<double> m_x1_all;
  TupleVec<double> m_y1_all;
  TupleVec<double> m_z1_all;
  TupleVec<double> m_x0_tr;
  TupleVec<double> m_y0_tr;
  TupleVec<double> m_z0_tr;
  TupleVec<double> m_x1_tr;
  TupleVec<double> m_y1_tr;
  TupleVec<double> m_z1_tr;
  
  TupleVec<double> m_x0_true;
  TupleVec<double> m_y0_true;
  TupleVec<double> m_z0_true;
  TupleVec<double> m_x1_true;
  TupleVec<double> m_y1_true;
  TupleVec<double> m_z1_true;
  TupleVec<double> m_eta0_true;
  TupleVec<double> m_phi0_true;
  TupleVec<double> m_eta1_true;
  TupleVec<double> m_phi1_true;
  TupleVec<double> m_deta_true;
  TupleVec<double> m_dphi_true;
  //TupleVec<int> m_pdgid;
  TupleVec<int> m_partTid; //truth particle pdgid
  TupleVec<int> m_nparentT;
  TupleVec<int> m_parentTid; //truth particle parent pdgid
  TupleVec<double> m_prod_vrtxT;
  TupleVec<double> m_prod_vrtyT;
  TupleVec<double> m_prod_vrtzT;

  //unassociated hits information
  TupleVec<int> m_unassociated0[3]; //0 stage
  TupleVec<int> m_unassociated1[3]; //1 stage
  TupleVec<int> m_unassociated2[3]; //2 stage
  

  //parameters for calorimeter
  int m_nCalo;
  TupleVec<double> m_eta_ca;
  TupleVec<double> m_phi_ca;
  TupleVec<double> m_ene_ca;

  TupleVec<int> m_nPixelClustersT; //how many clusters do truth particle have in the first two layers
  TupleVec<int> m_nBLayerClustersT;
  TupleVec<int> m_primaryDecayLabel; //label this track is really from primary or from others

  //efficiency tree 0;
  TTree* m_effTree0;
  TTree* m_effTree1;
  TupleVec<int> m_projected_eff;
  TupleVec<double> m_phi_eff;
  TupleVec<double> m_eta_eff;
  TupleVec<double> m_pt_eff;

#if 0
  std::vector<std::vector<int> > deadLayer0;
  std::vector<std::vector<int> > deadLayer1;
#endif
};

#endif
