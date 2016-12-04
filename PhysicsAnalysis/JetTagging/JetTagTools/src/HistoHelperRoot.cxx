/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
///    Author: A. Wildauer
///    CERN, January 14, 2005
///
///    Transition from AIDA to THistSvc (root)
///      and enhancement of functionality (smoothing, interpolation):
///    L. Vacavant, JB Devivie
///
/////////////////////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/HistoHelperRoot.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace Analysis
{

HistoHelperRoot::HistoHelperRoot(ITHistSvc* histoSvc) :
  m_histoSvc(histoSvc),
  m_histoMap1D(std::map<std::string, TH1*>()),
  m_histoMap2D(std::map<std::string, TH2*>()),
  m_histoMap3D(std::map<std::string, TH3*>()),
  m_histoLimitsMap1D(std::map<std::string, HistoLimits>()),
  m_histoLimitsMap2D(std::map<std::string, HistoLimits>()),
  m_histoLimitsMap3D(std::map<std::string, HistoLimits>()),
  m_checkOverflows(true)
  {}


HistoHelperRoot::~HistoHelperRoot() {}

std::string HistoHelperRoot::baseName(const std::string& fullHistoName) {
    const std::string delim("/");
    std::vector<std::string> words;
    std::string::size_type sPos, sEnd, sLen;
    sPos = fullHistoName.find_first_not_of(delim);
    while ( sPos != std::string::npos ) {
	sEnd = fullHistoName.find_first_of(delim, sPos);
	if(sEnd==std::string::npos) sEnd = fullHistoName.length();
	sLen = sEnd - sPos;
	std::string word = fullHistoName.substr(sPos,sLen);
	words.push_back(word);
	sPos = fullHistoName.find_first_not_of(delim, sEnd);
    }
    std::string base = "";
    if(words.size()>0) base = words[words.size()-1];
    //std::cout<<"baseName: decoding "<<fullHistoName<<" --> "<<base<<std::endl;
    return base;
}

void HistoHelperRoot::bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int bins, double minx, double maxx)
{
    m_histoMap1D[histoName] = new TH1F(this->baseName(histoName).c_str(),histoTitle.c_str(),bins,minx,maxx);
    //std::cout<<"HistoHelperRoot: booking 1D "<<histoName<<std::endl;
    if (!m_histoSvc->regHist(histoName,m_histoMap1D[histoName]).isSuccess()) {
      std::cout <<"Cannot book histo " << histoName << " with Title " << histoTitle <<std::endl;
    }
    m_histoLimitsMap1D[histoName] = HistoLimits(bins, minx, maxx);
}

void HistoHelperRoot::bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int bins, double* edge)
{
    m_histoMap1D[histoName] = new TH1F(this->baseName(histoName).c_str(),histoTitle.c_str(),bins,edge);
    if (!m_histoSvc->regHist(histoName,m_histoMap1D[histoName]).isSuccess()) {
      std::cout <<"Cannot book histo " << histoName << " with Title " << histoTitle <<std::endl;
    }
    m_histoLimitsMap1D[histoName] = HistoLimits(bins, edge[0], edge[bins]);
}

void HistoHelperRoot::bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int binsx, double minx, double maxx, unsigned int binsy, double miny, double maxy)
{
    m_histoMap2D[histoName] = new TH2F(this->baseName(histoName).c_str(),histoTitle.c_str(),binsx,minx,maxx,binsy,miny,maxy);
    if (!m_histoSvc->regHist(histoName,m_histoMap2D[histoName]).isSuccess()) {
      std::cout <<"Cannot book histo " << histoName << " with Title " << histoTitle <<std::endl;
    }
    m_histoLimitsMap2D[histoName] = HistoLimits(binsx, minx, maxx, binsy, miny, maxy);
}

void HistoHelperRoot::bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int binsx,  double* edgex, unsigned int binsy, double* edgey)
{
    m_histoMap2D[histoName] = new TH2F(this->baseName(histoName).c_str(),histoTitle.c_str(),binsx,edgex,binsy,edgey);
    if (!m_histoSvc->regHist(histoName,m_histoMap2D[histoName]).isSuccess()) {
      std::cout <<"Cannot book histo " << histoName << " with Title " << histoTitle <<std::endl;
    }
    m_histoLimitsMap2D[histoName] = HistoLimits(binsx, edgex[0], edgex[binsx], binsy, edgey[0], edgey[binsy]);
}

