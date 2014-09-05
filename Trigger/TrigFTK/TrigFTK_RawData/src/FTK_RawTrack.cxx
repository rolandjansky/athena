/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawTrack.h"

#include <iostream>
#include <TMath.h>
#include <cmath>

using namespace std;

const float FTK_RawTrack::d0_precision     = 1000.; 
const float FTK_RawTrack::z0_precision     = 100.; 
const float FTK_RawTrack::phi_precision    = 10000.; 
const float FTK_RawTrack::curv_precision   = 5.*10e6; 
const float FTK_RawTrack::cot_precision    = 5000.; 

FTK_RawTrack::FTK_RawTrack() :
  m_word_th1(0), // ROAD_ID                   32-bit
  m_word_th2(0), // TRACK_Z0,   TRACK_D0      16-bit, 16-bit
  m_word_th3(0), // TRACK_PHI0, TRACK_COTTH   16-bit, 16-bit
  m_word_th4(0), // TRACK_CURV, TRACK_CHISQ   16-bit, 16-bit
  m_word_th5(0)  // RSV,        TRACK_QUALITY 16-bit, 16-bit
{
  m_pix_clusters.reserve(npixlayers);
  m_sct_clusters.reserve(nsctlayers);

  // Initialise dummy Pixel clusters
  for (int i = 0; i < npixlayers; ++i){
    m_pix_clusters.push_back(FTK_RawPixelCluster(i));
  }

  // Initialise dummy SCT clusters
  for (int i = 0; i < nsctlayers; ++i){
    m_sct_clusters.push_back(FTK_RawSCT_Cluster(i));
  }
}

FTK_RawTrack::FTK_RawTrack(uint32_t word_th1, uint32_t word_th2, uint32_t word_th3, uint32_t word_th4, uint32_t word_th5){
  m_word_th1 = word_th1;
  m_word_th2 = word_th2;
  m_word_th3 = word_th3;
  m_word_th4 = word_th4;
  m_word_th5 = word_th5;

  m_pix_clusters.reserve(npixlayers);
  m_sct_clusters.reserve(nsctlayers);

  // Initialise dummy Pixel clusters
  for (int i = 0; i < npixlayers; ++i){
    m_pix_clusters.push_back(FTK_RawPixelCluster(i));
  }

  // Initialise dummy SCT clusters
  for (int i = 0; i < nsctlayers; ++i){
    m_sct_clusters.push_back(FTK_RawSCT_Cluster(i));
  }
} 

FTK_RawTrack::FTK_RawTrack(uint32_t word_th1, uint32_t word_th2, uint32_t word_th3, uint32_t word_th4, uint32_t word_th5, const std::vector<FTK_RawPixelCluster>& pixel_clusters, const std::vector<FTK_RawSCT_Cluster>& SCT_clusters){
  m_word_th1 = word_th1;
  m_word_th2 = word_th2;
  m_word_th3 = word_th3;
  m_word_th4 = word_th4;
  m_word_th5 = word_th5;

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

void FTK_RawTrack::setPixelCluster( FTK_RawPixelCluster v ) {
  int layer = v.getLayer();
  if ( layer > npixlayers || layer < 0 || layer > int(m_pix_clusters.size())) {
    return;
  }
  m_pix_clusters[layer] = v;
  return;
}

void FTK_RawTrack::setSCTCluster( FTK_RawSCT_Cluster v ) {
  int layer = v.getLayer();
  if ( layer > nsctlayers || layer < 0 || layer > int(m_sct_clusters.size())) {
    return;
  }
  m_sct_clusters[layer] = v;
  return;
}

void FTK_RawTrack::setPixelClusters( std::vector<FTK_RawPixelCluster> clusters ) {
  m_pix_clusters = clusters;
  return;
}

void FTK_RawTrack::setSCTClusters( std::vector<FTK_RawSCT_Cluster> clusters ) {
  m_sct_clusters = clusters;
  return;
}

void FTK_RawTrack::setD0(float track_d0){
  
  uint32_t d0 = 0;
  if(!(fabs(track_d0>32.767)))
    d0 = round( d0_precision*track_d0 + sixteen_bit_offset);

  d0 = d0 << 16;
  m_word_th2 = (d0 | m_word_th2);

  return; 
}

void FTK_RawTrack::setZ0(float track_z0){

  uint16_t z0 = 0;
  if(!(fabs(track_z0)>327.67))
    z0 = round( z0_precision*track_z0 + sixteen_bit_offset);

  m_word_th2 = z0 | m_word_th2;

  return;
}

void FTK_RawTrack::setCotTh(float track_cot){

  uint32_t cot = 0;
  if(!(fabs(track_cot)>6.5))
    cot  = round( cot_precision*track_cot + sixteen_bit_offset);

  cot = cot << 16;
  m_word_th3 = cot | m_word_th3;

  return;
}

void FTK_RawTrack::setPhi(float track_phi){

  uint16_t phi = 0;
  if(!(fabs(track_phi>TMath::Pi())))
    phi  = round( phi_precision*track_phi + sixteen_bit_offset);

  m_word_th3 = phi | m_word_th3;
  return;
}

void FTK_RawTrack::setCurv(float track_curv){

  uint16_t curv = 0;
  if(!(fabs(track_curv)>66))
    curv = round( curv_precision*track_curv + sixteen_bit_offset);

  m_word_th4 = curv | m_word_th4;

  return;
}

int FTK_RawTrack::return_bits(int low, int high, uint32_t word) const{
  word = word << (32-high);
  word = word >> low;
  return (int)word;
}

float FTK_RawTrack::getD0() const{
  double d0_f = (double)((return_bits(16, 32, m_word_th2) - sixteen_bit_offset)/d0_precision);
  return d0_f;
}

float FTK_RawTrack::getZ0() const{
  uint32_t d0 = m_word_th2;
  d0 = d0 << 16;
  d0 = d0 >> 16;
  double d0_f = (double)(((int)(d0) - sixteen_bit_offset)/z0_precision);
  return d0_f;
}

float FTK_RawTrack::getPhi() const{
  uint32_t d0 = m_word_th3;
  d0 = d0 << 16;
  d0 = d0 >> 16;
  double d0_f = (double)(((int)(d0) - sixteen_bit_offset)/phi_precision);
  return d0_f;
}

double FTK_RawTrack::getCotTh() const{
  uint16_t cot = 0;
  double cot_f = (double)(((int)(cot | (m_word_th3 >> 16)) - sixteen_bit_offset)/cot_precision);
  return cot_f;
}

double FTK_RawTrack::getCurv() const{
  uint16_t curv = 0;
  double curv_f = (double)(((int)(curv | (m_word_th4)) - sixteen_bit_offset)/curv_precision);
  return curv_f;
}


#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawTrack;
#endif
