/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "Eigen/Core"
#define EIGEN_DEVICE_FUNC 
#include "EigenHalf.h"
#undef EIGEN_DEVICE_FUNC 
#include <iostream>
#include <TMath.h>
#include <cmath>

using Eigen::half_impl::__half_raw;

const float FTK_RawTrack::s_d0_multiplier     = 1000.; // reciprocal of precision
const float FTK_RawTrack::s_z0_multiplier     = 100.; 
const float FTK_RawTrack::s_phi_multiplier    = 10000.; 
const float FTK_RawTrack::s_invpt_multiplier   = 2.e7; 
const float FTK_RawTrack::s_cot_multiplier    = 4000.; 
const float FTK_RawTrack::s_chi2_multiplier   = 1000.;
//const float FTK_RawTrack::s_quality_multiplier = 1000.;


// The Default constructor sets the version to the current version. 
// The constructors with datawords tak ethe version set in the dataword.

FTK_RawTrack::FTK_RawTrack() :
  m_word_th1((0xbda)<<16), // sector (16 bit), 0xa (4 bit), 0d (4 bit), 0xb (4 bit), reserved (3 bit), is fSSB (1 bit)
  m_word_th2(0), // layermap (12 bit), reserved (4 bit), tower (7 bit), reserved (1bit), TF num (3 bit), reserved (5 bit)
  m_word_th3((FTK_RAWTRACK_VERSION)<<24), // road (24 bit), reserved (8 bit)
  m_word_th4(0), // TRACK_D0, TRACK_CHISQ   16-bit, 16-bit
  m_word_th5(0), // TRACK_COTTH, TRACK_Z0   16-bit, 16-bit
  m_word_th6(0),  // TRACK_CURV, TRACK_PHI0  16-bit, 16-bit
  m_barcode(0)
{
  m_pix_clusters.reserve(s_npixlayers);
  m_sct_clusters.reserve(s_nsctlayers);

  // Initialise dummy Pixel clusters
  for (int i = 0; i < s_npixlayers; ++i){
    m_pix_clusters.push_back(FTK_RawPixelCluster());
  }

  // Initialise dummy SCT clusters
  for (int i = 0; i < s_nsctlayers; ++i){
    m_sct_clusters.push_back(FTK_RawSCT_Cluster());
  }
}

FTK_RawTrack::FTK_RawTrack(const FTK_RawTrack& track) {

  m_pix_clusters.reserve(track.getPixelClusters().size());
  m_sct_clusters.reserve(track.getSCTClusters().size());

  unsigned int layer=0;
  for (unsigned int ipix=0; ipix < track.getPixelClusters().size();  ++ipix,++layer){
    m_pix_clusters.push_back(track.getPixelCluster(layer));
  }
  for (unsigned int isct=0; isct < track.getSCTClusters().size();  ++isct,++layer){
    m_sct_clusters.push_back(track.getSCTCluster(layer));
  }
  m_word_th1 = track.getTH1();
  m_word_th2 = track.getTH2();
  m_word_th3 = track.getTH3();
  m_word_th4 = track.getTH4();
  m_word_th5 = track.getTH5();
  m_word_th6 = track.getTH6();

  m_barcode=track.getBarcode();
}

FTK_RawTrack::FTK_RawTrack(uint32_t word_th1, uint32_t word_th2, uint32_t word_th3, uint32_t word_th4, uint32_t word_th5, uint32_t word_th6)
{
  m_barcode = 0;
  m_word_th1 = word_th1;
  m_word_th2 = word_th2;
  m_word_th3 = word_th3;
  m_word_th4 = word_th4;
  m_word_th5 = word_th5;
  m_word_th6 = word_th6;

  m_pix_clusters.reserve(s_npixlayers);
  m_sct_clusters.reserve(s_nsctlayers);

  // Initialise dummy Pixel clusters
  for (int i = 0; i < s_npixlayers; ++i){
    m_pix_clusters.push_back(FTK_RawPixelCluster());
  }

  // Initialise dummy SCT clusters
  for (int i = 0; i < s_nsctlayers; ++i){
    m_sct_clusters.push_back(FTK_RawSCT_Cluster());
  }
} 

FTK_RawTrack::FTK_RawTrack(uint32_t word_th1, uint32_t word_th2, uint32_t word_th3, uint32_t word_th4, uint32_t word_th5, uint32_t word_th6, const std::vector<FTK_RawPixelCluster>& pixel_clusters, const std::vector<FTK_RawSCT_Cluster>& SCT_clusters){


  m_barcode = 0;
  m_word_th1 = word_th1;
  m_word_th2 = word_th2;
  m_word_th3 = word_th3;
  m_word_th4 = word_th4;
  m_word_th5 = word_th5;
  m_word_th6 = word_th6;

  // Initialise dummy Pixel clusters
  m_pix_clusters = pixel_clusters;

  // Initialise dummy SCT clusters
  m_sct_clusters = SCT_clusters;
} 

