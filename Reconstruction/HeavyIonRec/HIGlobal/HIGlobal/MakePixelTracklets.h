/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MakePixelTracklets_h
#define MakePixelTracklets_h
#include <string>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/DataHandle.h"

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
class StoreGateSvc;
class CaloDetDescrManager;
class CaloDetDescrElement;

class MakePixelTracklets : public Algorithm {
  
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
  
  void NewVectors();
  void DeleteNewVectors();
  void ClearVectors();
  StatusCode RetrievePixelClusters();
  StatusCode RetrieveCaloClusters();
  void LayerClusters();
  void ZVrtCollection(std::vector<double> & zVrts);
  int ZVrtFinder(std::vector<double> & zVrts);
  void MakeTracklets(bool trkoreff, int sumlayer, const std::vector<PixelCluster>& clusterColl0, const std::vector<PixelCluster>& clusterColl1, std::vector<std::vector<PixelCluster> >& outputTracklets);
  void RemoveDuplicate(int layer02or12, const std::vector<std::vector<PixelCluster> > &outputTracklets1, const std::vector<std::vector<PixelCluster> > &outputTracklets0, std::vector<std::vector<PixelCluster> > &cleanedTracklets1);
  void GangedOrOverlap(const std::vector<PixelCluster> & tracklet1, const std::vector<PixelCluster> & tracklet2, int & gangeLayer0, int & gangeLayer1, int & overlapLayer0, int & overlapLayer1);
  StatusCode FillNTuple();
  StatusCode RetrieveTruth();
  void TruthAssociation();
  void CalEfficiency(int layer);
  void AnaCaloClusters();
  
 private:
  const PixelID* pixelID;   
  StoreGateSvc *detStore;
  ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;
  std::string m_PixelClustersName;
  std::string m_multiTruthCollectionPixelName;
  std::string m_CaloClustersContainerName;
  std::string m_pixelTrackletsCollection;
  const CaloIdManager* m_caloMgr;
  const TileID*        m_tileID;
  const CaloCell_ID*   m_caloCellHelper;
  const CaloDetDescrManager* m_calo_dd_man;
  
  Trk::RecVertex thisVx;
  Trk::RecVertex beamposition;
  double vx_ref;
  double vy_ref;
  
  double m_etaCut;
  double m_phiCut;
  double m_etaMatch;
  double m_phiMatch;
  double m_etaMax;
  double m_matchR;
  double m_ptphi;
  double vrt_phi_cut;
  double vrt_z_cut;

  bool m_doHolesEfficiency;
  
  StoreGateSvc* m_StoreGate; 
  ITHistSvc* m_ths;
  
  const HepPDT::ParticleDataTable* m_particleDataTable;
  const DataHandle<PixelClusterContainer> m_clusters;
  const PRD_MultiTruthCollection *m_truthCollectionPixel;
  const CaloClusterContainer* m_clusterCont;
  
  int m_eventNumber;
  int m_numberOfEvents;

  TruthHelper::GenAccessIO* m_tesIO;

 public:

  // histograms etc.
  std::string m_ntupleLoc;

  TNtuple* m_Ntuple;
  
 private:
  bool m_mc_available;
  bool m_doStartingPoint3;
  int multisatisfaction;
  unsigned int event_id_run_number;
  unsigned int event_id_evt_number;
  unsigned int event_id_lumiBlock;
  unsigned int event_id_bc_id;

  //trackelts tree;
  TTree* trackletsTree;
  int NumVrtRec;
  int NumVrtFinder;
  double vz_finder;
  double vz;
  double vx;
  double vy;
  double sim_vx;
  double sim_vy;
  double sim_vz;
  int ntr;
  int ntrF;
  int ntrT;
  int n0;
  int n1;
  int n2;
  std::vector<int> * matched; //tracklets matched to truth or not, or how many times it matched
  //0 not matched at all, fakes, 1 matched once, 2 matched twice
  std::vector<int> * matchedNumber; //which mc particle it matched to
  std::vector<int> * matchedToWeakDecay; //1 is true
  std::vector<int> * startingPoint; //tracklets starting point, layer01 or layer12 or layer12,it's a sum of two layer number
  std::vector<int> * threeLayerTrk; //is this a three layer tracklets
  std::vector<int> * deleted; //tracklets labled of deleted because of duplicate
  std::vector<int> * deletedBecauseOf; //deleted because of duplicate which track number
  std::vector<double> * pt_tr;
  std::vector<double> * p_tr;
  std::vector<double> * px_tr;
  std::vector<double> * py_tr;
  std::vector<double> * pz_tr;
  std::vector<double> * ptF_tr;
  std::vector<double> * pF_tr;
  //first layer cluster parameters
  std::vector<double> * eta0_tr;
  std::vector<double> * phi0_tr;
  //second layer cluster parameters
  std::vector<double> * eta1_tr;
  std::vector<double> * phi1_tr;
  //
  std::vector<double> * dedx0_tr;
  std::vector<double> * dedx1_tr;
  std::vector<double> * pathLength0_tr;
  std::vector<double> * pathLength1_tr;
  //
  std::vector<int> * csize0_tr;
  std::vector<int> * csize1_tr;
  std::vector<double> * ccol0_tr;
  std::vector<double> * ccol1_tr;
  std::vector<double> * crow0_tr;
  std::vector<double> * crow1_tr;
  std::vector<double> * crphi0_tr;
  std::vector<double> * crphi1_tr;
  std::vector<double> * cz0_tr;
  std::vector<double> * cz1_tr;
  
