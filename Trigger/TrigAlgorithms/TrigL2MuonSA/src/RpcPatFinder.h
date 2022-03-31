/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPAPATFINDER_H
#define  TRIGL2MUONSA_RPAPATFINDER_H

#include <string> 
#include <list> 

#include "AthenaBaseComps/AthAlgTool.h"

// Original author: Massimo Corradi

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
struct RpcLayerHits
{
  std::vector<std::list<double>> hits_in_layer_eta;  
  std::vector<std::list<double>> hits_in_layer_phi;  
  std::vector<std::vector<double>> hits_in_layer_Z;  
  std::vector<std::vector<double>> hits_in_layer_R; 
  void clear() {
    std::list<double> z;
    z.clear();
    hits_in_layer_eta.assign(8,z);
    hits_in_layer_phi.assign(8,z);
    std::vector<double> zz;
    zz.clear();
    hits_in_layer_R.assign(8,zz);
    hits_in_layer_Z.assign(8,zz);
  }
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


class RpcPatFinder: public AthAlgTool
{

 public:

  RpcPatFinder(const std::string& type, 
	       const std::string& name,
               const IInterface*  parent);

 public:

  void addHit(const std::string& stationName,
	      int stationEta,
	      bool  measuresPhi,
	      unsigned int  gasGap,
	      unsigned int doubletR,
	      double gPosX, double gPosY, double gPosZ,
              TrigL2MuonSA::RpcLayerHits& rpcLayerHits) const;
  bool findPatternEta(double aw[], double bw[], unsigned int &pattern, const TrigL2MuonSA::RpcLayerHits& rpcLayerHits) const;
  bool findPatternPhi(double &phi_middle, double &phi_outer, unsigned int &pattern, const TrigL2MuonSA::RpcLayerHits& rpcLayerHits) const;
  
 private:
  int patfinder(bool iphi,
		unsigned int &result_pat,
		double &result_x,
		double &result_x1,
		double &result_dMO,
                const TrigL2MuonSA::RpcLayerHits rpcLayerHits) const;

  int patfinder_forEta(bool iphi,
		    unsigned int &result_pat,
		    double result_aw[],
		    double result_bw[],
		    double result_dist[],
                    const TrigL2MuonSA::RpcLayerHits rpcLayerHits) const;

  bool deltaOK(int l1, int l2, double x1, double x2, int isphi, double &delta) const;  
  double calibR(const std::string& stationName, double R, double Phi) const;  
  void abcal(unsigned int result_pat, 
             size_t index[], 
             double aw[], 
             double bw[], 
             const TrigL2MuonSA::RpcLayerHits rpcLayerHits) const;
};

}
#endif

