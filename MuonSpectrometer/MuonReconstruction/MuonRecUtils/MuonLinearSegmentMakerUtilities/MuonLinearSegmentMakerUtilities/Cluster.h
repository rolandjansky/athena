/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTER_H
#define CLUSTER_H

#include <math.h>
#include <string>
#include "MuonStationIndex/MuonStationIndex.h"

namespace ClusterSeg {

  struct Cluster {
  
    Cluster( double x_, double y_, double z_, bool isPhi_, Muon::MuonStationIndex::TechnologyIndex tIndex_, Muon::MuonStationIndex::PhiIndex pIndex_, bool isMatch_ ,int barcode_);
    Cluster( double x_, double y_, double z_, bool isPhi_, int tIndex_, int pIndex_, bool isMatch_ ,int barcode_);

    double x() const {return m_x;}
    double y() const {return m_y;}
    double z() const {return m_z;}
    double rSph() const {return sqrt(m_x*m_x+m_y*m_y+m_z*m_z);}
    double rCyl() const {return sqrt(m_x*m_x+m_y*m_y);}
    double phi() const {return atan2(m_y,m_x);}
    double theta() const {return acos(m_z/(this->rSph()));}
    double eta() const {return -log(tan(this->theta()/2.));}
    bool isPhi() const {return m_isPhi;}
    Muon::MuonStationIndex::TechnologyIndex techIndex() const {return m_tIndex;}
    Muon::MuonStationIndex::PhiIndex phiIndex() const {return m_pIndex;}
    bool isMatch() const {return m_isMatch;}
    int barcode() const {return m_barcode;}

    
    bool isSideA() const {if (m_z > 0.) return true; else return false;}

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

    double x() const {return this->rSph()*sin(this->theta())*cos(this->phi());}
    double y() const {return this->rSph()*sin(this->theta())*sin(this->phi());}
    double z() const {return m_z;}
    double rSph() const {return m_z/cos(this->theta());}
    double rCyl() const {return sqrt(pow(this->x(),2)+pow(this->y(),2));}
    double phi() const {return m_phi;}
    double theta() const {return 2*atan(exp(-1*this->eta()));}
    double eta() const {return m_eta;} 
    Muon::MuonStationIndex::TechnologyIndex techIndex() const {return m_tIndex;}
    Muon::MuonStationIndex::PhiIndex phiIndex() const {return m_pIndex;}
    bool isMatch() const {return m_isMatch;}
    int barcode() const {return m_barcode;}
    int eit() const {return m_eit;}
    int pit() const {return m_pit;}

    bool isTGC(int num) const;
    bool isRPC(int num) const;

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
