/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/SingleWedgePadTrigger.h"
#include "TrigT1NSWSimTools/vector_utils.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

using nsw::SingleWedgePadTrigger;
typedef SingleWedgePadTrigger Swpt; // just to get shorter lines


bool sortByLayer(const nsw::PadWithHits &p0, const nsw::PadWithHits& p1) {
  return p0.layer<p1.layer;
}
Swpt::SingleWedgePadTrigger(const std::string &pattern,
                            const vpads_t &pads,
                            const std::vector<size_t> &padIndices) :
  m_pattern(pattern), m_halfPadIndices(-999,-999), m_padIndices(padIndices),m_alreadyCombined(false)
{
  //std::cout<<"SingleWedgePadTrigger: "<<pattern<<" padIndices :"<<nsw::vec2str(padIndices)<<std::endl;
  assert(m_padIndices.size()>0); // a trigger without pads doesn't make sense
  for(size_t i=0; i<padIndices.size(); ++i) m_pads.push_back(pads[padIndices[i]]);
  std::sort(m_pads.begin(), m_pads.end(), sortByLayer);
  m_halfPadIndices = halfPadCoordinates();
}

std::string Swpt::EtaPhi::str() const {
  std::ostringstream oss;
  oss<<"("<<ieta<<", "<<iphi<<")";
  return oss.str();
}

Swpt::EtaPhiHalf Swpt::halfPadCoordinates() const {
  const vpads_t &pads = m_pads;
  using std::cout; using std::endl;
  size_t nPads(pads.size());
  bool haveEnoughPads(nPads>2);
  if(not haveEnoughPads)
      std::cerr<<"Swpt::halfPadCoordinates: need at least 3 pads"<<endl;
  assert(haveEnoughPads);
  const PadWithHits &pad0=pads[0], &pad1=pads[1], &pad2=pads[2];
  int l0(pad0.layer), l1(pad1.layer), l2(pad2.layer);
  // DG: Are we assuming that the pad indices are sorted? this assumption might not be always valid
  // DG: Should we also cross-check the 4th pad if present?
  bool missLast  (l0==STGC_LAYER_1 && l1==STGC_LAYER_2); // missLast also includes 4 out of 4.
  bool missMiddle(l0==STGC_LAYER_1 && l2==STGC_LAYER_4);
  bool missFirst (l0==STGC_LAYER_2 && l1==STGC_LAYER_3);
  bool validLayerCombination(missLast || missMiddle || missFirst);
  // if(!validLayerCombination) cout<<"buggy layer combination? layers: "<<l0<<","<<l1<<","<<l2<<endl;
  assert(validLayerCombination); // probably got a pattern we don't know how to interpret
  (void) validLayerCombination; // to get rid of weird unused variable warning...


//YR: the following is wrong but we keep it for phi as it isn't used much - for eta we use the simple calculation that follows.
  EtaPhiHalf pos(-999,-999);
  EtaPhi posA(-999,-999), posB(-999,-999);
  if       (missLast) { posA = EtaPhi(pad0.ieta, pad0.iphi); posB = EtaPhi(pad1.ieta, pad1.iphi); }
  else if(missMiddle) { posA = EtaPhi(pad0.ieta, pad0.iphi); posB = EtaPhi(pad2.ieta, pad2.iphi); }
  else if (missFirst) { posA = EtaPhi(pad1.ieta, pad1.iphi); posB = EtaPhi(pad0.ieta, pad0.iphi); }
  pos.ieta = ((posA.ieta==posB.ieta) ? (posA.ieta * 2) : (posA.ieta * 2 + 1));
  pos.iphi = ((posA.iphi==posB.iphi) ? (posA.iphi * 2) : (posA.iphi * 2 + 1));
  //cout<<"Half Pad Eta Index= "<<pos.ieta<<endl;
  //S.I Shouldnt it  be pad0.ieta+pad1.ieta ??? SI 13-06-2018
  pos.ieta =   pad0.ieta+pad2.ieta -1; //YR 1-5-2018 any of the first two layers define the 2-bandids and any of the last two layers select the bandid from that pair
  //YR Module specific corrections:
  if(pad0.sectortype==1 && pad0.module==1) pos.ieta +=1;
  if(pad0.sectortype==0 && pad0.module!=1) pos.ieta +=1;
  //cout<<"Corrected Half Pad Eta Index, CandidateY, ieta12, ieta34, pad2, layerp0  "<<pos.ieta<<"   "<<(pad0.m_cornerXyz[2][0]+pad0.m_cornerXyz[1][0]+pad2.m_cornerXyz[2][0]+pad2.m_cornerXyz[1][0])/4<<" "<<pad0.ieta<<" "<<pad2.ieta<<" "<<pad0.layer<< endl;
  return pos;
}

