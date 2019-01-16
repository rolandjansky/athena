/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadWithHits.h"

#include "TMath.h"
#include "TVector3.h"

#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/vector_utils.h"

#include <cassert>
#include <float.h> // FLT_MIN/FLT_MAX
#include <sstream>

namespace NSWL1 {
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
  
  //S.I : A lot of cleaning up can be done in this file. Most of the methods below are never used..
  float midSectorPhi(int sector){
    float nSectors=16.0;
    float sectorDphi(TMath::TwoPi()/nSectors);
    assert(sector>0); // we are assuming that the sector N starts from 1
    return (sector-1)*sectorDphi;
  }
  //--------------------------------------------------------------
  float phi_mpi_pi(const double &val) {
    double value(val);
    while(value <= -TMath::Pi()) value += 2.*TMath::Pi();
    while(value >  +TMath::Pi()) value -= 2.*TMath::Pi();
    return value;
  }
  //--------------------------------------------------------------
  float phi_zero_2pi(const double &val) {
    double value(val);
    while(value <= 0.     ) value += 2.*TMath::Pi();
    while(value >  2.*TMath::Pi()) value -= 2.*TMath::Pi();
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
                           const int &l, const int &sector, const int &/*sectortype*/, const int &side, const int &module) :
    Pad(ieta, iphi, multiplet, l, sector, side, module),
    m_totEnergy(0.), m_avgEta(0.), m_avgPhi(0.),
    m_minEta(FLT_MAX), m_minPhi(FLT_MAX), m_maxEta(FLT_MIN), m_maxPhi(FLT_MIN)
    {}
  //-------------------------------------
  PadWithHits::PadWithHits(std::shared_ptr<PadData> pData) :
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
    std::vector<size_t> indices_;
    inSector(int sector) : sector_(sector), index_(0) {}
    void operator() (const Pad &p) { if(sector_==p.sector) indices_.push_back(index_); ++index_; }
  };
  //-------------------------------------
  

  std::vector<size_t> filterBySector(const std::vector<Pad> &pads, int sector){
    return (std::for_each(pads.begin(), pads.end(), inSector(sector))).indices_;
  }
  //-------------------------------------
  std::vector<size_t> filterBySector(const std::vector<PadWithHits> &pads, int sector){
    return (std::for_each(pads.begin(), pads.end(), inSector(sector))).indices_;
  }
  //-------------------------------------
  std::vector<size_t> filterBySide(const std::vector<Pad>    &pads,
                                        const std::vector<size_t> &padSelectedIndices,
                                        int side)
  { // can this be done with std::algo without building a copy? (later)
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(side==pads[idx].side) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  std::vector<size_t> filterBySide(const std::vector<PadWithHits>    &pads,
                                        const std::vector<size_t> &padSelectedIndices,
                                        int side)
  { // can this be done with std::algo without building a copy? (later)
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(side==pads[idx].side) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  std::vector<size_t> filterByLayer(const std::vector<Pad>    &pads,
                                         const std::vector<size_t> &padSelectedIndices,
                                         int layer)
  { // can this be done with std::algo without building a copy? (later)
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(layer==pads[idx].layer) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  std::vector<size_t> filterByLayer(const std::vector<PadWithHits> &pads,
                                         const std::vector<size_t> &padSelectedIndices,
                                         int layer)
  {
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(layer==pads[idx].layer) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  std::vector<size_t> filterByMultiplet(const std::vector<Pad>    &pads,
                                             const std::vector<size_t> &padSelectedIndices,
                                             int multiplet)
  { // can this be done with std::algo without building a copy? (later)
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(multiplet==pads[idx].multiplet) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
  std::vector<size_t> filterByMultiplet(const std::vector<PadWithHits> &pads,
                                             const std::vector<size_t> &padSelectedIndices,
                                             int multiplet)
  {
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(multiplet==pads[idx].multiplet) indices.push_back(idx);
    }
    return indices;
  }
  //-------------------------------------
} // end nsw
