/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// Cluster.h (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#ifndef CLUSTER_H
#define CLUSTER_H

#include <math.h>
#include <string>
#include "MuonIdHelpers/MuonStationIndex.h"

namespace ClusterSeg {

  struct Cluster {
  
    Cluster( double x_, double y_, double z_, bool isPhi_, Muon::MuonStationIndex::TechnologyIndex tIndex_, Muon::MuonStationIndex::PhiIndex pIndex_, bool isMatch_ ,int barcode_);
    Cluster( double x_, double y_, double z_, bool isPhi_, int tIndex_, int pIndex_, bool isMatch_ ,int barcode_);

    double x(){return m_x;}
    double y(){return m_y;}
    double z(){return m_z;}
    double rSph(){return sqrt(m_x*m_x+m_y*m_y+m_z*m_z);}
    double rCyl(){return sqrt(m_x*m_x+m_y*m_y);}
    double phi(){return atan2(m_y,m_x);}
    double theta(){return acos(m_z/(this->rSph()));}
    double eta(){return -log(tan(this->theta()/2.));}
    bool isPhi(){return m_isPhi;}
    Muon::MuonStationIndex::TechnologyIndex techIndex(){return m_tIndex;}
    Muon::MuonStationIndex::PhiIndex phiIndex(){return m_pIndex;}
    bool isMatch(){return m_isMatch;}
    int barcode(){return m_barcode;}

    
    bool isSideA(){if (m_z > 0.) return true; else return false;}

    double m_x;
    double m_y;
    double m_z;
    bool m_isPhi; 
    Muon::MuonStationIndex::TechnologyIndex m_tIndex;
    Muon::MuonStationIndex::PhiIndex m_pIndex;
    bool m_isMatch;
    int m_barcode;
    std::string m_chamberId;
    
  };

  struct SpacePoint {

    SpacePoint( double eta_, double phi_, double z_, Muon::MuonStationIndex::TechnologyIndex tIndex_, Muon::MuonStationIndex::PhiIndex pIndex_, bool isMatch_ ,int barcode_,int eit_, int pit_);

    double x(){return this->rSph()*sin(this->theta())*cos(this->phi());}
    double y(){return this->rSph()*sin(this->theta())*sin(this->phi());}
    double z(){return m_z;}
    double rSph(){return m_z/cos(this->theta());}
    double rCyl(){return sqrt(pow(this->x(),2)+pow(this->y(),2));}
    double phi(){return m_phi;}
    double theta(){return 2*atan(exp(-1*this->eta()));}
    double eta(){return m_eta;} 
    Muon::MuonStationIndex::TechnologyIndex techIndex(){return m_tIndex;}
    Muon::MuonStationIndex::PhiIndex phiIndex(){return m_pIndex;}
    bool isMatch(){return m_isMatch;}
    int barcode(){return m_barcode;}
    int eit(){return m_eit;}
    int pit(){return m_pit;}

    bool isTGC(int num);
    bool isRPC(int num);

    double m_eta;
    double m_phi;
    double m_z;
    Muon::MuonStationIndex::TechnologyIndex m_tIndex;
    Muon::MuonStationIndex::PhiIndex m_pIndex;
    bool m_isMatch;
    int m_barcode;
    int m_eit;
    int m_pit;
  };

}

#endif
