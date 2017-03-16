/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_MapEta_H
#define LARG4BARREL_MapEta_H

#include <string>

#define NMAP_ETA

class MapEta {
private:
   std::string m_directory;
   int m_init;
   int m_nx,m_ny;
   float m_xmin,m_xmax,m_ymin,m_ymax;
   float m_deltax,m_deltay;
   float* m_resp;
   float* m_xt0;
   float* m_xt1;
   float* m_xt2;
public:
   ~MapEta();
   MapEta(int isampling);
   void SetDirectory(std::string dir);
   void Initialize(int isampling);
   int Nx() const {return m_nx;}
   int Ny() const {return m_ny;}
   float Xmin() const {return m_xmin;}
   float Xmax() const {return m_xmax;}
   float Ymin() const {return m_ymin;}
   float Ymax() const {return m_ymax;}
   void GetData(double x,double y, double* resp, double* xt0, double* xt1, double* xt2) const;
   void GetData0(double x,double y, double* resp) const;
};
#endif //LARG4BARREL_MapEta_H