void HistoHelperRoot::bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int binsx, double minx, double maxx, unsigned int binsy, double miny, double maxy, unsigned int binsz, double minz, double maxz)
{
    m_histoMap3D[histoName] = new TH3F(this->baseName(histoName).c_str(),histoTitle.c_str(),binsx,minx,maxx,binsy,miny,maxy,binsz,minz,maxz);
    if (!m_histoSvc->regHist(histoName,m_histoMap3D[histoName]).isSuccess()) {
      std::cout <<"Cannot book histo " << histoName << " with Title " << histoTitle <<std::endl;
    }
    m_histoLimitsMap3D[histoName] = HistoLimits(binsx, minx, maxx, binsy, miny, maxy, binsz, minz, maxz);
}



void HistoHelperRoot::fillHisto(const std::string& histoName, double value)
{
  /** make sure that underflow/overflow is filled in 1st/last bin and not
  in 1st-1 and last+1 bin */
  if (m_checkOverflows) {
    const HistoLimits& l = (*(m_histoLimitsMap1D.find(histoName))).second;
    if ( value < l.xmin ) value = l.xmin+0.0001;
    else if ( value > l.xmax ) value = l.xmax-0.0001;
  }
  m_histoMap1D[histoName]->Fill(value);
}

void HistoHelperRoot::fillHistoWithWeight(const std::string& histoName, double value, double weight)
{
  /** make sure that underflow/overflow is filled in 1st/last bin and not
  in 1st-1 and last+1 bin */
  if (m_checkOverflows) {
    const HistoLimits& l = (*(m_histoLimitsMap1D.find(histoName))).second;
    if ( value < l.xmin ) value = l.xmin+0.0001;
    else if ( value > l.xmax ) value = l.xmax-0.0001;
  }
  m_histoMap1D[histoName]->Fill(value,weight);
}

void HistoHelperRoot::fillHisto(const std::string& histoName, double valuex, double valuey)
{
  /** make sure that underflow/overflow is filled in 1st/last bin and not
  in 1st-1 and last+1 bin */
  if (m_checkOverflows) {
    const HistoLimits& l = (*(m_histoLimitsMap2D.find(histoName))).second;
    if ( valuex < l.xmin ) valuex = l.xmin+0.0001;
    else if ( valuex > l.xmax ) valuex = l.xmax-0.0001;
    if ( valuey < l.ymin ) valuey = l.ymin+0.0001;
    else if ( valuey > l.ymax ) valuey = l.ymax-0.0001;
  }
  m_histoMap2D[histoName]->Fill(valuex,valuey);
}
void HistoHelperRoot::fillHisto(const std::string& histoName, double valuex, double valuey, double valuez)
{
  /** make sure that underflow/overflow is filled in 1st/last bin and not
  in 1st-1 and last+1 bin */
  if (m_checkOverflows) {
    const HistoLimits& l = (*(m_histoLimitsMap3D.find(histoName))).second;
    if ( valuex < l.xmin ) valuex = l.xmin+0.0001;
    else if ( valuex > l.xmax ) valuex = l.xmax-0.0001;
    if ( valuey < l.ymin ) valuey = l.ymin+0.0001;
    else if ( valuey > l.ymax ) valuey = l.ymax-0.0001;
    if ( valuez < l.zmin ) valuez = l.zmin+0.0001;
    else if ( valuez > l.zmax ) valuez = l.zmax-0.0001;
  }
  m_histoMap3D[histoName]->Fill(valuex,valuey,valuez);
}

TH1* HistoHelperRoot::getHisto1D(const std::string& histoName)
{
  return m_histoMap1D[histoName];
}
TH2* HistoHelperRoot::getHisto2D(const std::string& histoName)
{
  return m_histoMap2D[histoName];
}
TH3* HistoHelperRoot::getHisto3D(const std::string& histoName)
{
  return m_histoMap3D[histoName];
}

void HistoHelperRoot::normalizeHistos()
{
  //std::cout <<"In HistoHelperRoot::normalizeHistos()" << std::endl;
  for (std::map<std::string, TH1*>::iterator mapItr = m_histoMap1D.begin();
       mapItr != m_histoMap1D.end(); ++mapItr ) {
    const std::string& name = (*mapItr).first;
    //
    TH1* histo = (*mapItr).second;
    double norm(histo->Integral());
    if (norm!=1. && norm != 0.) {
      //std::cout << "Normalizing: " << name << std::endl;
      histo->Scale(1./norm);
    } else if (norm == 0.) {
      std::cout << "Empty histogram ! "<< name <<std::endl;
    }
  }
  for (std::map<std::string, TH2*>::iterator mapItr = m_histoMap2D.begin();
       mapItr != m_histoMap2D.end(); ++mapItr ) {
    const std::string& name = (*mapItr).first;
    //
    TH2* histo = (*mapItr).second;
    double norm(histo->Integral());
    if (norm!=1. && norm != 0.) {
      //std::cout << "Normalizing: " << name << std::endl;
      histo->Scale(1./norm);
    } else if (norm == 0.) {
      std::cout << "Empty histogram ! "<< name <<std::endl;
    }
  }
  for (std::map<std::string, TH3*>::iterator mapItr = m_histoMap3D.begin();
       mapItr != m_histoMap3D.end(); ++mapItr ) {
    const std::string& name = (*mapItr).first;
    //
    TH3* histo = (*mapItr).second;
    double norm(histo->Integral());
    if (norm!=1. && norm != 0.) {
      //std::cout << "Normalizing: " << name << std::endl;
      histo->Scale(1./norm);
    } else if (norm == 0.) {
      std::cout << "Empty histogram ! "<< name <<std::endl;
    }
  }


}