bool Swpt::isSmallSector() const
{ 
    // small sectors are the even ones (when counting 1-16, but sectorId is 0-15)
    const PadWithHits &firstPad = m_pads[0];
    bool isEven = (firstPad.sectortype)==0;
    return isEven;
}

float Swpt::avgEtaFromFirstPad() const
{
  return m_pads[0].avgEta();
}

bool Swpt::isInTransitionRegion() const{
	//return false;
	Polygon ROI=padOverlap3(this->pads());
	 if(GeoUtils::area(ROI)==0) return false;

	const float PI=TMath::Pi();
	float phi0=m_pads[0].sectortype==1 ? (PI/4)*(m_pads[0].sector-1) : (PI/8)+(PI/4)*(m_pads[0].sector-1);
	if(phi0>PI) phi0-=2*PI;

	float ROIx=GeoUtils::centroid(ROI).x();
	float ROIy=GeoUtils::centroid(ROI).y(); //YR 8-18 using the center local Y of the ROI for the decision if in the TR
	float ROIr=sqrt(ROIx*ROIx+ROIy*ROIy);
	float ROIphi=ROIy/ROIx;
	float ROILocalPhi=abs(ROIphi-phi0);
	float ROILocalY=ROIr*cos(ROILocalPhi);

	bool isTr=false;
	float TransitonSmall[7] ={2104,2243,3248,3445,4216,4411,ROILocalY}; // YR should be taken from the XML (H1 active_max, H2 active min... projected from 7 to 1)
	float TransitonLarge[7] ={2144,2278,3294,3483,4406,4596,ROILocalY};

	if(m_pads[0].multiplet==2){
		for(int i= 0; i<6; i++){
			TransitonSmall[i] = TransitonSmall[i]*1.048; //Z5/Z1 ratio
			TransitonLarge[i] = TransitonLarge[i]*1.044;
		}
	}

if (isSmallSector()){
	std::sort(TransitonSmall, TransitonSmall + 7);
	if(TransitonSmall[1]==ROILocalY || TransitonSmall[3]==ROILocalY || TransitonSmall[5]==ROILocalY)  return true;
}
else{
	std::sort(TransitonLarge, TransitonLarge + 7);
	if(TransitonLarge[1]==ROILocalY || TransitonLarge[3]==ROILocalY || TransitonLarge[5]==ROILocalY)  return true;
}

  return isTr;
}

