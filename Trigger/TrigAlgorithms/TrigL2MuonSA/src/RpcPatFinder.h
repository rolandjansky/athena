/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

class RpcPatFinder: public AthAlgTool
{

 public:

  RpcPatFinder(const std::string& type, 
	       const std::string& name,
                 const IInterface*  parent);

 public:

  void clear();
  void addHit(std::string stationName,
	      int stationEta,
	      bool  measuresPhi,
	      unsigned int  gasGap,
	      unsigned int doubletR,
	      double gPosX, double gPosY, double gPosZ);
  bool findPatternEta(double aw[], double bw[],   unsigned int &pattern);
  bool findPatternPhi(double &phi_middle, double &phi_outer, unsigned int &pattern);
  
 private:
  std::vector<std::list<double>> m_hits_in_layer_eta;  
  std::vector<std::list<double>> m_hits_in_layer_phi;  
  std::vector<std::vector<double>> m_hits_in_layer_Z;  
  std::vector<std::vector<double>> m_hits_in_layer_R;  

  int patfinder(bool iphi,
		unsigned int &result_pat,
		double &result_x,
		double &result_x1,
		double &result_dMO);

  int patfinder_forEta(bool iphi,
		    unsigned int &result_pat,
		    double result_aw[],
		    double result_bw[],
		    double result_dist[]);

  bool deltaOK(int l1, int l2, double x1, double x2, int isphi, double &delta);  
  double calibR(std::string stationName, double R, double Phi) const;  
  void abcal(unsigned int result_pat, size_t index[], double aw[], double bw[]);
};

}
#endif

