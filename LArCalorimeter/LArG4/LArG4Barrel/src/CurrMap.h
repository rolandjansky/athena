/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_CurrMap_h
#define LARG4BARREL_CurrMap_h

#include <string>

class CurrMap {
  public:
    CurrMap(std::string filename, double xnorm);
    ~CurrMap();
    int GetNx() const       { return m_nx;}
    float GetXmin() const   { return m_xmin;}
    float GetXmax() const   { return m_xmax;}
    float GetDx() const     { return m_dx;}
    int GetNy() const       { return m_ny;}
    float GetYmin() const   { return m_ymin;}
    float GetYmax() const   { return m_ymax;}
    float GetDy() const     { return m_dy;}
    float GetGap(int ix,int iy) const {return m_gap[iy*m_nx+ix];}
    float GetCurr0(int ix,int iy) const {return m_curr0[iy*m_nx+ix];}
    float GetCurr1(int ix,int iy) const {return m_curr1[iy*m_nx+ix];}
    float GetCurr2(int ix,int iy) const {return m_curr2[iy*m_nx+ix];}
    void GetAll(double x, double y, double* gap, double* curr0,double* curr1,double* curr2) const;
  private:
    CurrMap(const CurrMap&);//coverity issue fix. Declared, but not implemented
    CurrMap& operator=(const CurrMap&);//coverity issue fix. Declared, but not implemented
    int m_nx,m_ny;
    float m_xmin,m_xmax,m_dx,m_ymin,m_ymax,m_dy;
    float *m_gap,*m_curr0,*m_curr1,*m_curr2;
    float m_norm;

};

#endif // LARG4BARREL_CurrMap_h