  //first layer for flip
  std::vector<double> * eta0F_tr;
  std::vector<double> * phi0F_tr;
  //second layer flip
  std::vector<double> * eta1F_tr;
  std::vector<double> * phi1F_tr;
  //difference between layer1 and layer 0
  std::vector<double> * deta_tr;
  std::vector<double> * dphi_tr;
  //difference between layer 1 and layer 0: flipper
  std::vector<double> * detaF_tr;
  std::vector<double> * dphiF_tr;
  //
  std::vector<int> * partid_tr;
  std::vector<int> * parentid_tr;
  //sometimes for one cluster there will be multip satisfying the cut
  std::vector<int> * multipTrks;
  //parameters for all the clusters in layer 0 and 1
  std::vector<double> * eta0_all;
  std::vector<double> * phi0_all;
  std::vector<double> * eta1_all;
  std::vector<double> * phi1_all;
  std::vector<double> * eta2_all;
  std::vector<double> * phi2_all;
  std::vector<double> * dedx0_all;
  std::vector<double> * dedx1_all;
  std::vector<double> * dedx2_all;
  std::vector<double> * pathLength0_all;
  std::vector<double> * pathLength1_all;
  std::vector<double> * pathLength2_all;
  std::vector<int> * csize0_all;
  std::vector<int> * csize1_all;
  std::vector<double> * ccol0_all;
  std::vector<double> * ccol1_all;
  std::vector<double> * crow0_all;
  std::vector<double> * crow1_all;
  std::vector<double> * crphi0_all;
  std::vector<double> * crphi1_all;
  std::vector<double> * cz0_all;
  std::vector<double> * cz1_all;
  
  //truth information
  std::vector<double> * etaT;
  std::vector<double> * phiT;
  std::vector<double> * ptT;
  std::vector<double> * pT;
  std::vector<double> * pxT;
  std::vector<double> * pyT;
  std::vector<double> * pzT;
  //ganged flag for layer 1 and layer 2 clusters within a tracklet
  std::vector<int> * ganged0_tr;
  std::vector<int> * ganged1_tr;

  std::vector<double> * x0_all;
  std::vector<double> * y0_all;
  std::vector<double> * z0_all;
  std::vector<double> * x1_all;
  std::vector<double> * y1_all;
  std::vector<double> * z1_all;
  std::vector<double> * x0_tr;
  std::vector<double> * y0_tr;
  std::vector<double> * z0_tr;
  std::vector<double> * x1_tr;
  std::vector<double> * y1_tr;
  std::vector<double> * z1_tr;
  
  std::vector<double> * x0_true;
  std::vector<double> * y0_true;
  std::vector<double> * z0_true;
  std::vector<double> * x1_true;
  std::vector<double> * y1_true;
  std::vector<double> * z1_true;
  std::vector<double> * eta0_true;
  std::vector<double> * phi0_true;
  std::vector<double> * eta1_true;
  std::vector<double> * phi1_true;
  std::vector<double> * deta_true;
  std::vector<double> * dphi_true;
  //std::vector<int> * pdgid;
  std::vector<int> * partTid; //truth particle pdgid
  std::vector<int> * nparentT;
  std::vector<int> * parentTid; //truth particle parent pdgid
  std::vector<double> * prod_vrtxT;
  std::vector<double> * prod_vrtyT;
  std::vector<double> * prod_vrtzT;

  //unassociated hits information
  std::vector<int> * unassociated0[3]; //0 stage
  std::vector<int> * unassociated1[3]; //1 stage
  std::vector<int> * unassociated2[3]; //2 stage
  

  //parameters for calorimeter
  int nCalo;
  std::vector<double> * eta_ca;
  std::vector<double> * phi_ca;
  std::vector<double> * ene_ca;
  //All first and second layer clusters
  std::vector<PixelCluster> clusterLayer[3];
  std::map<int,int> usedClusterLayer[3];
  //std::map<int,int> usedClusterLayer1;
  //std::map<int,int> usedClusterLayer2;
  std::vector<std::vector<PixelCluster> > trackletsCol; //tracklets collection
  std::vector<const HepMC::GenParticle*> primaryGenParts;
  std::vector<const HepMC::GenParticle*> weakDecayParts;
  std::vector<std::vector<PixelCluster> > genPartCol; //genParticle clusters collection for primaries
  std::vector<std::vector<PixelCluster> > weakDecayCol; //clusters collection for weak decays

  std::vector<int> * nPixelClustersT; //how many clusters do truth particle have in the first two layers
  std::vector<int> * nBLayerClustersT;
  std::vector<int> * primaryDecayLabel; //lable this track is really from primary or from others
  std::map<int,int> threeLayerTrkMap;
  //std::vector<std::vector<PixelCluster> > notFoundPart; //gen particle clusters
  //efficiency tree 0;
  TTree* effTree0;
  TTree* effTree1;
  std::vector<int> * projected_eff;
  std::vector<double> * phi_eff;
  std::vector<double> * eta_eff;
  std::vector<double> * pt_eff;

  std::vector<std::vector<int> > deadLayer0;
  std::vector<std::vector<int> > deadLayer1;
};

#endif
