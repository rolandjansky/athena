/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadWithHits.h"

#include "TMath.h"
#include "TVector3.h"

#include "TrigT1NSWSimTools/tdr_typedefs.h"
// #include "NSWNtuple/sTGCgeometry.h"
#include "TrigT1NSWSimTools/vector_utils.h"
// #include "NSWNtuple/NSW_sTGCHitPosition.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <float.h> // FLT_MIN/FLT_MAX
#include <iostream>
#include <math.h>  // M_PI
#include <sstream>

namespace nsw {
  using std::vector;
  using std::for_each;
  using std::cout;
  using std::endl;

 std::string Pad::pickle() const
  {
    std::ostringstream oo;
    oo<<"'ieta'      : " <<ieta<<      ", "
      <<"'iphi'      : " <<iphi<<      ", "
      <<"'multiplet' : " <<multiplet<< ", "
      <<"'layer'     : " <<layer<<     ", "
      <<"'sector'    : " <<sector<<    ", "
      <<"'sectortype'    : " <<sectortype<<    ", "
      <<"'side'      : " <<side<<      ", "
      <<"'module'    : " <<module<<    ", ";
    return oo.str();
  }


  //-------------------------------------
  bool operator== (const Pad &lhs, const Pad &rhs) {
    return lhs.ieta  ==rhs.ieta
      &&   lhs.iphi  ==rhs.iphi
      &&   lhs.multiplet ==rhs.multiplet
      &&   lhs.layer ==rhs.layer
      &&   lhs.sector==rhs.sector
      &&   lhs.sectortype==rhs.sectortype
      &&   lhs.side  ==rhs.side
      &&   lhs.module ==rhs.module;
  }
  //-------------------------------------
  std::ostream& operator<<( std::ostream& oo, const Pad &p){
    oo<<"pad["<<p.ieta<<","<<p.iphi<<"] :"
      " (m="<<p.multiplet<<", l="<<p.layer<<", s="<<p.sector<<", st="<<p.sectortype
      <<", side="<<(p.side==0 ? "A" : "C")<<", m="<<p.module
      <<")";
    return oo;
  }
  //-------------------------------------
  float midSectorPhi(int sector){
    float nSectors=16.0;
    float sectorDphi(TMath::TwoPi()/nSectors);
    // if(sector<1) cout<<"sector "<<sector<<endl;
    assert(sector>0); // we are assuming that the sector N starts from 1
    return (sector-1)*sectorDphi;
  }
  //--------------------------------------------------------------
  float phi_mpi_pi(const double &val) {
    double value(val);
    while(value <= -M_PI) value += 2.*M_PI;
    while(value >  +M_PI) value -= 2.*M_PI;
    return value;
  }
  //--------------------------------------------------------------
  float phi_zero_2pi(const double &val) {
    double value(val);
    while(value <= 0.     ) value += 2.*M_PI;
    while(value >  2.*M_PI) value -= 2.*M_PI;
    return value;
  }

  bool layerIsInvalid(int layer){
    bool invalid(layer<1 || layer>4);
    return invalid;
  }
  //-------------------------------------
  bool sectorIsInvalid(int sector){
    bool invalid(sector<1 || sector>16);
    return invalid;
  }

  //-------------------------------------
  Pad& Pad::fillCornerCoords(float corners[4][3]){
    std::vector<TVector3> points; 
    for(unsigned int i=0; i<4; i++) {
      for(unsigned int j=0; j<3; j++) {
        m_cornerXyz[i][j] = corners[i][j];
      }
      TVector3 point(m_cornerXyz[i][0],m_cornerXyz[i][1],m_cornerXyz[i][2]);
      points.push_back(point);
    }
    
    return *this;   
  }

  //-------------------------------------

