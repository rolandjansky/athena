/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSMAKER_H
#define HIPIXELTRACKLETSMAKER_H
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"
#include "HIGlobal/IndexEtaPhi.h"
#include "TF1.h"

//forward declaration
class PixelID;
class HIPixelTracklets;
class Point3D;

class HIPixelTrackletsMaker : public AthAlgorithm {
  typedef InDet::PixelClusterContainer PixelClusterContainer;
  typedef InDet::PixelClusterCollection PixelClusterCollection;
  typedef InDet::PixelCluster PixelCluster;
  typedef std::map<int, int>::iterator mapiiitr;

 public:
  HIPixelTrackletsMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private: 
  //data element
  double m_minEta; //eta range we are interested
  double m_maxEta;
  double m_minPhi;
  double m_maxPhi;
  int m_nBinsEta; //number of bins in eta direction
  int m_nBinsPhi; 
  double m_binSizeEta; //(m_maxEta-m_minEta)/m_nBinsEta
  double m_binSizePhi;
  double m_dEta; //used for tracklets finding
  double m_dPhi;
  
  double m_vx;
  double m_vy;
  double m_vz;

  bool m_doLayerSum3;
  int m_trackletsMethod;
  int m_backgroundFlip;
  bool m_doMC;
  bool m_doMagOff;
  TF1* m_sigmaDetaEtaFunc;
  TF1* m_sigmaDphiEtaFunc;
  const PixelID *m_pixelID;
  std::string m_pixelClustersName;
  HepGeom::Point3D<double> m_edgePositionLayer0[13][22];
  double m_edgePhiLayer0[13][22];
  HepGeom::Point3D<double> m_edgePositionLayer1[13][38];
  double m_edgePhiLayer1[13][38];
  HepGeom::Point3D<double> m_edgePositionLayer2[13][52];
  double m_edgePhiLayer2[13][52];
  const DataHandle<PixelClusterContainer> m_clusters;
  std::vector<PixelCluster> m_clusterLayer[3];
  std::map<int,double> m_etaValueLayer[3];
  std::map<int,double> m_phiValueLayer[3];
  std::map<int, int> m_usedClusterMap[3]; //used clusters
  std::map<int, std::vector<int> > m_clusterMap[3];
  std::set<int> m_filledCells[3];
  std::map<int, std::vector<IndexEtaPhi> > m_gangedClusIndexLayer0[22][13];
  std::map<int, std::vector<IndexEtaPhi> > m_gangedClusIndexLayer1[38][13];
  std::map<int, std::vector<IndexEtaPhi> > m_gangedClusIndexLayer2[52][13];
  std::map<int, int> m_gangedNoiseLayer[3];
  std::map<int, int> m_overlapClusLayer[3];
 private:
  //function
  void setEdgePosition();
  void setEdgePhi();
  void clearVectors();
  void layerClusters (const DataHandle<PixelClusterContainer>& clusters);
  void mapClusters();
  void makeTracklets(int innerLayer, int outLayer, std::map<int,std::vector<HIPixelTracklets*> > & trkletsCellMap);
  void makeTrackletsInCell(int innerLayer, int outLayer, int bin, std::vector<HIPixelTracklets*> & trkletsColl, std::vector<int> usedClusterIndex[3]);
  void returnCorrespondingCell(int bin, int & i, int & j);
  void makeTrackletsCellToCell(int innerLayer, int outLayr, int etaBin, int phiBin, const std::vector<int> & iclus, const std::vector<int> & oclus, std::vector<HIPixelTracklets*> & trkletsColl);
  int checkGangedOrOverlap(const HIPixelTracklets* trk1, const HIPixelTracklets* trk2);
  //void removeDuplicateInCell(const std::vector<HIPixelTracklets*> & trkletsCollWithDup, std::vector<HIPixelTracklets*> & trkletsColl, int & hasDuplicate);
  //void removeDuplicateInNeighborCell(const std::vector<HIPixelTracklets*> & trackletsInCell, const std::vector<HIPixelTracklets*> & trackletsInNeighborCell, std::vector<HIPixelTracklets*> & newTrackletsInNeighborCell, int & hasDuplicate);
  void removeDuplicateLayerToLayer(int innerLayer, const std::map<int,std::vector<HIPixelTracklets*> > & toBeCleanedColl, std::map<int,std::vector<HIPixelTracklets*> > & referenceColl0, std::map<int,std::vector<HIPixelTracklets*> > & referenceColl1, HIPixelTrackletsCollection* trkcoll);
  double energyThresh(int layer, double eta);
  void fittingParSigmaDeta(int tracklet, double eta, double & par0, double & par1);

  bool gangedNoiseClus(const std::vector<IndexEtaPhi> & pixsId0, 
		       const std::vector<IndexEtaPhi> & pixsId1);
  void removeGangedNoise(int layer, int arrSize0,
			 const std::map<int, std::vector<IndexEtaPhi> > gangedClusColl[][13]);
  bool gangedNoiseClusEqualOrIncld(const PixelCluster & clus0, const PixelCluster & clus1);
  void removeOverLapCluster(int layer, int arrSize0);
};



#endif
