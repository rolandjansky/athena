/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelClusterOnTrackErrorData.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>
#include <fstream>
#include <string>

const int nmax(200); // protection for loop bound
// use namespace PixelOfflineCalib;


namespace PixelCalib{

  PixelClusterOnTrackErrorData::PixelClusterOnTrackErrorData(){
     Initialize();
  }

   PixelClusterOnTrackErrorData::~PixelClusterOnTrackErrorData(){}

// Load defaults, will be used if reading from DB does not work
void PixelClusterOnTrackErrorData::Initialize(){

  // Bins of cluster size parametrizations
  m_version = 0;
  m_csx.reserve(3);
  m_csx.push_back(0.5);
  m_csx.push_back(1.5);
  m_csx.push_back(2.5);
  m_csy.reserve(4);
  m_csy.push_back(0.5);
  m_csy.push_back(1.5);
  m_csy.push_back(2.5);
  m_csy.push_back(3.5);

  // Bins of eta parametrization for barrel
  m_etaref.reserve(5);
  m_etaref.push_back(0.00); 
  m_etaref.push_back(0.55); 
  m_etaref.push_back(0.87); 
  m_etaref.push_back(1.32); 
  m_etaref.push_back(2.00);

  // alfa (Rphi incidence angle) bins: [-6., -4., ... ,14]
  int nphibins = 10;
  for(int i=0; i<nphibins+1; i++){
       m_phibins.push_back(-6+2*i);
  }

  // load defaults
  int nbiny = m_csx.size()*m_csy.size()*m_etaref.size();
  m_barreletaerror.reserve(nbiny);
  for(int i = 0; i<nbiny; i++){
     m_barreletaerror.push_back(115.4*CLHEP::micrometer);
  }

  int nbinx = m_csx.size()*m_phibins.size();
  m_barrelphierror.reserve(nbinx);
  for(int i = 0; i<nbinx; i++){
    m_barrelphierror.push_back(14.4*CLHEP::micrometer);
  }
  // IBL 
  m_csxbinsibl = 0;
  m_csybinsibl = 0;
  m_etabinsibl = 0;
  m_phibinsibl = 0;
  /* init not needed  
   // Init from DB for IBL: 
   m_version = -2;
   m_csxbinsibl = 3;
   m_csybinsibl = 3;
   m_etabinsibl = 6;
   m_ibletaref.reserve(m_etabinsibl+1);
   m_ibletaref.push_back(0.00);
   m_ibletaref.push_back(0.55);
   m_ibletaref.push_back(1.275);
   m_ibletaref.push_back(1.725);
   m_ibletaref.push_back(2.025);
   m_ibletaref.push_back(2.25);
   m_ibletaref.push_back(2.70);

   // alfa (Rphi incidence angle) bins: [-15.47, 15.47]
   float phimin = -15.47;
   float phimax = 15.47;
   m_phibinsibl = 9
   m_iblphibins.reserve(m_phibinsibl+1);
   for(int i=0; i<m_phibinsibl+1; i++){
     m_iblphibins.push_back(phimin +i*(phimax-phimin)/m_phibinsibl);
   }
   // load defaults for IBL
   int nbinyibl = m_csybinsibl*m_ibletaref.size();
   m_ibletaerror.reserve(nbinyibl);
   for(int i = 0; i<nbinyibl; i++){
     m_ibletaerror.push_back(72.2*CLHEP::micrometer);
   }

   int nbinxibl = m_csxbinsibl*m_iblphibins.size();
   m_iblphierror.reserve(nbinxibl);
   for(int i = 0; i<nbinxibl; i++){
     m_iblphierror.push_back(14.4*CLHEP::micrometer);
   }
  */
  return;
}

int PixelClusterOnTrackErrorData::getNumberOfPhiBarrelBins() const{
    return m_barrelphierror.size();
}

int PixelClusterOnTrackErrorData::getNumberOfEtaBarrelBins() const{
    return m_barreletaerror.size();
}

int PixelClusterOnTrackErrorData::getNumberOfPhiIBLBins() const{
  if(m_version>-2) return -1;
  return m_iblphierror.size();
}

int PixelClusterOnTrackErrorData::getNumberOfEtaIBLBins() const{
   if(m_version>-2) return -1;
   return m_ibletaerror.size();
}

double PixelClusterOnTrackErrorData::getPixelBarrelPhiError(double ang, 
                                                    int deltax){ 
   double errphi=50*CLHEP::micrometer/pow(12,0.5);
   // error on phi coordinate
    if(deltax == 1){
    // 1-row hit not expected - return conservative error estimate.
      if(ang>12){ errphi = 50*CLHEP::micrometer/sqrt(12.); }
      else{
         // probability to get a 1-row cluster. Error basically proportional 
	//  to this. 
         double frac = 0.8-0.6*ang/12;
         errphi = frac*50*CLHEP::micrometer/sqrt(12.);
	 // Now account for some smearing w.r.t. ideal case
	 // tuned so that pulls turns out ok
	 // also some overall rescaling
         double delta = 3*CLHEP::micrometer;
         errphi = 1.1*sqrt(errphi*errphi+delta*delta);
      }
    }
    else if(deltax == 2){
    // Charge interpolation: good precision, weakly dependent on angle.
    // Have not studied resolution yet for large angles (CTB, or very low 
    // pt tracks) - I put a conservative estimate here.
      if(ang > 14){ errphi = 25*CLHEP::micrometer/sqrt(12); }
      else{ errphi = 3*CLHEP::micrometer+2.5*CLHEP::micrometer*ang/14; }
    }
    else{
      // Have not studied resolution yet for large angles (CTB, or very low 
    // pt tracks) - I put a conservative estimate here.
      if(ang > 14){ errphi = 25*CLHEP::micrometer/sqrt(12); }
      // at low angles do not expect large clusters - maybe a delta ray?
      else{ errphi = deltax*50*CLHEP::micrometer/sqrt(12); }
    }
    return errphi; 
}

float PixelClusterOnTrackErrorData::getPixelBarrelEtaError(int ibin){
  if(ibin < 0) return -1;
  if(static_cast<unsigned int>(ibin) >= m_barreletaerror.size()) return -2;
  return m_barreletaerror[ibin];
}

float PixelClusterOnTrackErrorData::getPixelBarrelPhiError(int ibin){
  if(ibin < 0){ return -1; }
  if(static_cast<unsigned int>(ibin) >= m_barrelphierror.size()){ return -2;}
  return m_barrelphierror[ibin];
}

float PixelClusterOnTrackErrorData::getPixelIBLEtaError(int ibin){
    if(ibin < 0||m_version>-2) return -1;
    if(static_cast<unsigned int>(ibin) >= m_ibletaerror.size()) return -2;
    return m_ibletaerror[ibin];
}

float PixelClusterOnTrackErrorData::getPixelIBLPhiError(int ibin){
    if(ibin < 0||m_version>-2){ return -1; }
    if(static_cast<unsigned int>(ibin) >= m_iblphierror.size()){ return -2;}
    return m_iblphierror[ibin];
}

int PixelClusterOnTrackErrorData::getBarrelBinPhi(double angle, 
                                   int phiClusterSize){

  int iang =0;
  int nang = m_phibins.size();
  if(nang == 0) {
    Initialize();
    nang = m_phibins.size();
  }
  for(int i=0; i<nang; i++){  
    if(angle > m_phibins[i]) iang=i;  
  } 
  int iphi=0;
  int nphi = m_csx.size();
  for(int i=0; i<nphi; i++){  
    if(phiClusterSize>m_csx[i]) iphi=i;  
  } 
   
  return nphi*iang+iphi;
}

int PixelClusterOnTrackErrorData::getBarrelBinEta(double eta, int etaClusterSize, 
    int phiClusterSize){

  int ieta=0;
  int neta = m_etaref.size();
  if(neta == 0) {
    Initialize();
    neta = m_etaref.size();
  }
  for(int i=0; i<neta; i++){  
    if(eta>m_etaref[i]) ieta=i;  
  } 
  int iphi=0;
  int nphi = m_csx.size();
  for(int i=0; i<nphi; i++){  
    if(phiClusterSize>m_csx[i]) iphi=i;  
  } 
   
  int iz=0;
  int nz = m_csy.size();
  for(int i=0; i<nz; i++){  
    if(etaClusterSize>m_csy[i]) iz=i;  
  } 

  return nz*nphi*ieta+nz*iphi+iz;
}

int PixelClusterOnTrackErrorData::getIBLBinPhi(double angle,
						    int phiClusterSize){
    if(m_version>-2) return -1;
    int iang =0;
    int nang = m_iblphibins.size();
    for(int i=0; i<nang; i++){
      if(angle > m_iblphibins[i]) iang=i;
    }
    int iphi=0;
    for(int i=0; i<m_csxbinsibl; i++){
      if(phiClusterSize>m_csx[i]) iphi=i;
    }
    return m_csxbinsibl*iang+iphi;
}

int PixelClusterOnTrackErrorData::getIBLBinEta(double eta, int etaClusterSize){

    if(m_version>-2) return -1;
    int ieta=0;
    int neta = m_ibletaref.size();
    for(int i=0; i<neta; i++){
      if(eta>m_ibletaref[i]) ieta=i;
    }
    int iz=0;
    for(int i=0; i<m_csybinsibl; i++){
      if(etaClusterSize>m_csy[i]) iz=i;
    }
    return m_csybinsibl*ieta+iz;
}

void PixelClusterOnTrackErrorData::setParameters(const int n1, // number of cluster size bins (x-direction)
                                                 const int n2,   // number of cluster size bins (y-direction)  
						 const int n3,   // number of eta bins
						 const int n4,   // number of incidence angle bins
						 int offset, // start from c[offset]
					         const float* c){  // vector with bin values

  m_csx.clear();
  m_csy.clear();
  m_etaref.clear();
  m_phibins.clear();
  m_csx.reserve(n1);
  m_csy.reserve(n2);
  m_etaref.reserve(n3);
  m_phibins.reserve(n4);
  for(int i=0; i<n1; i++){
    m_csx.push_back(c[i+offset]);
  }   
  offset += n1;
  for(int i=0; i<n2; i++){
    m_csy.push_back(c[i+offset]);
  }   
  offset += n2;
  if(m_etabinsibl>0){ // IBL eta
    m_ibletaref.reserve(m_etabinsibl+1);
    for(int i = 0; i<m_etabinsibl+1; i++){
      m_ibletaref.push_back(c[i+offset]);
    }
    offset +=m_etabinsibl+1;
  }
  if(m_phibinsibl>0){ // IBL phi 
    m_iblphibins.reserve(m_phibinsibl+1);
    for(int i = 0; i<m_phibinsibl+1; i++){
      m_iblphibins.push_back(c[i+offset]);
    }
    offset +=m_phibinsibl+1;
  }    
  for(int i=0; i<n3; i++){
    m_etaref.push_back(c[i+offset]);
  }   
  offset += n3;
  for(int i=0; i<n4; i++){
    m_phibins.push_back(c[i+offset]);
  }   
  offset +=n4;
  int nxbinsibl = m_csxbinsibl*m_phibinsibl;
  if(nxbinsibl>0){ // IBL
    m_iblphierror.clear();
    m_iblphierror.reserve(nxbinsibl);
    for(int i = 0; i<nxbinsibl; i++){
      m_iblphierror.push_back(50.0/sqrt(12)*CLHEP::micrometer);
    }
  }
  int nybinsibl = m_csybinsibl*m_etabinsibl;
  if(nybinsibl>0){ // IBL
    m_ibletaerror.clear();
    m_ibletaerror.reserve(nybinsibl);
    for(int ib1=0; ib1<nybinsibl; ib1++){ 
      m_ibletaerror.push_back(250./sqrt(12)*CLHEP::micrometer);
    }
  }
  int nbiny = m_csx.size()*m_csy.size()*m_etaref.size();
  m_barreletaerror.clear();
  m_barreletaerror.reserve(nbiny);
  for(int i = 0; i<nbiny; i++){
     m_barreletaerror.push_back(400./sqrt(12)*CLHEP::micrometer);
  }
  int nbinx = m_csx.size()*m_phibins.size();
  m_barrelphierror.clear();
  m_barrelphierror.reserve(nbinx);
  for(int i = 0; i<nbinx; i++){
     m_barrelphierror.push_back(50./sqrt(12)*CLHEP::micrometer);
  }
  return;
}