void HistoHelperRoot::normalizeHistos(const std::string histname)
{
  double norm = 0.;
  //
  std::map<std::string,TH1*>::const_iterator pos = m_histoMap1D.find(histname);
  if (pos != m_histoMap1D.end()) {
    TH1* histo = pos->second;
    norm = histo->Integral();
    if (norm!=1. && norm != 0.) {
      histo->Scale(1./norm);
    } else if (norm == 0.) {
      std::cout << "Empty histogram ! "<< (*pos).first <<std::endl;
    }
  } else {
    std::map<std::string,TH2*>::const_iterator pos = m_histoMap2D.find(histname);
    if (pos != m_histoMap2D.end()) {
      TH2* histo = pos->second;
      norm = histo->Integral();
      if (norm!=1. && norm != 0.) {
  histo->Scale(1./norm);
      } else if (norm == 0.) {
  std::cout << "Empty histogram ! "<< (*pos).first <<std::endl;
      }
    } else {
      std::map<std::string,TH3*>::const_iterator pos = m_histoMap3D.find(histname);
      if (pos != m_histoMap3D.end()) {
  TH3* histo = pos->second;
  norm = histo->Integral();
  if (norm!=1. && norm != 0.) {
    histo->Scale(1./norm);
  } else if (norm == 0.) {
    std::cout << "Empty histogram ! "<< (*pos).first <<std::endl;
  }
      } else {
  std::cout << "Histo "<< histname << " does not exist ? Nothing to normalise !" << std::endl;
      }
    }
  }
}

void HistoHelperRoot::smoothHistos(const std::string histname)
{
  std::map<std::string,TH1*>::const_iterator pos = m_histoMap1D.find(histname);
  if (pos != m_histoMap1D.end()) {
    (pos->second)->Smooth();
  } else {
    std::cout << "Histo "<< histname << " does not exist ? Nothing to normalise !" << std::endl;
  }
}

void HistoHelperRoot::print() {
  for (std::map<std::string, TH1*>::iterator mapItr  = m_histoMap1D.begin();
       mapItr != m_histoMap1D.end(); ++mapItr ) {
    const std::string& name = (*mapItr).first;
    std::cout <<"The name of the 1D Histo " << name << std::endl;
  }
  for (std::map<std::string, TH2*>::iterator mapItr  = m_histoMap2D.begin();
       mapItr != m_histoMap2D.end(); ++mapItr ) {
    const std::string& name = (*mapItr).first;
    std::cout <<"The name of the 2D Histo " << name << std::endl;
  }
  for (std::map<std::string, TH3*>::iterator mapItr  = m_histoMap3D.begin();
       mapItr != m_histoMap3D.end(); ++mapItr ) {
    const std::string& name = (*mapItr).first;
    std::cout <<"The name of the 3D Histo " << name << std::endl;
  }
}

