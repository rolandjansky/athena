/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MapEta.h"

// For reading the data files in Athena.
#ifndef LARG4_STAND_ALONE
#include "PathResolver/PathResolver.h"
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

MapEta::MapEta(int isampling)
{
  m_directory="/afs/cern.ch/atlas/offline/data/lar/calo_data";
  m_nx=0;
  m_ny=0;
  m_init=0;
  m_resp=0;
  m_xt0=0;
  m_xt1=0;
  m_xt2=0;
  Initialize(isampling);
}

MapEta::~MapEta()
{
  if (m_resp) delete [] m_resp;
  if (m_xt0) delete [] m_xt0;
  if (m_xt1) delete [] m_xt1;
  if (m_xt2) delete [] m_xt2;
}


void MapEta::SetDirectory(std::string dir)
{
  m_directory=dir;
}

void MapEta::Initialize(int isampling)
{
  if (m_init==1) return;
  if (isampling < 1 || isampling >3) return;
  std::ostringstream fn;
  if (isampling==1) fn<<"deta_strip.map";
  if (isampling==2) fn<<"deta_middle.map";
  if (isampling==3) fn<<"eta_trans.map";
  std::string filename = fn.str();
  filename=filename.substr(0,15);
  std::string fileLocation;
#ifdef LARG4_STAND_ALONE
  // The stand-alone program expects to find the file via AFS.
  fileLocation = m_directory + "/" + filename;
#else
  // In Athena, the PathResolver tool will find the file for us.
  std::cout << "filename " << filename << std::endl;
  //std::string larLocation = PathResolver::find_directory("lar","DATAPATH");
  //fileLocation=larLocation+"/calo_data/"+filename;
  std::string larLocation = PathResolver::find_directory("LArG4Barrel","ATLASCALDATA");
  fileLocation=larLocation+"/"+filename;
  std::cout << "fileLocation " << fileLocation << std::endl;
#endif

  std::ifstream in(fileLocation.c_str());
  if (in)
    {
      in>>m_nx>>m_xmin>>m_xmax>>m_ny>>m_ymin>>m_ymax;
      if(m_nx>0 && m_ny>0 && m_nx<10000 && m_ny<10000){//coverity issue. This is a tainted variable protection, 10000 can be changed if required.
        //std::cout<<"nx"<<";"<<"ny="<<m_nx<<";"<<m_ny<<std::endl;
        m_deltax=(m_xmax-m_xmin)/((float) m_nx);
        m_deltay=(m_ymax-m_ymin)/((float) m_ny);
        // what is written as xmax in the map is x of last point + delta x
        // (such that npoint = (xmax-xmin)/deltax
        // to get the real last point in the map we should subtract deltax
        m_xmax = m_xmax - m_deltax;
        m_ymax = m_ymax - m_deltay;
        m_resp = new float[m_ny*m_nx];
        m_xt0  = new float[m_ny*m_nx];
        m_xt1  = new float[m_ny*m_nx];
        m_xt2  = new float[m_ny*m_nx];
        int ii,jj;
        for (int ix=0;ix<m_nx;ix++) {
          for (int iy=0;iy<m_ny;iy++) {
            in>>ii>>jj>>m_resp[ix+iy*m_nx]>>m_xt0[ix+iy*m_nx]>>m_xt1[ix+iy*m_nx]>>m_xt2[ix+iy*m_nx];
            if (ii != ix || jj != iy) std::cout << "MapEta: inconsistency when reading map..." << std::endl;
          }
        }

        std::cout << "Eta Map read " << " Nbins " << m_nx << " " << m_ny
                  << " bin size*1000 " << 1000*m_deltax << " " << 1000*m_deltay
                  << " X range  " << m_xmin   << " " << m_xmax
                  << " Y range  " << m_ymin   << " " << m_ymax << std::endl;
        m_init=1;
      }
      else{
        std::cout << "Error in MapEta::Initialize: nx or ny out of limits." << std::endl;
        return;
      }
    }
  else{
    std::cout << "Error in MapEta::Initialize: The file could not be open." << std::endl;
    return;
  }
}

