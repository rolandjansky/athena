/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CurrMap.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

CurrMap::CurrMap(std::string filename,double xnorm)
{
  // normalisation to convert nA from the map back to MeV
  // use average in straight section:
  //  this assumes that the electronic calibration is such
  //  that Sum ai*(ADC-PED) gives the initial current for a triangular
  //   signal of drift time ~475 ns
  m_norm = xnorm;
  m_nx=0;
  m_ny=0;
  m_xmin=0.;
  m_xmax=0.;
  m_dx=0.;
  m_ymin=0.;
  m_ymax=0.;
  m_dy=0.;

  m_gap=0;
  m_curr0=0;
  m_curr1=0;
  m_curr2=0;


  FILE * fp = fopen(filename.c_str(),"r");
  if(fp){
    char line[81];
    fgets(line,80,fp);
    sscanf(&line[0],"%d%f%f%d%f%f",&m_nx,&m_xmin,&m_xmax,&m_ny,&m_ymin,&m_ymax);

    if(m_nx>0 && m_ny>0 && m_nx<10000 && m_ny<10000) {//coverity issue. This is a tainted variable protection, 10000 can be changed if required.
      m_dx = (m_xmax-m_xmin)/((float) m_nx);
      m_dy = (m_ymax-m_ymin)/((float) m_ny);
      //std::cout << "  nx,xmin,xmax,ny,ymin,ymax " << m_nx << " " << m_xmin << " " << m_xmax << " "
      //                                             << m_ny << " " << m_ymin << " " << m_ymax << std::endl;

      m_gap = new float[m_nx*m_ny];
      m_curr0 = new float[m_nx*m_ny];
      m_curr1 = new float[m_nx*m_ny];
      m_curr2 = new float[m_nx*m_ny];

      for (int i=0; i<m_nx;i++)
        {
          for (int j=0; j<m_ny;j++) {
            m_gap[j*m_nx+i]=0.;
            m_curr0[j*m_nx+i]=0;
            m_curr1[j*m_nx+i]=0;
            m_curr2[j*m_nx+i]=0;
          }
        }

      int ix,iy;
      float gap,cur1,cur2,cur3;
      while (fgets(line,80,fp)) {
        sscanf(&line[0],"%d%d%f%f%f%f",&ix,&iy,&gap,&cur1,&cur2,&cur3);
        if(ix >= 0 && ix < m_nx && iy >= 0 && iy < m_ny){
          m_gap[iy*m_nx+ix]=gap;
          m_curr0[iy*m_nx+ix]=cur1/m_norm;
          m_curr1[iy*m_nx+ix]=cur2/m_norm;
          m_curr2[iy*m_nx+ix]=cur3/m_norm;
          //      std::cout << "read " << ix << " " << iy << " " << m_curr0[iy*m_nx+ix] << std::endl;
        }
      }
    }
    else{
      std::cout << "Error in CurrMap::CurrMap: nx or ny out of limits." << std::endl;
    }
    fclose(fp);
  }
  else{

    std::cout << "Error in CurrMap::CurrMap: The file could not be open." << std::endl;
  }


}


CurrMap::~CurrMap()
{
  if (m_gap) delete [] m_gap;
  if (m_curr0) delete [] m_curr0;
  if (m_curr1) delete [] m_curr1;
  if (m_curr2) delete [] m_curr2;
}


void CurrMap::GetAll(double x, double y, double* gap, double* curr0, double* curr1, double* curr2) const
{
  *gap=0;
  *curr0=0;
  *curr1=0;
  *curr2=0;

  if (m_nx==0 || m_ny ==0) return;
  if (x<m_xmin ) x=m_xmin;
  if (x>=m_xmax ) x=m_xmax-0.0001;
  if (y<m_ymin ) y=m_ymin;
  if (y>=m_ymax ) y=m_ymax-0.0001;

  int ix,iy;
  ix = (int) ((x-m_xmin)/m_dx);
  iy = (int) ((y-m_ymin)/m_dy);

  if ( (ix+1) < m_nx && (iy+1) < m_ny) {
    float x0 = ((float) ix)*m_dx+m_xmin;
    float x1 = x0+m_dx;
    float y0 = ((float) iy)*m_dy+m_ymin;
    float y1 = y0+m_dy;
    float w[4];
    w[0]=(x1-x)*(y1-y);
    w[1]=(x-x0)*(y1-y);
    w[2]=(x1-x)*(y-y0);
    w[3]=(x-x0)*(y-y0);

    float sumw=0.;
    for (int i=0;i<2;i++) {
      for (int j=0;j<2;j++) {
        int n=ix+i+(iy+j)*m_nx;
        if (m_curr0[n]>0) {
          int m=i+2*j;
          sumw +=w[m];
          *gap += m_gap[n]*w[m];
          *curr0 += m_curr0[n]*w[m];
          *curr1 += m_curr1[n]*w[m];
          *curr2 += m_curr2[n]*w[m];
        }
      }
    }
    if (sumw>0.) {
      *gap = *gap/sumw;
      *curr0 = *curr0/sumw;
      *curr1 = *curr1/sumw;
      *curr2 = *curr2/sumw;
    }
    else {
      // try to recover a non zero current by moving in the map
      int jy=-1;
      int jx=-1;
      int idistm=100;
      for (int iiy=iy-7;iiy<iy+8;iiy++) {
        for (int iix=ix-7;iix<ix+8;iix++) {
          if (iiy>=0 && iiy<m_ny && iix>=0 && iix<m_nx) {
            if (m_curr0[iix+iiy*m_nx]>0) {
              int idist=(iix-ix)*(iix-ix)+(iiy-iy)*(iiy-iy);
              if(idist<idistm) {
                idistm=idist;
                jx=iix;
                jy=iiy;
              }
            }
          }
        }
      }
      if (idistm<100 && jx>=0 && jy>=0) {
        *gap = m_gap[jx+jy*m_nx];
        *curr0 = m_curr0[jx+jy*m_nx];
        *curr1 = m_curr1[jx+jy*m_nx];
        *curr2 = m_curr2[jx+jy*m_nx];
      } else {
        *gap=0;
        *curr0=0;
        *curr1=0;
        *curr2=0;
      }
    }
  }
  else {
    // on the edges of the map, no linear interpolation
    int n=ix+iy*m_nx;
    if (m_curr0[n]<1e-6 && (ix+1) <m_nx) n=ix+1 +iy*m_nx;
    if (m_curr0[n]<1e-6 && (ix-1) >0 ) n=ix-1 +iy*m_nx;
    if (m_curr0[n]<1e-6 && (iy+1) <m_ny) n=ix+(iy+1)*m_nx;
    if (m_curr0[n]<1e-6 && (iy-1) <0) n=ix+(iy-1)*m_nx;
    if (m_curr0[n]<1e-6 && (ix+1) < m_nx && (iy+1) < m_ny) n=ix+1+(iy+1)*m_nx;
    if (m_curr0[n]<1e-6 && (ix-1) >0 && (iy+1) < m_ny)     n=ix-1+(iy+1)*m_nx;
    if (m_curr0[n]<1e-6 && (ix+1) < m_nx && (iy-1) >0 )    n=ix+1+(iy-1)*m_nx;
    if (m_curr0[n]<1e-6 && (ix-1) >0  && (iy-1) >0 )       n=ix-1+(iy-1)*m_nx;
    *gap = m_gap[n];
    *curr0 = m_curr0[n];
    *curr1 = m_curr1[n];
    *curr2 = m_curr2[n];
  }
}