void HistoHelperRoot::smoothASH2D(TH2* input2D, int m1, int m2, bool debug) {
  if(debug) {
    std::cout << "Smoothing a two dimensional histogram "<< input2D->GetName()
              << " " << m1 << " " << m2 << std::endl;
    std::cout << "First (1-3, 1-3) 3x3 bins before smoothing: " << std::endl;
    for(int i=1;i<4;i++) {
      for(int j=1;j<4;j++) {
        std::cout<<i<<" "<<j<<" : "<<input2D->GetBinContent(i,j)<< " / " ;
      }
    }
    std::cout << std::endl;
    int ioffset = input2D->GetNbinsX() / 2 ;
    int joffset = input2D->GetNbinsY() / 2 ;
    std::cout << "Middle (" << ioffset+1 << "-" << ioffset+4 << ", ("
              << joffset+1 << "-" << joffset+4 << ") 3x3 bins before smoothing: " << std::endl;
    for(int i=1;i<4;i++) {
      for(int j=1;j<4;j++) {
        std::cout<<i<<" "<<j<<" : "<<input2D->GetBinContent(i+ioffset,j+joffset)<< " / " ;
      }
    }
    std::cout << std::endl;
  }
  //
  const int lsup = 20;
  if (m1 > lsup || m2 > lsup) {
    std::cout <<"HistoHelperRoot::smoothASH2D: m1 or m2 too big !"<<std::endl;
    return;
  } else {
    int nx = input2D->GetNbinsX()+1;
    int ny = input2D->GetNbinsY()+1;
    float **h, **res;
    h   = new float*[nx-1];
    res = new float*[nx-1];
    for (int i = 0;i < nx-1;i++) {
      h[i]   = new float[ny-1];
      res[i] = new float[ny-1];
    }
    for (int iy = 1;iy<ny;iy++) {
      for (int ix = 1;ix<nx;ix++) {
        h[ix-1][iy-1] = (float) input2D->GetBinContent(ix,iy);
      }
    }
    //
    int i,j,k,l;
    float wk1[41],wk2[41],wgt[100][100];
    double wk[41][41],wks = 0.;
    float ai,am1 = float(m1), am2 = float(m2);
    const float am12 = am1*am1, am22 = am2*am2;
    const float inv_am1_am2 = 1. / (am1 * am2);
    const float inv_am12 = 1. / am12;
    const float inv_am22 = 1. / am22;
    // Initialisation
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        res[k][l] = 0.; wgt[k][l] = 0.;
      }
    }
    // Weights
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk1[i] = 15./16.*(1.-ai*inv_am12)*(1.-ai*inv_am12);
      wks = wks + wk1[i];
    }
    const double fac1 = am1 / wks;
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      wk1[i] =  wk1[i]*fac1;
    }
    wks = 0.;
    for (i = lsup+1-m2;i<lsup+m2;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk2[i] = 15./16.*(1.-ai*inv_am22)*(1.-ai*inv_am22);
      wks = wks + wk2[i];
    }
    const double fac2 = am2 / wks;
    for (i = lsup+1-m2;i<lsup+m2;i++) {
      wk2[i] =  wk2[i]*fac2;
    }
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      for (j = lsup+1-m2;j<lsup+m2;j++) {
        wk[i][j] = wk1[i]*wk2[j];
      }
    }
    //
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        for (i = std::max(0,k-m1+1);i<std::min(nx-1,k+m1);i++) {
          for (j = std::max(0,l-m2+1);j<std::min(ny-1,l+m2);j++) {
            res[i][j] = res[i][j] + wk[lsup+i-k][lsup+j-l]*h[k][l];
            wgt[i][j] = wgt[i][j] + wk[lsup+i-k][lsup+j-l];
          }
        }
      }
    }
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        res[k][l] = res[k][l]*inv_am1_am2;
        if (wgt[k][l] != 0.) {res[k][l] = res[k][l]/wgt[k][l];}
      }
    }
    // replace the histo content with the result of smoothing:
    input2D->Reset();
    for (int iy = 1;iy<ny;iy++) {
      for (int ix = 1;ix<nx;ix++) {
        input2D->SetBinContent(ix,iy,res[ix-1][iy-1]);
      }
    }
    for (i = 0; i < nx-1; i++){
      delete[] h[i];
      delete[] res[i];
    }
    delete[] h;
    delete[] res;
  }
  if(debug) {
    std::cout << "First (1-3, 1-3) 3x3 bins after smoothing: " << std::endl;
    for(int i=1;i<4;i++) {
      for(int j=1;j<4;j++) {
        std::cout<<i<<" "<<j<<" : "<<input2D->GetBinContent(i,j)<< " / " ;
      }
    }
    std::cout << std::endl;
    int ioffset = input2D->GetNbinsX() / 2 ;
    int joffset = input2D->GetNbinsY() / 2 ;
    std::cout << "Middle (" << ioffset+1 << "-" << ioffset+4 << ", ("
              << joffset+1 << "-" << joffset+4 << ") 3x3 bins after smoothing: " << std::endl;
    for(int i=1;i<4;i++) {
      for(int j=1;j<4;j++) {
        std::cout<<i<<" "<<j<<" : "<<input2D->GetBinContent(i+ioffset,j+joffset)<< " / " ;
      }
    }
    std::cout << std::endl;
  }
}