void MapEta::GetData(double x,double y,double* resp, double* xt0, double* xt1, double* xt2) const
{
  *resp=1;
  *xt0=1;
  *xt1=0;
  *xt2=0;
  if (m_nx==0 || m_ny ==0) return;
  if (x<m_xmin ) x=m_xmin+0.01*m_deltax;
  if (x>=m_xmax) x=m_xmax-0.01*m_deltax;
  if (y<m_ymin ) y=m_ymin+0.01*m_deltay;
  if (y>=m_ymax) y=m_ymax-0.01*m_deltay;

  int ix,iy;
  ix = (int) ((x-m_xmin)/m_deltax);
  iy = (int) ((y-m_ymin)/m_deltay);
  float x0 = ((float) ix)*m_deltax+m_xmin;
  float x1 = x0+m_deltax;
  float y0 = ((float) iy)*m_deltay+m_ymin;
  float y1 = y0+m_deltay;


  if (ix<0 || ix+1 >= m_nx || iy<0 || iy+1 >= m_ny) {
    std::cout << "MapEta:  Out of range " << ix << " " << iy << std::endl;
    return;
  }
  double w[4];
  w[0]=(x1-x)*(y1-y);
  w[1]=(x-x0)*(y1-y);
  w[2]=(x1-x)*(y-y0);
  w[3]=(x-x0)*(y-y0);

  double sumw=0.;
  *resp=0;
  *xt0=0;
  for (int i=0;i<2;i++) {
    for (int j=0;j<2;j++) {
      int n=ix+i+(iy+j)*m_nx;
      int m=i+2*j;
      sumw +=w[m];
      *resp += m_resp[n]*w[m];
      *xt0 += m_xt0[n]*w[m];
      *xt1 += m_xt1[n]*w[m];
      *xt2 += m_xt2[n]*w[m];
    }
  }
  const double inv_sumw = 1. / sumw;
  if (sumw>0.) {
    *resp = *resp*inv_sumw;
    *xt0 = *xt0*inv_sumw;
    *xt1 = *xt1*inv_sumw;
    *xt2 = *xt2*inv_sumw;
  }

}
void MapEta::GetData0(double x,double y,double* resp) const
{
  *resp=1;
  if (m_nx==0 || m_ny ==0) return;
  if (x<m_xmin ) x=m_xmin+0.01*m_deltax;
  if (x>=m_xmax) x=m_xmax-0.01*m_deltax;
  if (y<m_ymin ) y=m_ymin+0.01*m_deltay;
  if (y>=m_ymax) y=m_ymax-0.01*m_deltay;

  int ix,iy;
  ix = (int) ((x-m_xmin)/m_deltax);
  iy = (int) ((y-m_ymin)/m_deltay);
  float x0 = ((float) ix)*m_deltax+m_xmin;
  float x1 = x0+m_deltax;
  float y0 = ((float) iy)*m_deltay+m_ymin;
  float y1 = y0+m_deltay;

  if (ix<0 || ix+1 >= m_nx || iy<0 || iy+1 >= m_ny) {
    std::cout << "MapEta:  Out of range " << ix << " " << iy << std::endl;
    return;
  }
  double w[4];
  w[0]=(x1-x)*(y1-y);
  w[1]=(x-x0)*(y1-y);
  w[2]=(x1-x)*(y-y0);
  w[3]=(x-x0)*(y-y0);

  double sumw=0.;
  *resp = 0;
  for (int i=0;i<2;i++) {
    for (int j=0;j<2;j++) {
      int n=ix+i+(iy+j)*m_nx;
      int m=i+2*j;
      sumw +=w[m];
      *resp += m_resp[n]*w[m];
    }
  }
  if (sumw>0.) *resp = *resp/sumw;
  else *resp = 1;

}
