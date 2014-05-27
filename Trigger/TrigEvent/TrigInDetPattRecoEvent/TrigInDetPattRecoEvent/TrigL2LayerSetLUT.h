/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_LAYER_SET_LUT_H__
#define __TRIG_L2_LAYER_SET_LUT_H__

#include<cstring>

typedef struct TrigL2LayerSetLUT {
public:
TrigL2LayerSetLUT(float zmax, float tmax, int nbz, int nbt, int nL) : 
  m_zMax(zmax), m_tMax(tmax), m_nBinZ(nbz), m_nBinTheta(nbt), m_nLayers(nL) {
  m_lut = new short[m_nBinZ*m_nBinTheta*m_nLayers];
  memset(m_lut, 0, sizeof(short)*m_nBinZ*m_nBinTheta*m_nLayers);
  m_zBin = 2*m_zMax/m_nBinZ;
  m_tBin = 2*m_tMax/m_nBinTheta;
  m_nBZ = (m_nBinZ-1)/2;
  m_nBT = (m_nBinTheta-1)/2;
}
  ~TrigL2LayerSetLUT() {
    delete[] m_lut;
  }
  bool addSet(const short& src, float z, float t) {

    int zbin = (fabs(z)<=m_zBin) ? 0 : 1+(int)(fabs(z)/m_zBin-0.5);
    if(z<0.0) zbin = -zbin;
    int tbin = (fabs(t)<=m_tBin) ? 0 : 1+(int)(fabs(t)/m_tBin-0.5);
    if(t<0.0) tbin = -tbin;

    if(abs(zbin)>m_nBZ) return false;
    if(abs(tbin)>m_nBT) return false;
    int idx = m_nLayers*(tbin+m_nBT+(zbin+m_nBZ)*m_nBinTheta);
    memcpy(&m_lut[idx], &src, sizeof(short)*m_nLayers);
    return true;
  }
  const short* getSet(float z, float t) const {
    int zbin = (fabs(z)<=m_zBin) ? 0 : 1+(int)(fabs(z)/m_zBin-0.5);
    if(z<0.0) zbin = -zbin;
    int tbin = (fabs(t)<=m_tBin) ? 0 : 1+(int)(fabs(t)/m_tBin-0.5);
    if(t<0.0) tbin = -tbin;
    if(abs(zbin)>m_nBZ) return NULL;
    if(abs(tbin)>m_nBT) return NULL;
    int idx = m_nLayers*(tbin+m_nBT+(zbin+m_nBZ)*m_nBinTheta);
    return &m_lut[idx];
  }

  float m_zMax, m_tMax;
  int m_nBinZ, m_nBinTheta, m_nLayers, m_nBT, m_nBZ;
  float m_zBin, m_tBin;
  short* m_lut;
private:
TrigL2LayerSetLUT() : m_lut(NULL) {};
} TRIG_L2_LAYER_SET_LUT;


#endif