 void PixelClusterOnTrackErrorData::setVersion(int version){ m_version = version; }

 int PixelClusterOnTrackErrorData::getVersion() const{ return m_version; }

//////////////////////////////////////////////////////////////////////////////////////////
// save all costants to file
//
void PixelClusterOnTrackErrorData::Print(std::string file) const {

  std::ofstream* outfile = new std::ofstream(file.c_str());

  if(m_version<0)*outfile << m_version << std::endl;

  int ncsx = m_csx.size(); // number of cluster size (x-direction) bins
  int ncsy = m_csy.size(); // number of cluster size (x-direction) bins
  int neta = m_etaref.size(); // number of eta values bins
  int nalpha = m_phibins.size(); // number of incidence angle bins
  if(m_version<-1){
    *outfile <<ncsx<<" "<<ncsy<<" "<<neta<<" "<<nalpha<<" "<<m_csxbinsibl<<
	  " "<<m_csybinsibl<<" "<<m_etabinsibl<<" "<<m_phibinsibl<<std::endl;
  }
  else{
    *outfile << ncsx << " " << ncsy << " " << neta << " " << nalpha << std::endl;
  }

  for(int i=0; i<ncsx; i++){
    *outfile << m_csx[i] << " ";
  }
  *outfile << std::endl;
  for(int i=0; i<ncsy; i++){
    *outfile << m_csy[i] << " ";
  }
  *outfile << std::endl;
  if(m_etabinsibl>0 && m_phibinsibl >0 ){ // IBL
    for(unsigned int i=0; i<m_ibletaref.size(); i++){
      *outfile << m_ibletaref[i] << " ";
    }
    *outfile << std::endl;
    for(unsigned int i=0; i<m_iblphibins.size(); i++){
      *outfile << m_iblphibins[i] << " ";
    }
    *outfile << std::endl;
  }
  for(int i=0; i<neta; i++){
    *outfile << m_etaref[i] << " ";
  }
  *outfile << std::endl;
  for(int i=0; i<nalpha; i++){
    *outfile << m_phibins[i] << " ";
  }
  *outfile << std::endl;
  int nxbinsibl = m_csxbinsibl*m_phibinsibl;
  if(nxbinsibl>0){ // IBL
    for(int ib1=0; ib1<nxbinsibl; ib1++){
      *outfile <<m_iblphierror[ib1]<<std::endl;
    }
  }
  int nb1=ncsx*nalpha; // number of barrel phi bins
  int nb2=ncsx*ncsy*neta; // number of barrel eta bins
  for(int ib1=0; ib1<nb1; ib1++){
    *outfile << m_barrelphierror[ib1] << std::endl;
  }
  int nybinsibl = m_csybinsibl*m_etabinsibl;
  if(nybinsibl>0){ // IBL
    for(int ib1=0; ib1<nybinsibl; ib1++){
      *outfile <<m_ibletaerror[ib1]<<std::endl;
    }
  }
  for(int ib2=0; ib2<nb2; ib2++){
    *outfile << m_barreletaerror[ib2] << std::endl;
  }
  outfile->close(); 
  delete outfile; 
}


//////////////////////////////////////////////////////////////////////////////////////////


// Load costants from file
void PixelClusterOnTrackErrorData::Load(std::string file){
  std::ifstream infile(file.c_str()); 

  // number of bins of parametrization
  int ncsx; // cluster size in x
  int ncsy; // cluster size in y
  int neta = 0; // pseudorapidity
  int nalpha; // incidence angle
  m_csxbinsibl = 0;
  m_csybinsibl = 0;
  m_etabinsibl = 0;
  m_phibinsibl = 0;
  int nxbinsibl(0);
  int nybinsibl(0);

  infile >> m_version;
  if(m_version >= 0){  // very first format, without version number in file
    ncsx = m_version;
    m_version = 0;
    infile >> ncsy >> neta >> nalpha;
  }
  else{
    if(m_version<-1){ // IBL version 
      infile>> ncsx >> ncsy >> neta >> nalpha>>m_csxbinsibl>>m_csybinsibl>>m_etabinsibl>>m_phibinsibl;
      nxbinsibl = m_csxbinsibl*m_phibinsibl;
      nybinsibl = m_csybinsibl*m_etabinsibl;
    }
    else{
    infile >> ncsx >> ncsy >> neta >> nalpha;
    }
  }

  // read bins of parametrization

  float value;
  if(ncsx<0)ncsx = 0;
  if(ncsx>nmax) ncsx=nmax;
  m_csx.clear();
  m_csx.reserve(ncsx);
  for(int i=0; i<ncsx && !infile.eof(); i++){
    infile >> value;
    m_csx.push_back(value);
  }
  if(ncsy<0)ncsy=0;
  if(ncsy>nmax) ncsy=nmax;
  m_csy.clear();
  m_csy.reserve(ncsy);
  for(int i=0; i<ncsy && !infile.eof(); i++){
    infile >> value;
    m_csy.push_back(value);
  }
  if(m_etabinsibl>0 && m_phibinsibl >0 ){ // IBL
    m_ibletaref.clear();
    if(m_etabinsibl>nmax) m_etabinsibl=nmax;
    m_ibletaref.reserve(m_etabinsibl+1);
    for(int i=0; i<m_etabinsibl+1; i++){
      infile >> value;
      m_ibletaref.push_back(value);
    }
    m_iblphibins.clear();
    if(m_phibinsibl>nmax) m_phibinsibl=nmax;
    m_iblphibins.reserve(m_phibinsibl+1);
    for(int i=0; i<m_phibinsibl+1; i++){
      infile >> value;
      m_iblphibins.push_back(value);
    }	 
  }
  if(neta<0)neta=0;
  if(neta>nmax) neta=nmax;
  m_etaref.clear();
  m_etaref.reserve(neta);
  for(int i=0; i<neta && !infile.eof(); i++){
    infile >> value;
    m_etaref.push_back(value);
  }
  if(nalpha<0)nalpha = 0;
  if(nalpha>nmax) nalpha=nmax;
  m_phibins.clear();
  m_phibins.reserve(nalpha);
  for(int i=0; i<nalpha && !infile.eof(); i++){
    infile >> value;
    m_phibins.push_back(value);
  }
  if(nxbinsibl>0){ // IBL
    if(nxbinsibl>nmax) nxbinsibl=nmax;
    m_iblphierror.clear();
    m_iblphierror.reserve(nxbinsibl);
    for(int ib1=0; ib1<nxbinsibl; ib1++){
      infile >> value;
      m_iblphierror.push_back(value);
    }
  }
  int nb1=ncsx*nalpha; // number of barrel phi bins
  int nb2=ncsx*ncsy*neta; // number of barrel eta bins
  m_barrelphierror.clear();
  m_barreletaerror.clear();
  m_barrelphierror.reserve(nb1);
  m_barreletaerror.reserve(nb2);
  for(int ib1=0; ib1<nb1; ib1++){
     infile >> value;
     m_barrelphierror.push_back(value);
  }
  if(nybinsibl>0){ // IBL
    if(nybinsibl>nmax) nybinsibl=nmax;
    m_ibletaerror.clear();
    m_ibletaerror.reserve(nybinsibl);
    for(int ib1=0; ib1<nybinsibl; ib1++){
      infile >> value;
      m_ibletaerror.push_back(value);
    }
  }
  for(int ib2=0; ib2<nb2; ib2++){
    infile >> value;
    m_barreletaerror.push_back(value);
  }
  infile.close();  
}

} 

