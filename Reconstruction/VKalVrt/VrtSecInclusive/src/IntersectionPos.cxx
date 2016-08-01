/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VrtSecInclusive/IntersectionPos.h"
#include <TTree.h>

namespace VKalVrtAthena {

  //------------------------------------------------------------
  IntersectionPos_barrel::IntersectionPos_barrel(const char* name, const double& r)
    : m_name ( name ), m_x( new std::vector<double> ), m_y( new std::vector<double> ), m_z( new std::vector<double> ), m_radius(r)  {}
  IntersectionPos_barrel::~IntersectionPos_barrel() { delete m_x;  delete m_y;  delete m_z;  }
  void IntersectionPos_barrel::clear() { m_x->clear(); m_y->clear(); m_z->clear(); }
  void IntersectionPos_barrel::setBranchAddress( TTree *t, const char* collectionName ) {
    t->Branch( Form("%s_Int_%sX", collectionName, m_name.c_str()), &m_x);
    t->Branch( Form("%s_Int_%sY", collectionName, m_name.c_str()), &m_y);
    t->Branch( Form("%s_Int_%sZ", collectionName, m_name.c_str()), &m_z);
  }
  int IntersectionPos_barrel::bec() { return barrel; }
  
  
  //------------------------------------------------------------
  IntersectionPos_endcap::IntersectionPos_endcap(const char* name, const double& zpos, const double& rmin, const double& rmax)
    : m_name ( name ), m_x( new std::vector<double> ), m_y( new std::vector<double> ), m_z( new std::vector<double> ), m_zpos(zpos), m_rmin(rmin), m_rmax(rmax)  {}
  IntersectionPos_endcap::~IntersectionPos_endcap() { delete m_x;  delete m_y;  delete m_z;  }
  void IntersectionPos_endcap::clear() { m_x->clear(); m_y->clear(); m_z->clear(); }
  void IntersectionPos_endcap::setBranchAddress( TTree *t, const char* collectionName ) {
    t->Branch( Form("%s_Int_%sX", collectionName, m_name.c_str()), &m_x);
    t->Branch( Form("%s_Int_%sY", collectionName, m_name.c_str()), &m_y);
    t->Branch( Form("%s_Int_%sZ", collectionName, m_name.c_str()), &m_z);
  }
  int IntersectionPos_endcap::bec() { return endcap; }

}