FTK_RawTrack::~FTK_RawTrack()
{
}

const FTK_RawPixelCluster& FTK_RawTrack::getPixelCluster( int v ) const { 
  if ( v > s_npixlayers || v < 0 || v > int(m_pix_clusters.size())) { 
    return m_pix_clusters[0]; 
  } else { 
    return m_pix_clusters[v]; 
  } 
} 

FTK_RawPixelCluster& FTK_RawTrack::getPixelCluster( int v ) {
  if ( v > s_npixlayers || v < 0 || v > int(m_pix_clusters.size())) {
    return m_pix_clusters[0];
  } else {
    return m_pix_clusters[v];
  }
}

uint32_t FTK_RawTrack::getPixelWordA( int v ) const {
  return m_pix_clusters[v].getWordA();
}
uint32_t FTK_RawTrack::getPixelWordB( int v ) const {
  return m_pix_clusters[v].getWordB();
}
uint32_t FTK_RawTrack::getPixelBarcode( int v ) const {
  return m_pix_clusters[v].getBarcode();
}
uint32_t FTK_RawTrack::getSCTWord( int v ) const {
  return m_sct_clusters[v].getWord();
}
uint32_t FTK_RawTrack::getSCTBarcode( int v ) const {
  return m_sct_clusters[v].getBarcode();
}

const FTK_RawSCT_Cluster& FTK_RawTrack::getSCTCluster( int v ) const { 
  if ( v > s_nsctlayers || v < 0 || v > int(m_sct_clusters.size()) ) { 
    return m_sct_clusters[0]; 
  } else { 
    return m_sct_clusters[v]; 
  } 
} 

FTK_RawSCT_Cluster& FTK_RawTrack::getSCTCluster( int v ) {
  if ( v > s_nsctlayers || v < 0 || v > int(m_sct_clusters.size()) ) {
    return m_sct_clusters[0];
  } else {
    return m_sct_clusters[v];
  }
}

void FTK_RawTrack::setPixelCluster( const unsigned int layer, const FTK_RawPixelCluster &v ) {
  if ( layer > m_pix_clusters.size()) {
    return;
  }
  m_pix_clusters[layer] = v;
  return;
}

void FTK_RawTrack::setSCTCluster( const unsigned int layer,  const FTK_RawSCT_Cluster &v ) {
  unsigned int sct_layer = layer -s_npixlayers;
  if ( sct_layer > m_sct_clusters.size()) {
    return;
  }
  m_sct_clusters[sct_layer] = v; 
  return;
}

void FTK_RawTrack::setPixelClusters( std::vector<FTK_RawPixelCluster> &clusters ) {
  m_pix_clusters = clusters;
  return;
}

void FTK_RawTrack::setSCTClusters( std::vector<FTK_RawSCT_Cluster> &clusters ) {
  m_sct_clusters = clusters;
  return;
}

void FTK_RawTrack::setRoadID(unsigned int v) {
  v = v & 0xffffff;
  m_word_th3 = (m_word_th3 | v);
}

void FTK_RawTrack::setSectorID(unsigned int sector) {
  sector = sector & 0xffff;
  m_word_th1 = (m_word_th1 | sector);
}

unsigned int FTK_RawTrack::getSectorID() const {
  return m_word_th1 & 0xffff;
}


unsigned int FTK_RawTrack::getLayerMap() const {
  return m_word_th2 & 0xfff;
}

bool FTK_RawTrack::isMissingLayer(unsigned int i) const {
  bool missingLayer=true;
  if (i<s_nlayers) {
    if ( (m_word_th2 & (0x1<<i)) !=0) missingLayer=false;
  }
    return missingLayer;
 }
      
bool FTK_RawTrack::isMissingPixelLayer(const unsigned int i) const {
  bool missingLayer=true;
  if (i<s_npixlayers) {
    if ( (m_word_th2 & (0x1<<i)) !=0) missingLayer=false;
  }
    return missingLayer;
 }
      
bool FTK_RawTrack::isMissingSCTLayer(const unsigned int i) const {
  bool missingLayer=true;
  
  int isct=i+s_npixlayers;
  if (isct<s_nlayers) {
    if ( (m_word_th2 & (0x1<<isct)) !=0) missingLayer=false;
  }
    return missingLayer;
 }
      
unsigned int FTK_RawTrack::getTower() const {
  return (m_word_th2>>16) &  0x7f;
}


void FTK_RawTrack::setLayerMap(unsigned int layerMap) {
  layerMap = layerMap & 0xfff;
  m_word_th2 = (m_word_th2 | layerMap);
}

