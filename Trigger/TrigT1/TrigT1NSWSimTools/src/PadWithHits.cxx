/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PadWithHits.h"

#include "TMath.h"
#include "TVector3.h"

#include "tdr_typedefs.h"
// #include "NSWNtuple/sTGCgeometry.h"
#include "vector_utils.h"
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
      <<"'side'      : " <<side<<      ", "
      <<"'module'    : " <<module<<    ", "
      <<"'loPhi'     : " <<m_loPhi<<   ", "
      <<"'hiPhi'     : " <<m_hiPhi<<   ", "
      <<"'loHei'     : " <<m_loHei<<   ", "
      <<"'hiHei'     : " <<m_hiHei<<   ", "
      <<"'loEta'     : " <<m_loEta<<   ", "
      <<"'hiEta'     : " <<m_hiEta<<   ", ";
    return oo.str();
  }


  //-------------------------------------
  bool operator== (const Pad &lhs, const Pad &rhs) {
    return lhs.ieta  ==rhs.ieta
      &&   lhs.iphi  ==rhs.iphi
      &&   lhs.multiplet ==rhs.multiplet
      &&   lhs.layer ==rhs.layer
      &&   lhs.sector==rhs.sector
      &&   lhs.side  ==rhs.side
      &&   lhs.module ==rhs.module;
  }
  //-------------------------------------
  std::ostream& operator<<( std::ostream& oo, const Pad &p){
    oo<<"pad["<<p.ieta<<","<<p.iphi<<"] :"
      " (m="<<p.multiplet<<", l="<<p.layer<<", s="<<p.sector
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
  //-------------------------------------
/*
  bool determinePadFromHit(const D3PDReader::sTGCHitsD3PDObjectElement &hit,
                           Pad &result, bool verbose)
  {
   return determinePad(hit.layer(), hit.wedgeId(), hit.wedgeType(),
                        hit.sectorNumber(), hit.detectorNumber(),
                        TVector3(hit.globalPositionX(),
                                 hit.globalPositionY(),
                                 hit.globalPositionZ()),
                        result);
  }
*/
  //-------------------------------------
/*
  bool wedgeidIsInvalid(int wedgeid, bool verbose){
    bool invalid(wedgeid!=LARGE && wedgeid!=SMALL);
    if(invalid && verbose) cout<<"invalid wedgeid "<<wedgeid<<endl;
    return invalid;
  }
*/
  //-------------------------------------
/*
  bool wedgetypeIsInvalid(int wedgetype, bool verbose){
    bool invalid(wedgetype!=PIVOT && wedgetype!=CONFIRM);
    if(invalid && verbose) cout<<"invalid wedgetype "<<wedgetype<<endl;
    return invalid;
  }
*/
  //-------------------------------------
  bool layerIsInvalid(int layer, bool verbose){
    bool invalid(layer<1 || layer>4);
    if(verbose){}
    // if(invalid && verbose) cout<<"invalid layer "<<layer<<endl;
    return invalid;
  }
  //-------------------------------------
  bool sectorIsInvalid(int sector, bool verbose){
    bool invalid(sector<1 || sector>16);
    if(verbose){}
    // if(invalid && verbose) cout<<"invalid sector "<<sector<<endl;
    return invalid;
  }
/*
  bool detectorNumberIsInvalid(int dn, bool verbose){
    bool invalid(dn<0 || dn >STGC_DETECTOR_3);
    if(invalid && verbose) cout<<"invalid detectorNumber "<<dn<<endl;
    return invalid;
  }
*/
  //-------------------------------------
/*
  bool determinePad(int layer, int wedgeId, int wedgeType, int sector, int detectorNumber,
                    TVector3 pos, Pad & result, bool verbose)
  {
    if(layerIsInvalid         (layer,          verbose)) return false;
    if(sectorIsInvalid        (sector,         verbose)) return false;
    if(detectorNumberIsInvalid(detectorNumber, verbose)) return false;
    int la(layer-1); // layer is in [1,4], but indices start from 0
    int wi(wedgeId), wt(wedgeType), sn(sector), dn(detectorNumber);
    int side(Pad::sideFromZ(pos.Z()));
    int st(STGC_TYPES);
    int sdt(STGC_DETECTORS);
    int lt(NO_LAYER);
    using namespace std;
    // some conversion enum<-->int (should be implemented with the enum)
	if     (LARGE == wi && PIVOT   == wt) st = LARGE_PIVOT;
	else if(LARGE == wi && CONFIRM == wt) st = LARGE_CONFIRM;
	else if(SMALL == wi && PIVOT   == wt) st = SMALL_PIVOT;
	else if(SMALL == wi && CONFIRM == wt) st = SMALL_CONFIRM;
    else {
      if(verbose) cout<<"invalid L/S ("<<wi<<"), P/C ("<<wt<<") values "<<endl;
      return false;
    }
    switch(la) {
    case 0: lt = STGC_LAYER_1; break;
    case 1: lt = STGC_LAYER_2; break;
    case 2: lt = STGC_LAYER_3; break;
    case 3: lt = STGC_LAYER_4; break;
    default :
      if(verbose) cout<<"invalid layer index "<<la<<endl;
      return false;
    }
    bool isInnerDetector(dn==STGC_DETECTOR_0 || dn==STGC_DETECTOR_1);
    float phiPadSize(PAD_PHI_DIVISION);
    if(isInnerDetector) phiPadSize /= PAD_PHI_SUBDIVISION; // inner dets have finer pads
	float phiOrigin(PAD_COL_PHI0[st][dn][la]);
	float hLowEdge(H_PAD_ROW_0[st][la]);
    float padHeight(PAD_HEIGHT[st][la]);
    //-- int leftmostCol(INDEX_LEFTMOST_COL[st][dn][la]); // see comment a few lines below
    //-- int rightmostCol(INDEX_RIGHTMOST_COL[st][dn][la]);
	float phiCenterSector(midSectorPhi(sn));
    float phiSectorFive(midSectorPhi(5)); // sector 5 is the one aligned with the ATLAS y axis
	pos.RotateZ(phiSectorFive - phiCenterSector);
    hLowEdge  = adjustHforZdifference(hLowEdge,  st, la);
    padHeight = adjustHforZdifference(padHeight, st, la);
    int ieta = TMath::Floor((pos.Y() - hLowEdge) / padHeight);

	int iphi = TMath::Floor((pos.Phi() - phiOrigin - phiSectorFive) / phiPadSize);

    float loHei(hLowEdge + ieta*padHeight);
    float hiHei(loHei + padHeight);
    float loPhi(phiOrigin + phiSectorFive + iphi*phiPadSize);
    float hiPhi(loPhi + phiPadSize);
    if((loHei > pos.Y()) || (pos.Y() > hiHei) || (loPhi > pos.Phi()) || (pos.Phi() > hiPhi))
      cout<<"Something wrong with pad edges? " // here we should assert, but there's 2pi ambiguity
          <<"height : "<<loHei<<" < "<<pos.Y()  <<" < "<<hiHei<<endl
          <<"phi    : "<<loPhi<<" < "<<pos.Phi()<<" < "<<hiPhi<<endl
          <<"loPhi=("<<phiOrigin<<" + "<<phiSectorFive<<" + "<<iphi<<"*"<<phiPadSize<<")"<<endl
          <<"phiOrigin(PAD_COL_PHI0["<<st<<"]["<<dn<<"]["<<la<<"]) : "<<PAD_COL_PHI0[st][dn][la]<<endl
        ;
    // DG not sure whether we should use this adjustment...ask Shikma
    //-- if     (iphi < 0 && iphi < leftmostCol ) iphi = leftmostCol;
  	//-- else if(iphi > 0 && iphi > rightmostCol) iphi = rightmostCol;
    if(verbose)
      cout<<"determine pad ("<<pos.X()<<", "<<pos.Y()<<", "<<pos.Z()<<")"
          <<" sector "<<sn<<" layer "<<(la+1)
          <<" ("<<ieta<<","<<iphi<<")"<<endl;
    result.ieta   = ieta;
    result.iphi   = iphi;
    result.det    = sdt;
    result.stgc   = st;
    result.layer  = lt;
    result.sector = sn;
    result.side   = side;
    result.setLowPhi(loPhi).setHighPhi(hiPhi).setLowH(loHei).setHighH(hiHei).fillCornerCoords(pos.Z(), phiSectorFive - phiCenterSector);
    return true;
  };
*/
  Pad& Pad::fillCornerCoords(float loHeight, float hiHeight,
                             float loPhi, float hiPhi,
                             float zPos, float angleToSec5)
  {
    float cotanLoPhi(1./tan(loPhi)), cotanHiPhi(1./tan(hiPhi)); // no NaN, we are in sec 5, so around pi/2
    // N.B. loHeight, hiHeight, loPhi, hiPhi are all computed in the coordinates rotated to sec 5
    // We now rotate back the original sector and provide global coordinates
    TVector3 p0(cotanLoPhi*loHeight, loHeight ,zPos); p0.RotateZ(-angleToSec5);
    TVector3 p1(cotanLoPhi*hiHeight, hiHeight, zPos); p1.RotateZ(-angleToSec5);
    TVector3 p2(cotanHiPhi*loHeight, loHeight, zPos); p2.RotateZ(-angleToSec5);
    TVector3 p3(cotanHiPhi*hiHeight, hiHeight, zPos); p3.RotateZ(-angleToSec5);
    m_cornerXyz[0][0] = p0.X(); m_cornerXyz[0][1] = p0.Y(); m_cornerXyz[0][2] = p0.Z();
    m_cornerXyz[1][0] = p1.X(); m_cornerXyz[1][1] = p1.Y(); m_cornerXyz[1][2] = p1.Z();
    m_cornerXyz[2][0] = p2.X(); m_cornerXyz[2][1] = p2.Y(); m_cornerXyz[2][2] = p2.Z();
    m_cornerXyz[3][0] = p3.X(); m_cornerXyz[3][1] = p3.Y(); m_cornerXyz[3][2] = p3.Z();
    //m_loEta = p0.Eta();
    //m_hiEta = p1.Eta();
    //m_loPhi = p0.Phi();
    //m_hiPhi = p2.Phi();
    return *this;
  }
  //-------------------------------------
  Pad& Pad::fillCornerCoords(float corners[4][3])
  {
    std::vector<TVector3> points; 
    for(unsigned int i=0; i<4; i++) {
      for(unsigned int j=0; j<3; j++) {
        m_cornerXyz[i][j] = corners[i][j];
      }
      TVector3 point(m_cornerXyz[i][0],m_cornerXyz[i][1],m_cornerXyz[i][2]);
      //cout << "    >> Pad Corner " << i 
      //      << " x " << m_cornerXyz[i][0]
      //      << " y " << m_cornerXyz[i][1]
      //      << " z " << m_cornerXyz[i][2] 
      //      << " eta " << point.Eta() 
      //      << " phi " << point.Phi()
      //      << endl;
      points.push_back(point);
    }
    // This is not strictly correct but and approximation - ASM-2017-07-07
    // This is not strictly correct but and approximation - ASM-2017-07-07
    // This is not strictly correct but and approximation - ASM-2017-07-07
    //m_loEta = points[0].Eta();
    //m_hiEta = points[2].Eta();
    //m_loPhi = points[0].Phi();
    //m_hiPhi = points[1].Phi();
    m_loEta = points[0].Eta();
    m_hiEta = points[0].Eta();
    m_loPhi = points[0].Phi();
    m_hiPhi = points[0].Phi();
    for(unsigned int i=1; i<4; i++) {
        if(points[i].Eta()<m_loEta) m_loEta = points[i].Eta();
        if(points[i].Eta()>m_hiEta) m_hiEta = points[i].Eta();
        if(points[i].Phi()<m_loPhi) m_loPhi = points[i].Phi();
        if(points[i].Phi()>m_hiPhi) m_hiPhi = points[i].Phi();
    }
    // This is not strictly correct but and approximation - ASM-2017-07-07
    // This is not strictly correct but and approximation - ASM-2017-07-07
    // This is not strictly correct but and approximation - ASM-2017-07-07
    
    return *this;   
  }
  //-------------------------------------
/*
  float adjustHforZdifference(const float &h, const STGCType &s, const int &l)

    return h * Z_CURRENT_LAYOUT[s][l] / Z_DANIEL[s][l];
  }
*/
  //-------------------------------------
  PadWithHits::PadWithHits(const int &ieta, const int &iphi,
                           const int &multiplet,
                           const int &l, const int &sector, const int &side, const int &module) :
    Pad(ieta, iphi, multiplet, l, sector, side, module),
    m_totEnergy(0.), m_avgEta(0.), m_avgPhi(0.),
    m_minEta(FLT_MAX), m_minPhi(FLT_MAX), m_maxEta(FLT_MIN), m_maxPhi(FLT_MIN)
    {}
  //-------------------------------------
  PadWithHits::PadWithHits(const NSWL1::PadData &pData) :
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
  //-------------------------------------
/*
  PadWithHits& PadWithHits::addHit(const D3PDReader::sTGCHitsD3PDObjectElement &hit, const size_t &hitIndex)
  {
    m_hitIndices.push_back(hitIndex);
    TVector3 pos(hit.globalPositionX(), hit.globalPositionY(), hit.globalPositionZ());
    float eta(pos.Eta()), phi(pos.Phi());
    updateWeightedCoord(eta, phi, hit.depositEnergy());
    m_totEnergy += hit.depositEnergy(); // must be after updateWeightedCoord (otherwise double-count)
    m_minEta = (m_minEta < eta ? m_minEta : eta);
    m_minPhi = (m_minPhi < phi ? m_minPhi : phi);
    m_maxEta = (m_maxEta > eta ? m_maxEta : eta);
    m_maxPhi = (m_maxPhi > phi ? m_maxPhi : phi);
    return *this;
  }
*/
  //-------------------------------------
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
  struct inSector: public std::unary_function<const Pad&, void>
  {
    int sector_;
    size_t index_;
    vector<size_t> indices_;
    inSector(int sector) : sector_(sector), index_(0) {}
    void operator() (const Pad &p) { if(sector_==p.sector) indices_.push_back(index_); ++index_; }
  };
  //-------------------------------------
  vector<size_t> filterPadIndicesBySector(const vector<Pad> &pads, int sector)
  {
    return (for_each(pads.begin(), pads.end(), inSector(sector))).indices_;
  }
  //-------------------------------------
  vector<size_t> filterPadIndicesBySector(const vector<PadWithHits> &pads, int sector)
  {
    return (for_each(pads.begin(), pads.end(), inSector(sector))).indices_;
  }
  //-------------------------------------
  vector<size_t> filterPadIndicesBySide(const vector<Pad>    &pads,
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
  vector<size_t> filterPadIndicesBySide(const vector<PadWithHits>    &pads,
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
  vector<size_t> filterPadIndicesByLayer(const vector<Pad>    &pads,
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
  vector<size_t> filterPadIndicesByLayer(const vector<PadWithHits> &pads,
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
  vector<size_t> filterPadIndicesByMultiplet(const vector<Pad>    &pads,
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
  vector<size_t> filterPadIndicesByMultiplet(const vector<PadWithHits> &pads,
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
