#ifndef ROI_H
#define ROI_H

#include "Ntuple.h"
#include "RoIBase.h"


class BjetTag {

 public:

  BjetTag(){m_xIP1d=m_xIP2d=m_xIP3d=m_xChi2=m_xsvtx=m_xmvtx=m_xevtx=m_xnvtx=m_xcomb=0;};

  float xIP1D() {return m_xIP1d;};
  float xIP2D() {return m_xIP2d;};
  float xIP3D() {return m_xIP3d;};
  float xCHI2() {return m_xChi2;};
  float xSVtx() {return m_xsvtx;};
  float xMVtx() {return m_xmvtx;};
  float xEVtx() {return m_xevtx;};
  float xNVtx() {return m_xnvtx;};
  float xComb() {return m_xcomb;};

  void xIP1D(float tmp) {m_xIP1d=tmp;};
  void xIP2D(float tmp) {m_xIP2d=tmp;};
  void xIP3D(float tmp) {m_xIP3d=tmp;};
  void xCHI2(float tmp) {m_xChi2=tmp;};
  void xSVtx(float tmp) {m_xsvtx=tmp;};
  void xMVtx(float tmp) {m_xmvtx=tmp;};
  void xEVtx(float tmp) {m_xevtx=tmp;};
  void xNVtx(float tmp) {m_xnvtx=tmp;};
  void xComb(float tmp) {m_xcomb=tmp;};

 private:  

  float m_xIP1d, m_xIP2d, m_xIP3d, m_xChi2, m_xsvtx, m_xmvtx, m_xevtx, m_xnvtx, m_xcomb;
  
};

class RoI : public RoIBase {

 public:

  RoI(double phi,double eta);

  void   SetWeights(Ntuple*);
  void   SetVerb(bool lverbval){lverb=lverbval;};
  bool   lverb;

  BjetTag l2Tag;   
  BjetTag efTag;   
   
};


#endif