void HistoHelperRoot::smoothASH3D(TH3* input3D, int m1, int m2, int m3, bool debug) {
  if(debug) {
    std::cout << "Smoothing a three dimensional histogram "<<  input3D->GetName()
              << " " << m1 << " " << m2 << " " << m3 << std::endl;
    std::cout << "First 2x2x2 bins before smoothing: " << std::endl;
    for(int i=1;i<3;i++) {
      for(int j=1;j<3;j++) {
        for(int k=1;k<3;k++) {
          std::cout<<i<<" "<<j<<" "<<k<<" : "<<input3D->GetBinContent(i,j,k)<< " / " ;
	}
      }
    }
    std::cout << std::endl;
    int ioffset = input3D->GetNbinsX() / 2 ;
    int joffset = input3D->GetNbinsY() / 2 ;
    int koffset = input3D->GetNbinsZ() / 2 ;
    std::cout << "Middle (" << ioffset+1 << "-" << ioffset+3 << ", "
              << joffset+1 << "-" << joffset+3 << ", "
              << koffset+1 << "-" << koffset+3 << ") 2x2 bins before smoothing: " << std::endl;
    for(int i=1;i<3;i++) {
      for(int j=1;j<3;j++) {
        for(int k=1;k<3;k++) {
          std::cout<<i<<" "<<j<<" "<<k<<" : "<<input3D->GetBinContent(i+ioffset,j+joffset,k+koffset)<< " / " ;
	}
      }
    }
    std::cout << std::endl;
  }
  //
  const int lsup = 20;
  if (m1 > lsup || m2 > lsup || m3 > lsup) {
    std::cout <<"HistoHelperRoot::smoothASH3D: m1, m2 or m3 too big !"<<std::endl;
    return;
  } else {
    int nx = input3D->GetNbinsX()+1;
    int ny = input3D->GetNbinsY()+1;
    int nz = input3D->GetNbinsZ()+1;
    float ***h, ***res;
    h   = new float**[nx-1];
    res = new float**[nx-1];
    for (int i = 0;i < nx-1; i++) {
      h[i]   = new float*[ny-1];
      res[i] = new float*[ny-1];
      for (int j = 0; j < ny-1; j++) {
        h[i][j]   = new float[nz-1];
        res[i][j] = new float[nz-1];
      }
    }
    for (int iz = 1;iz<nz;iz++) {
      for (int iy = 1;iy<ny;iy++) {
        for (int ix = 1;ix<nx;ix++) {
          h[ix-1][iy-1][iz-1] = (float) input3D->GetBinContent(ix,iy,iz);
        }
      }
    }
    //
    int i,j,k,l,m,n;
    float wk1[41],wk2[41],wk3[41];
    //float wgt[100][100][100]; // Trop gros pour certaines machines !!??
    double wk[41][41][41],wks = 0.;
    float ai,am1 = float(m1), am2 = float(m2), am3 = float(m3);
    const float am12 = am1*am1, am22 = am2*am2, am32 = am3*am3;
    const float inv_am1_am2 = 1. / (am1*am2);
    const float inv_am12 = 1. / am12;
    const float inv_am22 = 1. / am22;
    const float inv_am32 = 1. / am32;
    
    // Initialisation
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        for (m = 0;m<nz-1;m++) {
          res[k][l][m] = 0.; 
	  //wgt[k][l][m] = 0.;
        }
      }
    }
    //
    // Bidouille car selon la machine un tableau wgt[100][100][100] peut poser probleme !
    //
    float ***wgt;
    int dimension = 100;  int x, y, z = 0;
    wgt = new float**[dimension];
    if (!wgt) {
      std::cout <<"HistoHelperRoot::smoothASH3D: problem to allocate memory, exiting"<<std::endl;
      exit(-1);
    }
    for(x = 0; x < dimension; x++) {
      wgt[x] = new float*[dimension];
      if (!wgt) {
        std::cout <<"HistoHelperRoot::smoothASH3D: problem to allocate memory, exiting"<<std::endl;
	exit(-1);
      }
      for(y = 0; y < dimension; y++) {
	wgt[x][y] = new float[dimension];
	if (!wgt) {
          std::cout <<"HistoHelperRoot::smoothASH3D: problem to allocate memory, exiting"<<std::endl;
	  exit(-1);
	}
	for(z = 0; z < dimension; z++)
	  wgt[x][y][z] = 0;
      }
    }
    // Weights
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk1[i] = 15./16.*(1.-ai*inv_am12)*(1.-ai*inv_am12);
      wks = wks + wk1[i];
    }
    const double fac1 = am1 / wks;
    for (i = lsup+1-m1;i<lsup+m1;i++) {
      wk1[i] =  wk1[i]*fac1;
    }
    wks = 0.;
    for (i = lsup+1-m2;i<lsup+m2;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk2[i] = 15./16.*(1.-ai*inv_am22)*(1.-ai*inv_am22);
      wks = wks + wk2[i];
    }
    const double fac2 = am2 / wks;
    for (i = lsup+1-m2;i<lsup+m2;i++) {
      wk2[i] =  wk2[i]*fac2;
    }
    wks = 0.;
    for (i = lsup+1-m3;i<lsup+m3;i++) {
      ai = float(i-lsup)*float(i-lsup);
      wk3[i] = 15./16.*(1.-ai*inv_am32)*(1.-ai*inv_am32);
      wks = wks + wk3[i];
    }
    const double fac3 = am3 / wks;
    for (i = lsup+1-m3;i<lsup+m3;i++) {
      wk3[i] =  wk3[i]*fac3;
    }

    for (i = lsup+1-m1;i<lsup+m1;i++) {
      for (j = lsup+1-m2;j<lsup+m2;j++) {
        for (k = lsup+1-m3;k<lsup+m3;k++) {
          wk[i][j][k] = wk1[i]*wk2[j]*wk3[k];
        }
      }
    }
    //
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        for (m = 0;m<nz-1;m++) {
          for (i = std::max(0,k-m1+1);i<std::min(nx-1,k+m1);i++) {
            for (j = std::max(0,l-m2+1);j<std::min(ny-1,l+m2);j++) {
              for (n = std::max(0,m-m3+1);n<std::min(nz-1,m+m3);n++) {
                res[i][j][n] = res[i][j][n] + wk[lsup+i-k][lsup+j-l][lsup+n-m]*h[k][l][m];
                wgt[i][j][n] = wgt[i][j][n] + wk[lsup+i-k][lsup+j-l][lsup+n-m];
              }
            }
          }
        }
      }
    }
    for (k = 0;k<nx-1;k++) {
      for (l = 0;l<ny-1;l++) {
        for (m = 0;m<nz-1;m++) {
          res[k][l][m] = res[k][l][m]*inv_am1_am2;
          if (wgt[k][l][m] != 0.) {res[k][l][m] = res[k][l][m]/wgt[k][l][m];}
        }
      }
    }
    // replace the content of histo with results of smoothing:
    input3D->Reset();
    for (int iz = 1;iz<nz;iz++) {
      for (int iy = 1;iy<ny;iy++) {
        for (int ix = 1;ix<nx;ix++) {
          input3D->SetBinContent(ix,iy,iz,res[ix-1][iy-1][iz-1]);
        }
      }
    }
    for (i = 0; i < nx-1; i++){
      for (j = 0; j < ny-1; j++) {
        delete[] h[i][j];
        delete[] res[i][j];
      }
      delete[] h[i];
      delete[] res[i];
    }
    delete[] h;
    delete[] res;
    //
    for (i = 0; i < dimension; i++){
      for (j = 0; j < dimension; j++)
	delete[] wgt[i][j];
      delete[] wgt[i];
    }
    delete[] wgt;
  }
  if(debug) {
    std::cout << "First 2x2x2 bins after smoothing: " << std::endl;
    for(int i=1;i<3;i++) {
      for(int j=1;j<3;j++) {
        for(int k=1;k<3;k++) {
          std::cout<<i<<" "<<j<<" "<<k<<" : "<<input3D->GetBinContent(i,j,k)<< " / " ;
	}
      }
    }
    std::cout << std::endl;
    int ioffset = input3D->GetNbinsX() / 2 ;
    int joffset = input3D->GetNbinsY() / 2 ;
    int koffset = input3D->GetNbinsZ() / 2 ;
    std::cout << "Middle (" << ioffset+1 << "-" << ioffset+3 << ", "
              << joffset+1 << "-" << joffset+3 << ", "
              << koffset+1 << "-" << koffset+3 << ") 2x2 bins after smoothing: " << std::endl;
    for(int i=1;i<3;i++) {
      for(int j=1;j<3;j++) {
        for(int k=1;k<3;k++) {
          std::cout<<i<<" "<<j<<" "<<k<<" : "<<input3D->GetBinContent(i+ioffset,j+joffset,k+koffset)<< " / " ;
	}
      }
    }
    std::cout << std::endl;
  }
}

