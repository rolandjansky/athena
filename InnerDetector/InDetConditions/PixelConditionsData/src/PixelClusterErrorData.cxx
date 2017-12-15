/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelClusterErrorData.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <fstream>
#include <string>

const int nmax(200);// protection for loop bound

// use namespace PixelOfflineCalib;

namespace PixelCalib{

  //PixelClusterErrorData::PixelClusterErrorData(){
  // Initialize();
  //}

// Load defaults - real values to be loaded from database
// the number of bins is still hardwired however

void PixelClusterErrorData::Initialize(){

  std::cout << "PixelClusterErrorData Initialize " << std::endl;

  m_version = 1;
  // number of bins of parametrizations
  m_nPhi=3; m_nZ=4;
  // Bins of eta parametrization for barrel
  //  m_etaref = 0;
  m_etaref.reserve(6);
  m_etaref.push_back(0.00); 
  m_etaref.push_back(0.55); 
  m_etaref.push_back(0.87); 
  m_etaref.push_back(1.32); 
  m_etaref.push_back(2.00);
  m_etaref.push_back(2.95);

  // m_barrelphierror = 0;
  // m_barreletaerror = 0;
  m_barrelphierror.reserve(60);
  m_barreletaerror.reserve(60);
  for(int i = 0; i<60; i++){
     m_barrelphierror.push_back(14.43*CLHEP::micrometer);
     m_barreletaerror.push_back(115.5*CLHEP::micrometer);
  }

  // m_endcapphierror = 0;
  // m_endcapetaerror = 0;
  m_endcapphierror.reserve(12);
  m_endcapetaerror.reserve(12);
  for(int i=0; i<12; i++){
    m_endcapphierror.push_back(14.43*CLHEP::micrometer);
    m_endcapetaerror.push_back(115.5*CLHEP::micrometer);
  }

  // IBL
  // m_iblphierror = 0;
  // m_ibletaerror = 0;
  m_iblphierror.reserve(60);
  m_ibletaerror.reserve(60);
  for(int i = 0; i<60; i++){
    m_iblphierror.push_back(14.43*CLHEP::micrometer);
    m_ibletaerror.push_back(72.2*CLHEP::micrometer);
  }

  std::cout << "done" << std::endl;
  
  return;
}

void PixelClusterErrorData::setVersion(int version){ m_version = version; }

int PixelClusterErrorData::getVersion() const{ return m_version; }

float PixelClusterErrorData::getPixelEndcapPhiError(int ibin) const{
  if(ibin < 0) return -1;
  if(static_cast<unsigned int>(ibin) > m_endcapphierror.size()) return -1;
  return m_endcapphierror[ibin];
}

float PixelClusterErrorData::getPixelEndcapRError(int ibin) const{
  if(ibin < 0) return -1;
  if(static_cast<unsigned int>(ibin) > m_endcapetaerror.size()) return -1;
  return m_endcapetaerror[ibin];
}

float PixelClusterErrorData::getPixelBarrelPhiError(int ibin) const{
  if(ibin < 0) return -1;
  if(static_cast<unsigned int>(ibin) > m_barrelphierror.size()) return -1;
  return m_barrelphierror[ibin];
}

float PixelClusterErrorData::getPixelBarrelEtaError(int ibin) const{
 if(ibin < 0) return -1;
 if(static_cast<unsigned int>(ibin) > m_barreletaerror.size()) return -1;
 return m_barreletaerror[ibin];
}


float PixelClusterErrorData::getPixelIBLPhiError(int ibin) const{
  if(ibin < 0||m_version>-2) return -1;
  if(static_cast<unsigned int>(ibin) > m_iblphierror.size()) return -1;
  return m_iblphierror[ibin];
}

float PixelClusterErrorData::getPixelIBLEtaError(int ibin) const{
  if(ibin < 0||m_version>-2) return -1;
  if(static_cast<unsigned int>(ibin) > m_ibletaerror.size()) return -1;
  return m_ibletaerror[ibin];
}

// SET METHODS

int PixelClusterErrorData::setPixelEndcapPhiError(int ibin, float error){
  if(ibin<0) return 0;
  if(static_cast<unsigned int>(ibin)>m_endcapphierror.size()) return 0;
  m_endcapphierror[ibin] = error;
  return 1;
}

int PixelClusterErrorData::setPixelEndcapRError(int ibin, float error){
  if(ibin<0) return 0;
  if(static_cast<unsigned int>(ibin)>m_endcapetaerror.size()) return 0;
  m_endcapetaerror[ibin] = error;
  return 1;
}

int PixelClusterErrorData::setPixelBarrelPhiError(int ibin, float error){
 if(ibin<0) return 0;
 if(static_cast<unsigned int>(ibin)>m_barrelphierror.size()) return 0;
 m_barrelphierror[ibin] = error;
 return 1;
}

int PixelClusterErrorData::setPixelBarrelEtaError(int ibin, float error){
 if(ibin<0) return 0;
 if(static_cast<unsigned int>(ibin)>m_barreletaerror.size()) return 0;
 m_barreletaerror[ibin] = error;
 return 1;
}

int PixelClusterErrorData::setPixelIBLPhiError(int ibin, float error){
    if(ibin<0||m_version>-2) return 0;
    if(static_cast<unsigned int>(ibin)>m_iblphierror.size()) return 0;
    m_iblphierror[ibin] = error;
    return 1;
}

int PixelClusterErrorData::setPixelIBLEtaError(int ibin, float error){
    if(ibin<0||m_version>-2) return 0;
    if(static_cast<unsigned int>(ibin)>m_ibletaerror.size()) return 0;
    m_ibletaerror[ibin] = error;
    return 1;
}


// get bin of pixel barrel error parametrization

int PixelClusterErrorData::getBarrelBin(double eta, int etaClusterSize, 
    int phiClusterSize) const{

  int ieta=0;
  int neta = m_etaref.size();
  for(int i=0; i<neta; i++){  if(eta>m_etaref[i]) ieta=i;  }
  // phi cluster size bin
  int iphi = phiClusterSize-1;
  if(iphi > m_nPhi-1) iphi = m_nPhi-1;
  // z cluster size bin
  int iz = etaClusterSize-1;
  if(iz > m_nZ-1) iz = m_nZ-1;

  return m_nZ*m_nPhi*ieta+m_nZ*iphi+iz;
}

int PixelClusterErrorData::getEndcapBin(int etaClusterSize, int phiClusterSize) const{

  //  std::cout << "getting bin..." << std::endl;
  int iphi = phiClusterSize-1;
  if(iphi > m_nPhi-1) iphi = m_nPhi-1;
  int iz = etaClusterSize-1;
  if(iz > m_nZ-1) iz = m_nZ-1;
  // std::cout << "returning" << m_nZ*iphi+iz << std::endl;
  return m_nZ*iphi+iz; 
}

// save all costants to file
void PixelClusterErrorData::Print(std::string file) const {

  std::ofstream* outfile = new std::ofstream(file.c_str()); 

   if(m_version < 0) *outfile << m_version << std::endl;
   *outfile << m_barrelphierror.size() << std::endl;
   *outfile << m_endcapphierror.size() << std::endl;
  for(unsigned int ib=0; ib<m_barrelphierror.size(); ib++){
    *outfile << m_barrelphierror[ib] << " " << m_barreletaerror[ib] << std::endl;
  }
  for(unsigned int ie=0; ie<m_endcapphierror.size(); ie++){
    *outfile << m_endcapphierror[ie] << " " << m_endcapetaerror[ie] << std::endl;
  }
  // IBL ?
  if(m_version<-1){
    for(unsigned int ib=0; ib<m_iblphierror.size(); ib++){
      *outfile << m_iblphierror[ib] << " " << m_ibletaerror[ib] << std::endl;
    }
  }

  outfile->close(); 
  delete outfile; 
}

// Load costants from file
void PixelClusterErrorData::Load(std::string file){
  std::ifstream infile(file.c_str()); 

  // number of bins for barrel and endcap
  int nb = 0;
  int ne = 0;

  infile >> m_version;
  if(m_version >= 0){  // very first format, without version number in file
    nb = m_version;
    m_version = 0;
    infile >> ne;
  }
  else{
    infile >> nb >> ne;
  }

  m_barrelphierror.clear();
  m_barreletaerror.clear();
  m_endcapphierror.clear();
  m_endcapetaerror.clear();
  m_iblphierror.clear();
  m_ibletaerror.clear();

  float value;
  if(nb<0) nb=0;
  else if(nb>nmax) nb=nmax;
  if(ne<0) ne=0;
  else if(ne>nmax) ne=nmax;
  for(int ib=0; ib<nb && !infile.eof(); ib++){
    infile >> value;
    m_barrelphierror.push_back(value);
    infile >> value;
    m_barreletaerror.push_back(value);
  }
  for(int ie=0; ie<ne && !infile.eof(); ie++){
    infile >> value;
    m_endcapphierror.push_back(value);
    infile >> value;
    m_endcapetaerror.push_back(value);
  }
  // there is more data for IBL some version number ?
  if(m_version<-1){ 
    for(int ib=0; ib<nb; ib++){
      infile >> value;
      m_iblphierror.push_back(value);
      infile >> value;
      m_ibletaerror.push_back(value);
    }
  }

  infile.close();  
}

}
