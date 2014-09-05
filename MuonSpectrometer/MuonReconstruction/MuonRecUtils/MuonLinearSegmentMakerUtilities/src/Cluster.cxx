/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// Cluster.cxx (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"
#include <iostream>

namespace ClusterSeg { 

  Cluster::Cluster( double x_, double y_, double z_, bool isPhi_, bool isMatch_, int barcode_) : m_x(x_),m_y(y_),m_z(z_),m_isPhi(isPhi_),m_isMatch(isMatch_),m_barcode(barcode_),m_chamberId("") {}
  Cluster::Cluster( double x_, double y_, double z_, bool isPhi_, bool isMatch_, int barcode_,std::string chamberId_) : m_x(x_),m_y(y_),m_z(z_),m_isPhi(isPhi_),m_isMatch(isMatch_),m_barcode(barcode_),m_chamberId(chamberId_) {}

  bool Cluster::isTGC(int num){
    double z_ = fabs((*this).z());
    bool return_val = false;
    switch (num){
    case 1 :
      if(z_ > 13200. && z_ < 13700.) return_val = true;
      break;
    case 2 : 
      if(z_ > 14500. && z_ < 14950.) return_val = true;
      break;
    case 3 :
      if(z_ > 14950.) return_val = true;
      break;
    case 4 :
      if(z_ < 13000.) return_val = true;
      break;
    default :
      std::cout << " input not valid. " << std::endl;
    }
    return return_val;
  }

  SpacePoint::SpacePoint( double eta_, double phi_, double z_, bool isMatch_, int barcode_, int eit_, int pit_) : m_eta(eta_),m_phi(phi_),m_z(z_),m_isMatch(isMatch_),m_barcode(barcode_),m_eit(eit_),m_pit(pit_) {}

  bool SpacePoint::isTGC(int num){
    double z_ = fabs((*this).z());
    bool return_val = false;
    switch (num){
    case 1 :
      if(z_ > 13200. && z_ < 13700.) return_val = true;
      break;
    case 2 : 
      if(z_ > 14500. && z_ < 14950.) return_val = true;
      break;
    case 3 :
      if(z_ > 14950.) return_val = true;
      break;
    case 4 :
      if(z_ < 13000.) return_val = true;
      break;
    default :
      std::cout << " input not valid. " << std::endl;
    }
    return return_val;
  }

}