// Interpolation... probably exists somewhere. Where ?
//
// histo 1D
//
double HistoHelperRoot::Interpol1d(double x, TH1* h)
{
  int bin((h->GetXaxis())->FindBin(x));
  double bincenter(h->GetBinCenter(bin));
  double nextbincenter(bincenter);
  double nextbincontent(0);
  if(x>bincenter && bin < h->GetNbinsX())
    {
      nextbincenter   = h->GetBinCenter(bin+1);
      nextbincontent  = h->GetBinContent(bin+1);
    }
  else if(x<bincenter && bin > 1)
    {
      nextbincenter   = h->GetBinCenter(bin-1);
      nextbincontent  = h->GetBinContent(bin-1);
    }
  double tmp(h->GetBinContent(bin));
  if(nextbincenter != bincenter) tmp = ( nextbincontent*(x-bincenter) + tmp*(nextbincenter-x) ) / (nextbincenter - bincenter);
  return tmp;
}

//
// histo 2D
//
double HistoHelperRoot::Interpol2d(double x, double y, TH2* h)
{
  int nx  = h->GetNbinsX(), ny = h->GetNbinsY();
  int ibx = h->GetXaxis()->FindBin(x), iby = h->GetYaxis()->FindBin(y);
  int ibx2,iby2;
  double z00,z11,z01,z10,xc,yc,xc2,yc2,u,t,r;
  if (ibx > nx) ibx = nx;
  if (iby > ny) iby = ny;
  xc  = h->GetXaxis()->GetBinCenter(ibx);
  yc  = h->GetYaxis()->GetBinCenter(iby);
  z11 = h->GetBinContent(ibx,iby);
  if ( (ibx > 1  || (ibx == 1  && x > xc) ) &&
       (ibx < nx || (ibx == nx && x < xc) ) ) {
    if (x > xc) {ibx2 = ibx + 1;} else {ibx2 = ibx - 1;}
    xc2 = h->GetXaxis()->GetBinCenter(ibx2);
    if ( (iby > 1  || (iby == 1  && y > yc) ) &&
         (iby < ny || (iby == ny && y < yc) ) ) {
      if (y > yc) {iby2 = iby + 1;} else {iby2 = iby - 1;}
      yc2 = h->GetYaxis()->GetBinCenter(iby2);
      z00 = h->GetBinContent(ibx2,iby2);
      z10 = h->GetBinContent(ibx,iby2);
      z01 = h->GetBinContent(ibx2,iby);
      t   = (x - xc2)/(xc-xc2);
      u   = (y - yc2)/(yc-yc2);
      r   = z11*t*u+z00*(1.-t)*(1.-u)+z01*(1.-t)*u+z10*t*(1.-u);
    } else {
      z01 = h->GetBinContent(ibx2,iby);
      t   = (x - xc2)/(xc-xc2);
      r   = z11*t + z01*(1.-t);
    }
  } else if ((iby > 1  || (iby == 1  && y > yc) ) &&
       (iby < ny || (iby == ny && y < yc) ) ) {
    if (y > yc) {iby2 = iby + 1;} else {iby2 = iby - 1;}
    z10 = h->GetBinContent(ibx,iby2);
    yc2 = h->GetYaxis()->GetBinCenter(iby2);
    u   = (y - yc2)/(yc-yc2);
    r   = z11*u + z10*(1.-u);
  } else {
    r = z11;
  }
  return r;
}

