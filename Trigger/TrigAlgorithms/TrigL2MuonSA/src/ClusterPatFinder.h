/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_CLUSTERPATFINDER_H
#define  TRIGL2MUONSA_CLUSTERPATFINDER_H

#include <string> 
#include <list> 
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

namespace TrigL2MuonSA {

struct ClusterPattern
{
  std::multimap<int, int> groupCand;
  int group;
  double dMM;
  double dMO;
  double aw[3];
  double bw[3];
  int clustersID[8];
  bool isGoodFit;
  double phi_middle;
  double phi_outer;
  int nclusters;

  void Clear(){
    groupCand.clear();
    group = -1;
    dMM = 9999;
    dMO = 9999;
    isGoodFit = false;
    phi_middle = 0;
    phi_outer = 0;
    nclusters = 0;
    for(int i=0; i < 3; i++){
      aw[i] = 0;
      bw[i] = 0;
    }
    for(int i=0; i < 8; i++) clustersID[i] = -1;
  }
  
  bool operator==(const ClusterPattern& clp) const {
    for(int i=0; i < 8; i++){
      if(clustersID[i] != clp.clustersID[i]) return false;
    }
    return true;
  }

};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
struct RpcLayerClusters
{
  std::vector<std::list<double>> clusters_in_layer_eta;  
  std::vector<std::list<double>> clusters_in_layer_phi;  
  std::vector<std::vector<double>> clusters_in_layer_Z;  
  std::vector<std::vector<double>> clusters_in_layer_R; 
  void clear() {
    std::list<double> z;
    z.clear();
    clusters_in_layer_eta.assign(8,z);
    clusters_in_layer_phi.assign(8,z);
    std::vector<double> zz;
    zz.clear();
    clusters_in_layer_R.assign(8,zz);
    clusters_in_layer_Z.assign(8,zz);
  }
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class ClusterPatFinder: public AthAlgTool
{

 public:
  ClusterPatFinder(const std::string& type, 
       	           const std::string& name,
                   const IInterface*  parent);

 public:

  void addCluster(std::string stationName,
	          int stationEta,
                  bool  measuresPhi,
                  unsigned int  gasGap,
                  unsigned int doubletR,
                  double gPosX, double gPosY, double gPosZ, 
                  TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const;

  bool findPatternEta(std::vector<std::vector<double>>& aw, 
                      std::vector<std::vector<double>>& bw,
                      TrigL2MuonSA::RpcLayerClusters&   rpcLayerClusters) const; 
  
  bool findPatternPhi(std::vector<double>& phi_middle, 
                      std::vector<double>& phi_outer, 
                      TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const;

 private:
  bool patfinder_forEta(std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns, 
                        TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const;

  bool patfinder(std::vector<TrigL2MuonSA::ClusterPattern>& crPattern, 
                 TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const;

  bool deltaOK(int l1, int l2, double x1, double x2, int isphi, double &delta) const;  
    
  double calibR(std::string stationName, double R, double Phi) const;  
  
  void abcal(unsigned int result_pat, size_t index[], double aw[], double bw[], TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const;
  
  void removeSimilarRoad(std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns) const;

  void setGroup(int& nGroup,
                std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns) const;
  void selectGoodFit(int nGroup,
                     std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns) const;
};

}
#endif

