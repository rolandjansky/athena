/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPAPATFINDER_H
#define  TRIGL2MUONSA_RPAPATFINDER_H

#include "GaudiKernel/MsgStream.h"

#include <string> 
#include <list> 

// Original author: Massimo Corradi

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class RpcPatFinder{

 public:
  RpcPatFinder(MsgStream* msg);
  ~RpcPatFinder(void);

 public:
  inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }

  void clear();
  void addHit(std::string stationName,
	      bool  measuresPhi,
	      unsigned int  gasGap,
	      unsigned int doubletR,
	      double gPosX, double gPosY, double gPosZ);
  bool findPatternEta(double &ZoverR_middle, double &ZoverR_outer,   unsigned int &pattern);
  bool findPatternPhi(double &phi_middle, double &phi_outer, unsigned int &pattern);
  
 private:
  inline MsgStream& msg() const { return *m_msg; }

  MsgStream* m_msg;
  std::vector<std::list<double>> m_hits_in_layer_eta;  
  std::vector<std::list<double>> m_hits_in_layer_phi;  
  int patfinder(bool iphi,
		unsigned int &result_pat,
		double &result_x,
		double &result_x1,
		double &result_dMO);
  bool deltaOK(int l1, int l2, double x1, double x2, int isphi, double &delta);  
};

}
#endif