//
// histo 3D
//
double HistoHelperRoot::Interpol3d(double x, double y, double z, TH3* h)
{
  int nx  = h->GetNbinsX(), ny = h->GetNbinsY(), nz = h->GetNbinsZ();
  int ibx = h->GetXaxis()->FindBin(x), iby = h->GetYaxis()->FindBin(y), ibz = h->GetZaxis()->FindBin(z);
  int ibx2,iby2,ibz2;
  double z000,z010,z110,z100,z001,z011,z111,z101,xc,yc,zc,xc2,yc2,zc2,u,t,v,r;
  if (ibx > nx) ibx = nx;
  if (iby > ny) iby = ny;
  if (ibz > nz) ibz = nz;
  xc  = h->GetXaxis()->GetBinCenter(ibx);
  yc  = h->GetYaxis()->GetBinCenter(iby);
  zc  = h->GetZaxis()->GetBinCenter(ibz);
  //
  z111 = h->GetBinContent(ibx,iby,ibz);
  if ( (ibx > 1  || (ibx == 1  && x > xc) ) &&
       (ibx < nx || (ibx == nx && x < xc) ) ) {
    if (x > xc) {ibx2 = ibx + 1;} else {ibx2 = ibx - 1;}
    xc2 = h->GetXaxis()->GetBinCenter(ibx2);
    if ( (iby > 1  || (iby == 1  && y > yc) ) &&
         (iby < ny || (iby == ny && y < yc) ) ) {
      if (y > yc) {iby2 = iby + 1;} else {iby2 = iby - 1;}
      yc2 = h->GetYaxis()->GetBinCenter(iby2);
      if ( (ibz > 1  || (ibz == 1  && z > zc) ) &&
     (ibz < nz || (ibz == nz && z < zc) ) ) {
  if (z > zc) {ibz2 = ibz + 1;} else {ibz2 = ibz - 1;}
  zc2 = h->GetZaxis()->GetBinCenter(ibz2);
  //
  z000 = h->GetBinContent(ibx2,iby2,ibz2);
  z100 = h->GetBinContent(ibx,iby2,ibz2);
  z010 = h->GetBinContent(ibx2,iby,ibz2);
  z110 = h->GetBinContent(ibx,iby,ibz2);
  z001 = h->GetBinContent(ibx2,iby2,ibz);
  z101 = h->GetBinContent(ibx,iby2,ibz);
  z011 = h->GetBinContent(ibx2,iby,ibz);
  t   = (x - xc2)/(xc-xc2);
  u   = (y - yc2)/(yc-yc2);
  v   = (z - zc2)/(zc-zc2);
  r   = z111*t*u*v           + z001*(1.-t)*(1.-u)*v
      + z011*(1.-t)*u*v      + z101*t*(1.-u)*v
      + z110*t*u*(1.-v)      + z000*(1.-t)*(1.-u)*(1.-v)
      + z010*(1.-t)*u*(1.-v) + z100*t*(1.-u)*(1.-v);
      } else {
  z011 = h->GetBinContent(ibx2,iby,ibz);
  z001 = h->GetBinContent(ibx2,iby2,ibz);
  z101 = h->GetBinContent(ibx,iby2,ibz);
  t   = (x - xc2)/(xc-xc2);
  u   = (y - yc2)/(yc-yc2);
  r   = z111*t*u + z011*(1.-t)*u + z101*t*(1.-u) + z001*(1.-t)*(1.-u) ;
      }
    } else if ((ibz > 1  || (ibz == 1  && z > zc) ) &&
         (ibz < nz || (ibz == nz && z < zc) ) ) {
      if (z > zc) {ibz2 = ibz + 1;} else {ibz2 = ibz - 1;}
      z110 = h->GetBinContent(ibx,iby,ibz2);
      z010 = h->GetBinContent(ibx2,iby,ibz2);
      z011 = h->GetBinContent(ibx2,iby,ibz);
      zc2 = h->GetYaxis()->GetBinCenter(ibz2);
      t   = (x - xc2)/(xc-xc2);
      v   = (z - zc2)/(zc-zc2);
      r   = z111*t*v + z011*(1.-t)*v + z110*t*(1.-v) +z010*(1.-t)*(1.-v);
    } else {
      z011 = h->GetBinContent(ibx2,iby,ibz);
      t   = (x - xc2)/(xc-xc2);
      r   = z111*t + z011*(1.-t);
    }
  } else {
    if ( (iby > 1  || (iby == 1  && y > yc) ) &&
         (iby < ny || (iby == ny && y < yc) ) ) {
      if (y > yc) {iby2 = iby + 1;} else {iby2 = iby - 1;}
      yc2 = h->GetYaxis()->GetBinCenter(iby2);
      if ( (ibz > 1  || (ibz == 1  && z > zc) ) &&
     (ibz < nz || (ibz == nz && z < zc) ) ) {
  if (z > zc) {ibz2 = ibz + 1;} else {ibz2 = ibz - 1;}
  zc2 = h->GetZaxis()->GetBinCenter(ibz2);
  //
  z100 = h->GetBinContent(ibx,iby2,ibz2);
  z110 = h->GetBinContent(ibx,iby,ibz2);
  z101 = h->GetBinContent(ibx,iby2,ibz);
  u   = (y - yc2)/(yc-yc2);
  v   = (z - zc2)/(zc-zc2);
  r   = z111*u*v      + z101*(1.-u)*v
      + z110*u*(1.-v) + z100*(1.-u)*(1.-v);
      } else {
  z101 = h->GetBinContent(ibx,iby2,ibz);
  u   = (y - yc2)/(yc-yc2);
  r   = z111*u + z101*(1.-u);
      }
    } else if ((ibz > 1  || (ibz == 1  && z > zc) ) &&
         (ibz < nz || (ibz == nz && z < zc) ) ) {
      if (z > zc) {ibz2 = ibz + 1;} else {ibz2 = ibz - 1;}
      z110 = h->GetBinContent(ibx,iby,ibz2);
      zc2 = h->GetYaxis()->GetBinCenter(ibz2);
      v   = (z - zc2)/(zc-zc2);
      r   = z111*v + z110*(1.-v);
    } else {
      r = z111;
    }
  }
  return r;
}

}
