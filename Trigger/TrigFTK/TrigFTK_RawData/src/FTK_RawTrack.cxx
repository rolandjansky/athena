/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawTrack.h"

#include <iostream>
#include <TMath.h>
#include <cmath>

using namespace std;

const float FTK_RawTrack::d0_multiplier     = 1000.; // reciprocal of precision
const float FTK_RawTrack::z0_multiplier     = 100.; 
const float FTK_RawTrack::phi_multiplier    = 10000.; 
const float FTK_RawTrack::invpt_multiplier   = 2.e7; 
const float FTK_RawTrack::cot_multiplier    = 4000.; 
const float FTK_RawTrack::chi2_multiplier   = 1000.;
//const float FTK_RawTrack::quality_multiplier = 1000.;


FTK_RawTrack::FTK_RawTrack() :
  m_word_th1((0xbda)<<16), // sector (16 bit), 0xa (4 bit), 0d (4 bit), 0xb (4 bit), reserved (3 bit), is fSSB (1 bit)
  m_word_th2(0), // layermap (12 bit), reserved (4 bit), tower (7 bit), reserved (1bit), TF num (3 bit), reserved (5 bit)
  m_word_th3(0), // road (24 bit), reserved (8 bit)
  m_word_th4(0), // TRACK_D0, TRACK_CHISQ   16-bit, 16-bit
  m_word_th5(0), // TRACK_COTTH, TRACK_Z0   16-bit, 16-bit
  m_word_th6(0),  // TRACK_CURV, TRACK_PHI0  16-bit, 16-bit
  m_barcode(0)
{
  m_pix_clusters.reserve(npixlayers);
  m_sct_clusters.reserve(nsctlayers);

  // Initialise dummy Pixel clusters
  for (int i = 0; i < npixlayers; ++i){
    m_pix_clusters.push_back(FTK_RawPixelCluster(FTK_RawPixelCluster::missingLayer));
  }

  // Initialise dummy SCT clusters
  for (int i = 0; i < nsctlayers; ++i){
    m_sct_clusters.push_back(FTK_RawSCT_Cluster(FTK_RawPixelCluster::missingLayer));
  }
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

  m_pix_clusters.reserve(npixlayers);
  m_sct_clusters.reserve(nsctlayers);

  // Initialise dummy Pixel clusters
  for (int i = 0; i < npixlayers; ++i){
    m_pix_clusters.push_back(FTK_RawPixelCluster(FTK_RawPixelCluster::missingLayer));
  }

  // Initialise dummy SCT clusters
  for (int i = 0; i < nsctlayers; ++i){
    m_sct_clusters.push_back(FTK_RawSCT_Cluster(FTK_RawPixelCluster::missingLayer));
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
  if ( v > npixlayers || v < 0 || v > int(m_pix_clusters.size())) { 
    return m_pix_clusters[0]; 
  } else { 
    return m_pix_clusters[v]; 
  } 
} 

FTK_RawPixelCluster& FTK_RawTrack::getPixelCluster( int v ) {
  if ( v > npixlayers || v < 0 || v > int(m_pix_clusters.size())) {
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
  if ( v > nsctlayers || v < 0 || v > int(m_sct_clusters.size()) ) { 
    return m_sct_clusters[0]; 
  } else { 
    return m_sct_clusters[v]; 
  } 
} 

FTK_RawSCT_Cluster& FTK_RawTrack::getSCTCluster( int v ) {
  if ( v > nsctlayers || v < 0 || v > int(m_sct_clusters.size()) ) {
    return m_sct_clusters[0];
  } else {
    return m_sct_clusters[v];
  }
}

void FTK_RawTrack::setPixelCluster( const FTK_RawPixelCluster &v ) {
  int layer = v.getLayer();
  if ( layer > npixlayers || layer < 0 || layer > int(m_pix_clusters.size())) {
    return;
  }
  m_pix_clusters[layer] = v;
  return;
}

void FTK_RawTrack::setSCTCluster( const FTK_RawSCT_Cluster &v ) {
  int layer = v.getLayer();
  if ( layer > nsctlayers || layer < 0 || layer > int(m_sct_clusters.size())) {
    return;
  }
  m_sct_clusters[layer] = v;
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

void FTK_RawTrack::setLayerMap(unsigned int layerMap) {
  layerMap = layerMap & 0xfff;
  m_word_th2 = (m_word_th2 | layerMap);
}


void FTK_RawTrack::setD0(float track_d0){

  float d0_f =  track_d0 * d0_multiplier + (float) sixteen_bit_offset;
  if (d0_f>65535.) d0_f = 65535.;
  else if (d0_f < 0.) d0_f = 0;

  uint32_t d0 = round(d0_f);
  m_word_th4 = (d0 | m_word_th4);

  return; 
}

void FTK_RawTrack::setZ0(float track_z0){

  float z0_f =  track_z0 *  z0_multiplier + (float)sixteen_bit_offset;
  if (z0_f>65535.) z0_f = 65535.;
  else if (z0_f < 0.) z0_f = 0;
  
  uint32_t z0 = round(z0_f);

  z0 = z0 << 16;

  m_word_th5 = z0 | m_word_th5;

  return;
}

void FTK_RawTrack::setCotTh(float track_cot){
  float cot_f = track_cot * cot_multiplier + (float)sixteen_bit_offset;

  if (cot_f>65535.) cot_f = 65535.;
  if (cot_f < 0.) cot_f = 0.;
  uint32_t cot = round(cot_f);
  m_word_th5 = cot | m_word_th5;

  return;
}

double FTK_RawTrack::getCotTh() const{
  uint16_t cot = m_word_th5 & 0xffff;
  double cot_f = (double) cot;
  cot_f = cot_f-sixteen_bit_offset;
  cot_f = cot_f/cot_multiplier;

  return cot_f;
}



void FTK_RawTrack::setPhi(float track_phi){

  uint32_t phi = 0;
  while (track_phi>TMath::Pi()) track_phi -=TMath::Pi(); // get phi in range -pi to pi
  phi  = round( phi_multiplier*track_phi + sixteen_bit_offset);
  phi = phi << 16;
  m_word_th6 = phi | m_word_th6;
  return;
}

void FTK_RawTrack::setInvPt(float track_invpt){


  float ptinv = track_invpt * invpt_multiplier + (float) sixteen_bit_offset;
  if (ptinv<0.) ptinv=0.;
  else if (ptinv>65535.) ptinv=65535.;
  uint16_t invpt = round(ptinv); 
  m_word_th6 = invpt | m_word_th6;
  return;
}


double FTK_RawTrack::getInvPt() const{

  uint32_t invpt = (m_word_th6 & 0xffff);
  
  double invpt_f = (double) invpt;
  invpt_f = (invpt_f-sixteen_bit_offset);
  invpt_f =  invpt_f /invpt_multiplier;
  return invpt_f;
}



void FTK_RawTrack::setChi2(float track_chi2){

  float ch2_f = track_chi2* chi2_multiplier;
  if (ch2_f > 65535.)ch2_f = 65535.; 
  uint32_t chi2 = round(ch2_f);
  chi2 = chi2 << 16;
  m_word_th4 = chi2 | m_word_th4;

  return;
}

// ununsed now
// void FTK_RawTrack::setQuality(float track_qual){
  
//   float q_f = track_qual * quality_multiplier;
//   if (q_f > 65535.) q_f = 65535.;
//   else if (q_f < 0) q_f = 0.;
//   uint32_t qual = round (q_f);
//   qual = qual << 16;
//   m_word_th5 = qual | m_word_th5;

//   return;
// }

float FTK_RawTrack::getD0() const{
  uint32_t d0=m_word_th4 & 0xffff;
  double d0_f = ((double)d0 - (double)sixteen_bit_offset)/d0_multiplier;
  return d0_f;
}

float FTK_RawTrack::getZ0() const{
  uint32_t z0 = (m_word_th5 & 0xffff0000) >> 16;
  double z0_f = ((double)z0 - (double) sixteen_bit_offset)/z0_multiplier;
  return z0_f;
}

float FTK_RawTrack::getPhi() const{
  uint32_t phi = (m_word_th6 & 0xffff0000) >> 16;
  double d0_f = ((double)phi - (double) sixteen_bit_offset)/phi_multiplier;
  return d0_f;
}


double FTK_RawTrack::getChi2() const{
  uint32_t chi2 = (m_word_th4 & 0xffff0000) >> 16;
  double chi2_f = (double)chi2/chi2_multiplier;
  return chi2_f;
}

// double FTK_RawTrack::getQuality() const{
//   uint32_t qual = m_word_th5 >> 16;
//   double qual_f = (double) qual /quality_multiplier;
//   return qual_f;
// }

#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawTrack;
#endif
