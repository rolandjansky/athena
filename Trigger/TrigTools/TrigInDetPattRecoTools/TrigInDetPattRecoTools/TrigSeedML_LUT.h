/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_SEED_ML_LUT_H__
#define __TRIG_SEED_ML_LUT_H__

#include <vector>

typedef struct TrigSeedML_LUT {
public:
TrigSeedML_LUT() : m_id(-1), m_w(0), m_h(0), m_data(0), m_invBinWidthX(0), m_invBinWidthY(0) {};

TrigSeedML_LUT(int id, int w, int h, float c[4]) : m_id(id), m_w(w), m_h(h) {
  for(int i=0;i<4;i++) m_c[i] = c[i];
  initialize();
}

TrigSeedML_LUT(const TrigSeedML_LUT& tsl) : m_id(tsl.m_id), m_w(tsl.m_w), m_h(tsl.m_h), m_data(tsl.m_data) {
  for(int i=0;i<4;i++) m_c[i] = tsl.m_c[i];
  m_invBinWidthX = m_w/(m_c[1]-m_c[0]);
  m_invBinWidthY = m_h/(m_c[3]-m_c[2]);
}

  ~TrigSeedML_LUT() {m_data.clear();}

  bool check(float fX, float fY) const {

    int i = (fY - m_c[2])*m_invBinWidthY;
    int j = (fX - m_c[0])*m_invBinWidthX;

    if(i<0 || i>= m_h || j<0 || j>=m_w) return false;

    return (m_data[j+i*m_w] != 0);
  }

  bool getValidRange(float fX, float& min, float& max) const {
    min = m_c[2];
    max = m_c[3];
    int j = (fX - m_c[0])*m_invBinWidthX;
    if(j<0 || j>=m_w) return false;

    int idx=j;
    int i1=0;
    int i2=0;
    int i=0;
    for(;i<m_h;i++, idx+=m_w) {
      if(m_data[idx] == 0) continue;
      else {
        i1 = i;
        i2 = i1;
        break;  
      }    
    }
    for(;i<m_h;i++, idx+=m_w) {
      if(m_data[idx] != 0) i2++;
      else break;
    }

    min = m_c[2] + i1/m_invBinWidthY;
    max = m_c[2] + i2/m_invBinWidthY;
    return true;
  }
  void initialize() {
    m_invBinWidthX = m_w/(m_c[1]-m_c[0]);
    m_invBinWidthY = m_h/(m_c[3]-m_c[2]);
    m_data.resize(m_h*m_w, 0);
  }

  void setBin(int r, int c) {
    m_data[c + r*m_w] = 1;
  }

  void generate(float offset, float slope, float hwm, float hwp) {
    
    m_data.resize(m_h*m_w, 0);

    for(int j=0;j<m_w;j++) {
      float x = m_c[0] + j/m_invBinWidthX;

      float y = offset + x*slope;
      float yp= y+hwp;
      float ym= y+hwm;

      if(yp < m_c[2]) yp = m_c[2];
      if(yp > m_c[3]) yp = m_c[3];
      
      if(ym < m_c[2]) ym = m_c[2];
      if(ym > m_c[3]) ym = m_c[3];

      int i1 = int((ym-m_c[2])*m_invBinWidthY);
      int i2 = int((yp-m_c[2])*m_invBinWidthY);

      for(int i=i1;i<i2;i++) {
        m_data[j+m_w*i] = 1;
      }

    }
  }
  
  int m_id;
  int m_w, m_h;
  float m_c[4];
  std::vector<unsigned char> m_data;
  float m_invBinWidthX, m_invBinWidthY;
} TRIG_SEED_ML_LUT;

#endif