//S.I 3
Polygon Swpt::padOverlap3(const vpads_t &pads){ // \todo some duplication with halfPadCoordinates()...refactor
  using std::cout; using std::endl;
  size_t nPads(pads.size());
  bool haveEnoughPads(nPads>2);
  if(not haveEnoughPads)
      std::cerr<<"Swpt::halfPadCoordinates: need at least 3 pads"<<endl;
  assert(haveEnoughPads);
  const PadWithHits &pad0=pads[0], &pad1=pads[1], &pad2=pads[2];
  int l0(pad0.layer), l1(pad1.layer), l2(pad2.layer); // DG: Are we assuming that the pad indices are sorted? this assumption might not be always valid
  // identify the two staggered pads (see parameter book)
  bool hasL1L2(l0==STGC_LAYER_1 && l1==STGC_LAYER_2); // also includes 4 out of 4.
  bool hasL1L4(l0==STGC_LAYER_1 && l2==STGC_LAYER_4);
  bool hasL2L3(l0==STGC_LAYER_2 && l1==STGC_LAYER_3);
  bool validLayerCombination(hasL1L2 || hasL1L4 || hasL2L3);
  // if(!validLayerCombination) cout<<"buggy layer combination? layers: "<<l0<<","<<l1<<","<<l2<<endl;
  assert(validLayerCombination); // probably got a pattern we don't know how to interpret
  (void) validLayerCombination;
 
  
  //make a vector of pad etaphi polygons
  //calculate the intersection and return it
  std::vector<Polygon> projected_pads;
  //one pad --> one polygon
  //S.I VERY IMPORTANT : A polygon is an ordered set of vertices. So order matters !
  //According to the previous experience (our drawing tool) vertices are connected in the order :
  std::vector<int> vertexordering={0,1,3,2};
  //project each pad polygon onto the first plane of the wedge
  float Zproj=pads[0].m_cornerXyz[1][2];//second index x:0 y:1 z:2
  for(auto pad : pads){
      Vertices vts;
      for(unsigned int i=0;i<4;i++){
        int icorner=vertexordering[i];
        float x=pad.m_cornerXyz[icorner][0];
        float y=pad.m_cornerXyz[icorner][1];
        vts.push_back(Vertex(x,y));
      } 
      Polygon padPolygon=GeoUtils::buildPolygon(vts);
      if(pad.layer==1 && pad.multiplet==1){
          //GeoUtils::print(padPolygon);
      }
      projected_pads.push_back(GeoUtils::Project(padPolygon,pad.m_cornerXyz[2][2],Zproj));
  }
  Polygon res=GeoUtils::intersectionRegion(projected_pads);
  return res;
}


//eof S.I

std::string vec2pickle(const Swpt::vpads_t &pads){
    std::ostringstream oo;
    oo<<"[";
    for(size_t i=0; i<pads.size(); ++i) oo<<"{"<<pads[i].pickle()<<"}, ";
    oo<<"]";
    return oo.str();
}
std::string Swpt::pickle() const{
    std::ostringstream oo;
    oo<<"'pattern' : "<<     "'"<<m_pattern<<"'"<<           ", "
      <<"'padIndices' : "       <<vec2str(m_padIndices)<<    ", "
      <<"'alreadyCombined' : "  <<m_alreadyCombined<<        ", "
      <<"'pads' : "             <<vec2pickle(m_pads)<<       ", "
      <<"'halfPadIndices' : "   <<m_halfPadIndices.pickle()<<", ";
    return oo.str();
}

bool Swpt::areInnerOuterConsistent(const EtaPhiHalf &inner, const EtaPhiHalf &outer, bool verbose) {
// ASM-2016-10-4 : Figure out the logic behind this, the staggering changed now relaxing this a bit 
//  bool mismatchEta(outer.ieta < inner.ieta || outer.ieta > inner.ieta + 1);
//  bool mismatchPhi(outer.iphi < inner.iphi || outer.iphi > inner.iphi + 1);
  bool mismatchEta(outer.ieta < inner.ieta - 4 || outer.ieta > inner.ieta + 4);
  bool mismatchPhi(outer.iphi < inner.iphi - 4 || outer.iphi > inner.iphi + 4);
  bool mismatch(mismatchEta || mismatchPhi);
  (void) verbose;
  // if(verbose) std::cout<<"areConsistent ? "
  //                      <<(mismatch ? "no":"yes")
  //                      <<" : inner "<<inner.str()<<", outer "<<outer.str()
  //                      <<"(mismatchEta|mismatchPhi) = ("<<mismatchEta<<" | "<<mismatchPhi<<")"
  //                      <<std::endl;
  return !mismatch;
}

Swpt::EtaPhiHalf Swpt::bandIndices(const EtaPhiHalf &inner, const EtaPhiHalf &outer) {
  // DG check that we should actually multiply *2 twice (it seems to be the case in the 1st implem)
  bool sameEta(inner.ieta == outer.ieta), samePhi(inner.iphi == outer.iphi);
  return EtaPhiHalf((sameEta ? inner.ieta * 2 : inner.ieta * 2 + 1),
                    (samePhi ? inner.iphi * 2 : inner.iphi * 2 + 1));
}