void FTK_RawTrack::setTower(unsigned int tower) {
  tower = (tower & 0x7f) << 16;
  m_word_th2 = (m_word_th2 | tower);
}

void FTK_RawTrack::setD0(float track_d0){

  uint16_t d0 = Eigen::half(track_d0).x;

  m_word_th4 = (d0 | m_word_th4);

  return; 
}

void FTK_RawTrack::setZ0(float track_z0){

  uint16_t z0 = Eigen::half(track_z0).x;

  m_word_th5 = (z0 << 16) | m_word_th5;

  return;
}

void FTK_RawTrack::setCotTh(float track_cot){

  uint16_t cot = Eigen::half(track_cot).x;

  m_word_th5 = cot | m_word_th5;

  return;
}

double FTK_RawTrack::getCotTh() const{
  double cot_f=0.;
  uint16_t cot = m_word_th5 & 0xffff;
  if ((this->trackVersion())==0) {
    cot_f = (double) cot;
    cot_f = cot_f-s_sixteen_bit_offset;
    cot_f = cot_f/s_cot_multiplier;
  } else {
    cot_f = float(Eigen::half(__half_raw(cot)));
  }
  return cot_f;
}



void FTK_RawTrack::setPhi(float track_phi){

  uint16_t phi = Eigen::half(track_phi).x;

  m_word_th6 = (phi<<16) | m_word_th6;

  return;
}

void FTK_RawTrack::setInvPt(float track_invpt){

  uint16_t invpt = Eigen::half(track_invpt).x;

  m_word_th6 = invpt | m_word_th6;
  return;
}


double FTK_RawTrack::getInvPt() const{

  
  double invpt_f = 0.;

  if (this->trackVersion()==0) {
    uint32_t invpt = (m_word_th6 & 0xffff);
    invpt_f = (invpt-s_sixteen_bit_offset);
    invpt_f =  invpt_f /s_invpt_multiplier;
  } else {
    uint16_t invpt = (m_word_th6 & 0xffff);
    invpt_f = float(Eigen::half(__half_raw(invpt)));
  }
  return invpt_f;
}



void FTK_RawTrack::setChi2(float track_chi2){

  uint16_t chi2 = Eigen::half(track_chi2).x;
  m_word_th4 = (chi2<< 16) | m_word_th4;

  return;
}

float FTK_RawTrack::getD0() const{
  double d0_f = 0.;
  if (this->trackVersion()==0) {
    uint32_t d0=m_word_th4 & 0xffff;
    d0_f = ((double)d0 - (double)s_sixteen_bit_offset)/s_d0_multiplier;
  } else {
    uint16_t d0=m_word_th4 & 0xffff;
     d0_f = float(Eigen::half(__half_raw(d0)));
  }
  return d0_f;
    
}

float FTK_RawTrack::getZ0() const{
  double z0_f=0.;
  if (this->trackVersion()==0) {
    uint32_t z0 = (m_word_th5 & 0xffff0000) >> 16;
    z0_f = ((double)z0 - (double) s_sixteen_bit_offset)/s_z0_multiplier;
  } else {
    uint16_t z0 = (m_word_th5 & 0xffff0000) >> 16;
    z0_f = float(Eigen::half(__half_raw(z0)));
  }
  return z0_f;
}

float FTK_RawTrack::getPhi() const{
  double phi_f=0.;
  if (this->trackVersion()==0) {
    uint32_t  phi = (m_word_th6 & 0xffff0000) >> 16;
    phi_f = ((double)phi - (double) s_sixteen_bit_offset)/s_phi_multiplier;
  } else {
    uint16_t  phi = (m_word_th6 & 0xffff0000) >> 16;
    phi_f = float(Eigen::half(__half_raw(phi)));
  }
  return phi_f;
}


double FTK_RawTrack::getChi2() const{
  double chi2_f = 0.;
  if (this->trackVersion()==0) {
    uint32_t chi2 = (m_word_th4 & 0xffff0000) >> 16;
    chi2_f = (double)chi2/s_chi2_multiplier;
  } else {
    uint16_t chi2 = (m_word_th4 & 0xffff0000) >> 16;
    chi2_f = float(Eigen::half(__half_raw(chi2)));
  }
  return chi2_f;

}


void FTK_RawTrack::setIsAuxFormat(bool isAux) {
  if (isAux) {
    m_word_th2 =   m_word_th2 | 0x1000;
  } else {
    m_word_th2 =   m_word_th2 & 0xFFFFEFFF;
  }
}

bool FTK_RawTrack::getIsAuxFormat() const {
  return ((m_word_th2 & 0x1000) !=0);
}


#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawTrack;
#endif