  PadWithHits::PadWithHits(const int &ieta, const int &iphi,
                           const int &multiplet,
                           const int &l, const int &sector, const int &sectortype, const int &side, const int &module) :
    Pad(ieta, iphi, multiplet, l, sector, side, module),
    m_totEnergy(0.), m_avgEta(0.), m_avgPhi(0.),
    m_minEta(FLT_MAX), m_minPhi(FLT_MAX), m_maxEta(FLT_MIN), m_maxPhi(FLT_MIN)
    {}
  //-------------------------------------
  PadWithHits::PadWithHits(spPadData pData) :
    Pad(pData),
    m_totEnergy(0.), m_avgEta(0.), m_avgPhi(0.),
    m_minEta(FLT_MAX), m_minPhi(FLT_MAX), m_maxEta(FLT_MIN), m_maxPhi(FLT_MIN)
    {}
  //-------------------------------------
  PadWithHits::PadWithHits(const Pad &p) :
    Pad(p),
    m_totEnergy(0.), m_avgEta(0.), m_avgPhi(0.),
    m_minEta(FLT_MAX), m_minPhi(FLT_MAX), m_maxEta(FLT_MIN), m_maxPhi(FLT_MIN)
    {}

    
  std::string PadWithHits::pickle() const
  {
    std::ostringstream oo;
    oo<<Pad::pickle()
      <<"'hitIndices' : " <<vec2str(m_hitIndices)<<", "
      <<"'totEnergy' : "  <<m_totEnergy<<          ", "
      <<"'avgEta' : "     <<m_avgEta<<             ", "
      <<"'avgPhi' : "     <<m_avgPhi<<             ", "
      <<"'minEta' : "     <<m_minEta<<             ", "
      <<"'minPhi' : "     <<m_minPhi<<             ", "
      <<"'maxEta' : "     <<m_maxEta<<             ", "
      <<"'maxPhi' : "     <<m_maxPhi<<             ", "
      <<"'cornerXyz' : "
      <<"("
      <<"("<<m_cornerXyz[0][0]<<","<<m_cornerXyz[0][1]<<","<<m_cornerXyz[0][2]<<"),"
      <<"("<<m_cornerXyz[1][0]<<","<<m_cornerXyz[1][1]<<","<<m_cornerXyz[1][2]<<"),"
      <<"("<<m_cornerXyz[2][0]<<","<<m_cornerXyz[2][1]<<","<<m_cornerXyz[2][2]<<"),"
      <<"("<<m_cornerXyz[3][0]<<","<<m_cornerXyz[3][1]<<","<<m_cornerXyz[3][2]<<"),"
      <<")"<<                                      ", ";
    return oo.str();
  }
  //-------------------------------------
  void PadWithHits::updateWeightedCoord(const double &eta, const double &phi, const float &charge)
  {
    float weightOld(fabs(m_totEnergy)), weightNew(fabs(charge));
    float weightTot(weightOld+weightNew);
    if(weightTot==0.0){
      m_avgEta = 0.5*(m_avgEta + eta);
      m_avgPhi = 0.5*(m_avgPhi + phi);
    } else {
      float invWeightTot = 1.0 / weightTot;
      m_avgEta = ((m_avgEta*weightOld + eta*weightNew) * invWeightTot);
      m_avgPhi = ((m_avgPhi*weightOld + phi*weightNew) * invWeightTot);
    }
  }
  //-------------------------------------
  // a functor storing the indices of pads in a given sector
  struct inSector: public std::unary_function<const Pad&, void>{
    int sector_;
    size_t index_;
    vector<size_t> indices_;
    inSector(int sector) : sector_(sector), index_(0) {}
    void operator() (const Pad &p) { if(sector_==p.sector) indices_.push_back(index_); ++index_; }
  };
  //-------------------------------------
  

  vector<size_t> filterBySector(const vector<Pad> &pads, int sector){
    return (for_each(pads.begin(), pads.end(), inSector(sector))).indices_;
  }
  //-------------------------------------
  vector<size_t> filterBySector(const vector<PadWithHits> &pads, int sector){
    return (for_each(pads.begin(), pads.end(), inSector(sector))).indices_;
  }
  //-------------------------------------
  vector<size_t> filterBySide(const vector<Pad>    &pads,
                                        const vector<size_t> &padSelectedIndices,
                                        int side)
  { // can this be done with std::algo without building a copy? (later)
    vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(side==pads[idx].side) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  vector<size_t> filterBySide(const vector<PadWithHits>    &pads,
                                        const vector<size_t> &padSelectedIndices,
                                        int side)
  { // can this be done with std::algo without building a copy? (later)
    vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(side==pads[idx].side) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  vector<size_t> filterByLayer(const vector<Pad>    &pads,
                                         const vector<size_t> &padSelectedIndices,
                                         int layer)
  { // can this be done with std::algo without building a copy? (later)
    vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(layer==pads[idx].layer) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  vector<size_t> filterByLayer(const vector<PadWithHits> &pads,
                                         const vector<size_t> &padSelectedIndices,
                                         int layer)
  {
    vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(layer==pads[idx].layer) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  vector<size_t> filterByMultiplet(const vector<Pad>    &pads,
                                             const vector<size_t> &padSelectedIndices,
                                             int multiplet)
  { // can this be done with std::algo without building a copy? (later)
    vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(multiplet==pads[idx].multiplet) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  vector<size_t> filterByMultiplet(const vector<PadWithHits> &pads,
                                             const vector<size_t> &padSelectedIndices,
                                             int multiplet)
  {
    vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(multiplet==pads[idx].multiplet) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
} // end nsw
