/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "CLHEP/Units/SystemOfUnits.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include <iostream>
#include <fstream>
#include <cmath>

namespace PixelCalib{

  // constructor. Setting defaults for everything. Hopefully constants 
  // will be read from database, or possibly from text file
PixelChargeInterpolationParameters::PixelChargeInterpolationParameters(){


    m_version = 0;
        
        // define bins of parametrization

	// eta bins: [-2.5, -2.0, .... 2.5]
	int netabins = 10;
	for(int i=0; i<netabins+1; i++){
  	   m_etabins.push_back(-2.5+0.5*i);
	}
	// alfa (Rphi incidence angle) bins: [-6., -4., ... ,14]
        int nphibins = 10;
	for(int i=0; i<nphibins+1; i++){
	    m_phibins.push_back(-6+2*i);
	}
	// cluster size bins
	int ncs=3;
	for(int i=0; i<ncs+1; i++){
 	    m_csx.push_back(1.5+i);
	    m_csy.push_back(1.5+i);
	}
	m_nlayerbins = 6;

        // Initialize default values for parametrization 
        // Should reading from database fail, these values would be used
	int nxbin=nphibins*m_nlayerbins*ncs;
	int nybin=netabins*m_nlayerbins*ncs;
	//	std::cout << "TOMTOM Setting defaults" << std::endl;
	for(int i=0; i<nxbin; i++){
          m_deltax.push_back(0.);
          m_errdeltax.push_back(0.);
	}
	for(int i=0; i<nybin; i++){
	  m_deltay.push_back(0.);
          m_errdeltay.push_back(0.);
	}
	// IBL 
	// eta bins: [0., 2.7]
	m_etaibl = 0;
	m_alphaibl = 0;
	m_csxbinsibl = 0;
	m_csybinsibl = 0;

	/* IBL initial from DB 
	  int netaibl = 6; 
	  m_ibletabins.reserve(netaibl+1);
	  m_ibletabins.push_back(0.00);
	  m_ibletabins.push_back(0.55);
	  m_ibletabins.push_back(1.275);
	  m_ibletabins.push_back(1.725);
	  m_ibletabins.push_back(2.025);
	  m_ibletabins.push_back(2.25);
	  m_ibletabins.push_back(2.70);

	  // alfa (Rphi incidence angle) bins: [-15.47, 15.47]
	  float phimin = -15.47;
	  float phimax = 15.47;
	  int nalphaibl = 9;
	  m_ibletabins.reserve(nalphaibl+1);
	  for(int i=0; i<nalphaibl+1; i++){
	  m_iblphibins.push_back(phimin +i*(phimax-phimin)/(nalphaibl));
	  }
	*/
}

  void PixelChargeInterpolationParameters::setVersion(int version){ m_version = version; }

  int PixelChargeInterpolationParameters::getVersion() const{ return m_version; }

  void PixelChargeInterpolationParameters::setParameters(const int n1, // number of cluster size bins (x-direction)
                                                 const int n2,   // number of cluster size bins (y-direction)  
						 const int n3,   // number of eta bins
						 const int n4,   // number of incidence angle bins
						 int offset,     // start from c[offset]
						 const float* c){  // vector with bin values
    m_csx.clear();
    m_csy.clear();
    m_etabins.clear();
    m_phibins.clear();
    // n bins means we need n+1 bin extreme values 
    m_csx.reserve(n1+1);
    m_csy.reserve(n2+1);
    m_etabins.reserve(n3+1);
    m_phibins.reserve(n4+1);  
    for(int i=0; i<n1+1; i++){
      m_csx.push_back(c[i+offset]);
    }   
    offset += n1+1;
    for(int i=0; i<n2+1; i++){
      m_csy.push_back(c[i+offset]);
    }   
    offset += n2+1;
    if(m_etaibl>0){ // IBL eta
      m_ibletabins.reserve(m_etaibl+1);
      for(int i = 0; i<m_etaibl+1; i++){
	m_ibletabins.push_back(c[i+offset]);
      }
      offset +=m_etaibl+1;
    }
    if(m_alphaibl>0){ // IBL phi 
      m_iblphibins.reserve(m_alphaibl+1);
      for(int i = 0; i<m_alphaibl+1; i++){
	m_iblphibins.push_back(c[i+offset]);
      }
      offset +=m_alphaibl+1;
    }    
    for(int i=0; i<n3+1; i++){
      m_etabins.push_back(c[i+offset]);
    }   
    offset += n3+1;
    for(int i=0; i<n4+1; i++){
      m_phibins.push_back(c[i+offset]);
    }   
    
    offset +=n4+1;
    
    m_deltax.clear();
    m_deltay.clear();
    m_errdeltax.clear();
    m_errdeltay.clear();
    int nxbin=m_csxbinsibl*m_alphaibl + n4*m_nlayerbins*n1;
    int nybin=m_csybinsibl*m_etaibl + n3*m_nlayerbins*n2;

    m_deltax.reserve(nxbin);
    m_deltay.reserve(nybin);
    m_errdeltax.reserve(nxbin);
    m_errdeltay.reserve(nybin);
    for(int i=0; i<nxbin; i++){
      m_deltax.push_back(0.);
      m_errdeltax.push_back(0.);
    }
    for(int i=0; i<nybin; i++){
      m_deltay.push_back(0.);
      m_errdeltay.push_back(0.);
    }
    
    return;
  }
  
  int PixelChargeInterpolationParameters::getNumberOfXbins() const{ 
    return m_deltax.size();
  }

  int PixelChargeInterpolationParameters::getNumberOfYbins() const{ 
    return m_deltay.size();
  }

  // just return 0 (no correction) outside parametrized range
  float PixelChargeInterpolationParameters::getDeltaX(int i) const{
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_deltax.size()-1) return 0;      
    return m_deltax[i];
  }

  // just return 0 (no correction) outside parametrized range
  float PixelChargeInterpolationParameters::getDeltaY(int i) const{
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_deltay.size()-1) return 0;      
    return m_deltay[i];
  }

  // 0 is an error code
  int PixelChargeInterpolationParameters::setDeltaX(int i, float value){
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_deltax.size()-1) return 0;
    m_deltax[i] = value;
    return 1;
  }

  // 0 is an error code
  int PixelChargeInterpolationParameters::setDeltaY(int i, float value){
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_deltay.size()-1) return 0;
    m_deltay[i] = value;
    return 1;
  }

  
  // just return 0 (no correction) outside parametrized range
  float PixelChargeInterpolationParameters::getErrDeltaX(int i) const{
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_errdeltax.size()-1) return 0;      
    return m_errdeltax[i];
  }

  // just return 0 (no correction) outside parametrized range
  float PixelChargeInterpolationParameters::getErrDeltaY(int i) const{
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_errdeltay.size()-1) return 0;      
    return m_errdeltay[i];
  }

  // 0 is an error code
  int PixelChargeInterpolationParameters::setErrDeltaX(int i, float value){
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_errdeltax.size()-1) return 0;
    m_errdeltax[i] = value;
    return 1;
  }

  // 0 is an error code
  int PixelChargeInterpolationParameters::setErrDeltaY(int i, float value){
    if(i<0) return 0;
    if(static_cast<unsigned int>(i) > m_errdeltay.size()-1) return 0;
    m_errdeltay[i] = value;
    return 1;
  }


 float PixelChargeInterpolationParameters::getDeltaXbarrel(int nrows, 
                                                           float ang,
                                                           int ilayer) const
 {
   int ibin = getBarrelBinX(nrows, ang, ilayer);
   float delta2 = getDeltaX(ibin);
   return delta2;
 }

 
  float PixelChargeInterpolationParameters::getDeltaYbarrel(int ncol, 
                                                        float eta,
                                                        int ilayer) const{
    //   double delta = 0;
   // if(ncol == 2 && eta<2) delta = 0.25*CLHEP::mm+0.13*CLHEP::mm*eta;
   // if(ncol == 2 && eta>2) delta = 0.5*CLHEP::mm;
   // if(ncol == 3 && eta>0.8) delta = 0.25*CLHEP::mm;

   int ibin = getBarrelBinY(ncol, eta, ilayer);
   float delta2 = getDeltaY(ibin);
   return delta2;
 }

  /** methods to get/set the calibration data as a function of 
        the bin index for the various variables separately */

  float PixelChargeInterpolationParameters::getDeltaX(int iangle, 
                                          int iclustersize, int ilayer) const{
    int ibin = getBarrelBinX(iclustersize, iangle, ilayer);
    if(ibin < 0) return 0;
    return getDeltaX(ibin);
  }

  float PixelChargeInterpolationParameters::getDeltaY(int ieta, 
                                          int iclustersize, int ilayer) const{
    int ibin = getBarrelBinY(iclustersize, ieta, ilayer);
    if(ibin < 0) return 0;
    return getDeltaY(ibin);
  }

  int PixelChargeInterpolationParameters::setDeltaX(int iangle, 
                                          int iclustersize, int ilayer,
                                          float value){
    int ibin = getBarrelBinX(iclustersize, iangle, ilayer);
    if(ibin < 0) return 0; // error code
    return setDeltaX(ibin, value);
  }

  int PixelChargeInterpolationParameters::setDeltaY(int ieta, 
                                          int iclustersize, int ilayer,
                                          float value){
    int ibin = getBarrelBinY(iclustersize, ieta, ilayer);
    if(ibin < 0) return 0; // error code
    return setDeltaY(ibin, value);
  }

  /** methods to get/set the calibration data errors as a function of 
        the bin index for the various variables separately */

  float PixelChargeInterpolationParameters::getErrDeltaX(int iangle, 
                                          int iclustersize, int ilayer) const{
    int ibin = getBarrelBinX(iclustersize, iangle, ilayer);
    if(ibin < 0) return 0;
    return getErrDeltaX(ibin);
  }

  float PixelChargeInterpolationParameters::getErrDeltaY(int ieta, 
                                          int iclustersize, int ilayer) const{
    int ibin = getBarrelBinY(iclustersize, ieta, ilayer);
    if(ibin < 0) return 0;
    return getErrDeltaY(ibin);
  }

  int PixelChargeInterpolationParameters::setErrDeltaX(int iangle, 
                                          int iclustersize, int ilayer,
                                          float value){
    int ibin = getBarrelBinX(iclustersize, iangle, ilayer);
    if(ibin < 0) return 0; // error code
    return setErrDeltaX(ibin, value);
  }

  int PixelChargeInterpolationParameters::setErrDeltaY(int ieta, 
                                          int iclustersize, int ilayer,
                                          float value){
    int ibin = getBarrelBinY(iclustersize, ieta, ilayer);
    if(ibin < 0) return 0; // error code
    return setErrDeltaY(ibin, value);
  }

  float PixelChargeInterpolationParameters::getDeltaXendcap() const{
    return 10*CLHEP::micrometer;
  } 
   
  float PixelChargeInterpolationParameters::getDeltaYendcap() const{
    return 10*CLHEP::micrometer;
  }

  int PixelChargeInterpolationParameters::getBarrelBinX(int nrows, 
                                                  float ang, 
                                                  int ilayer) const{
    // look for the phi bin
    int iphi = 0;
    int icsx=0;
    if(m_version<-1 && ilayer ==0){
      int nphi = m_iblphibins.size();
      // check wether we are inside the parametrized range
      if(ang < m_iblphibins[0]) return -1;
      if(ang > m_iblphibins[nphi-1]) return -1;
      if (nrows>=2 && nrows<=m_csxbinsibl+1) icsx=nrows-2;
      else{
	return -1;
      }
      for(int i=0; i<nphi; i++){
	if(ang > m_iblphibins[i]) iphi=i;     // 8
      }
    }
    else{
      int nphi = m_phibins.size();
      // check wether we are inside the parametrized range
      if(ang < m_phibins[0]) return -1;
      if(ang > m_phibins[nphi-1]) return -1;
      for(int i=0; i<nphi; i++){
	if(ang > m_phibins[i]) iphi=i;     // 8
      }
      int ncsx = m_csx.size();  // 4?
      if(nrows>m_csx[ncsx-1]) return -1;
      if(nrows<m_csx[0]) return -1; 
      for(int i=0; i<ncsx; i++){
	if(nrows>(m_csx[i])) icsx=i;   //0
      }
    }
    return getBarrelBinX(icsx, iphi, ilayer);
    // int ibin = iphi*m_nlayerbins*(ncsx-1)+ilayer*(ncsx-1)+icsx; // 8*6*3
    // return ibin;

  }

  int PixelChargeInterpolationParameters::getBarrelBinY(int ncol, 
                                                  float eta, 
                                                  int ilayer) const{
    //	int nxbin=nphibins*m_nlayerbins*ncs;
    //  int nybin=netabins*m_nlayerbins*ncs;
    int ieta=0;
    int icsy=0; 
    if(m_version<-1 && ilayer==0){ // IBL
      int neta = m_ibletabins.size();
      if(eta < m_ibletabins[0]) return -1;
      if(eta > m_ibletabins[neta-1]) return -1;
      for(int i=0; i<neta; i++){
	if(eta > m_ibletabins[i]) ieta=i;   // 10
      }
      if (ncol>=2 && ncol<=m_csybinsibl+1) icsy=ncol-2;
      else{
	return -1;
      }
    }
    else{
      int neta = m_etabins.size();
      if(eta < m_etabins[0]) return -1;
      if(eta > m_etabins[neta-1]) return -1;
      for(int i=0; i<neta; i++){
	if(eta > m_etabins[i]) ieta=i;   // 10
      }
      int ncsy = m_csy.size();
      if(ncol < m_csy[0]) return -1;
      if(ncol > m_csy[ncsy-1]) return -1;
      for(int i=0; i<ncsy; i++){
	if(ncol>m_csy[i]) icsy=i;
      }
    }
    return getBarrelBinY(icsy, ieta,ilayer);
    // int ibin = ieta*m_nlayerbins*(ncsy-1)+ilayer*(ncsy-1)+icsy;
    // return ibin;

  }
   
    /** Get global bin index as a function of the separate bin index 
        for each variable of the parametrization */
   int PixelChargeInterpolationParameters::getBarrelBinX(int nrows, 
                                                  int iangle, 
                                                  int ilayer) const{

    // check wether we are inside parametrized range
     int dl = (ilayer>0) ? 1 : 0;
     if(nrows > int(m_csx.size()-1) || nrows < 0 ||
	iangle > int(m_phibins.size()-1) || iangle < 0 ||
	ilayer <0 || ilayer > (m_nlayerbins+dl)) return -1;
    int ncsx = m_csx.size();
    int ibin = (m_version<-1&&ilayer==0) ? iangle*m_csxbinsibl+nrows: 
      m_csxbinsibl*m_alphaibl+iangle*m_nlayerbins*(ncsx-1)+(ilayer-dl)*(ncsx-1)+nrows; 
    return ibin;
  }

  /** Get global bin index as a function of the separate bin index 
        for each variable of the parametrization */
   int PixelChargeInterpolationParameters::getBarrelBinY(int ncol, 
                                                  int ieta, 
                                                  int ilayer) const{

    // check wether we are inside parametrized range
     int dl = (ilayer>0) ? 1 : 0;
     if(ncol > int(m_csy.size()-1) || ncol < 0 ||
	ieta > int(m_etabins.size()-1) || ieta < 0 ||
	ilayer <0 || ilayer > (m_nlayerbins+dl)) return -1;
     int ncsy = m_csy.size();
     int ibin = (m_version<-1&&ilayer==0) ? ieta*m_csybinsibl+ncol: 
       m_csybinsibl*m_etaibl+ieta*m_nlayerbins*(ncsy-1)+(ilayer-dl)*(ncsy-1)+ncol; 
     return ibin;
   }


  //
  void PixelChargeInterpolationParameters::Print(std::string file) const{

    std::ofstream* outfile = new std::ofstream(file.c_str()); 

    //    std::cout << "TOMTOM printing constants" << std::endl; 
    
    if(m_version < 0) *outfile << m_version << std::endl;
    // The number of bins is the number of bin extremes minus 1.
    int ncsx = m_csx.size()-1;
    int ncsy = m_csy.size()-1;
    int neta = m_etabins.size()-1;
    int nalpha = m_phibins.size()-1;

    // if new format, write number o bins and bin extremes	
    if(m_version <0){ // IBL 

      //    std::cout << "TOMTOM new format" << std::endl; 
      if(m_version<-1){ 
	*outfile <<ncsx<<" "<<ncsy<<" "<<neta<<" "<<nalpha<<" "<<m_csxbinsibl<<
	  " "<<m_csybinsibl<<" "<<m_etaibl<<" "<<m_alphaibl<<std::endl;
      }
      else{ 
	*outfile <<ncsx<<" "<<ncsy<<" "<<neta<<" "<<nalpha<<std::endl;
      }
      // Then write bin extremes

      for(unsigned int i=0; i<m_csx.size(); i++){
	*outfile << m_csx[i] << " ";
      }
      *outfile << std::endl;
      for(unsigned int i=0; i<m_csy.size(); i++){
	*outfile << m_csy[i] << " ";
      }
      *outfile << std::endl;
      if(m_etaibl>0 && m_alphaibl >0 ){ // IBL
	for(unsigned int i=0; i<m_ibletabins.size(); i++){
	  *outfile << m_ibletabins[i] << " ";
	}
	*outfile << std::endl;
	for(unsigned int i=0; i<m_iblphibins.size(); i++){
	  *outfile << m_iblphibins[i] << " ";
	}
	*outfile << std::endl;
      }
      for(unsigned int i=0; i<m_etabins.size(); i++){
	*outfile << m_etabins[i] << " ";
      }
      *outfile << std::endl;
      for(unsigned int i=0; i<m_phibins.size(); i++){
	*outfile << m_phibins[i] << " ";
      }
      *outfile << std::endl;      
    }
    else{

      // std::cout << "TOMTOM old format" << std::endl; 
      *outfile << m_deltax.size() << " " << m_deltay.size() << std::endl;
    }

    // Then bin content
    for(unsigned int ib=0; ib<m_deltax.size(); ib++){
      *outfile << m_deltax[ib] << " " << m_errdeltax[ib]  << std::endl;
    }
    for(unsigned int ie=0; ie<m_deltay.size(); ie++){
      *outfile << m_deltay[ie] << " " << m_errdeltay[ie]  << std::endl;
    }
    outfile->close(); 
    
    delete outfile; 
  }

  // Load costants from file
  void PixelChargeInterpolationParameters::Load(std::string file){
     int nmax(200); // set protection of variables read from file, not too big
     std::ifstream infile(file.c_str());
     int version;
     int nxbins;
     int nybins;
     int ncsx;
     int ncsy;
     int neta;
     int nalpha;
     m_csxbinsibl = 0;
     m_csybinsibl = 0;
     m_etaibl = 0;
     m_alphaibl = 0;
     int nxbinsibl(0);
     int nybinsibl(0);

     float value, error;
     infile >> version;
     if(version > 0){   // old format (without version number)

       m_version = 0;
       nxbins = version;
       infile >> nybins;
       if(nxbins<0)nxbins = 0;
       nxbins = std::min(nxbins, nmax);
       if(nybins<0)nybins = 0;
       nybins = std::min(nybins, nmax);
       m_deltax.clear();
       m_deltay.clear();
       m_deltax.reserve(nxbins);
       m_deltay.reserve(nybins); 

       for(int ib=0; ib<nxbins; ib++){  
         infile >> value; 
         m_deltax.push_back(value);
       }
       for(int ib=0; ib<nybins; ib++){  
         infile >> value; 
         m_deltay.push_back(value); 
       }

       return;

     }
     
     else {    
       m_version = version;
       if(version<-1){ // IBL version 
	 // read the number of bins
	 infile >> ncsx >> ncsy >> neta >> nalpha>>m_csxbinsibl>>m_csybinsibl>>m_etaibl>>m_alphaibl;
	 nxbinsibl = m_csxbinsibl*m_alphaibl;
	 nybinsibl = m_csybinsibl*m_etaibl;
       }
       else{ // current pixel detector 
	 infile >> ncsx >> ncsy >> neta >> nalpha;
       }

       // read bins of parametrization
       // Note: n bins = n+1 bin extremes

       float value; 
       m_csx.clear();
       if(ncsx<0)ncsx = 0;
       ncsx = std::min(ncsx,nmax);
       m_csx.reserve(ncsx+1);
       for(int i=0; i<ncsx+1; i++){
	 infile >> value;
	 m_csx.push_back(value);
       }
       m_csy.clear();
       if(ncsy<0)ncsy = 0;
       ncsy = std::min(ncsy, nmax);
       m_csy.reserve(ncsy+1);
       for(int i=0; i<ncsy+1; i++){
	 infile >> value;
	 m_csy.push_back(value);
       }
       if(m_etaibl>0 && m_alphaibl >0 ){ // IBL
	 m_ibletabins.clear();
	 m_etaibl = std::min(m_etaibl, nmax);
	 m_ibletabins.reserve(m_etaibl+1);
	 for(int i=0; i<m_etaibl+1; i++){
	   infile >> value;
	   m_ibletabins.push_back(value);
	 }
	 m_iblphibins.clear();
	 m_alphaibl = std::min(m_alphaibl, nmax);
	 m_iblphibins.reserve(m_alphaibl+1);
	 for(int i=0; i<m_alphaibl+1; i++){
	   infile >> value;
	   m_iblphibins.push_back(value);
	 }	 
       }
       //
       m_etabins.clear();
       if(neta<0)neta = 0;
       neta = std::min(neta, nmax);
       m_etabins.reserve(neta+1);
       for(int i=0; i<neta+1; i++){
	 infile >> value;
	 m_etabins.push_back(value);
       }
       m_phibins.clear();
       if(nalpha<0)nalpha = 0;
       nalpha = std::min(nalpha, nmax);
       m_phibins.reserve(nalpha+1);
       for(int i=0; i<nalpha+1; i++){
	 infile >> value;
	 m_phibins.push_back(value);
       }

       // Read contents of parametrization
       
       int nxbins = 6*ncsx*nalpha + nxbinsibl;
       int nybins = 6*ncsy*neta + nybinsibl;
       m_deltax.clear();
       m_deltay.clear();
       m_errdeltax.clear();
       m_errdeltay.clear();
       if(nxbins<0)nxbins = 0;
       nxbins = std::min(nxbins, nmax);
       if(nybins<0)nybins = 0;
       nybins =std::min(nybins, nmax);
       m_deltax.reserve(nxbins);
       m_deltay.reserve(nybins); 
       m_errdeltax.reserve(nxbins);
       m_errdeltay.reserve(nybins);
       for(int ib=0; ib<nxbins; ib++){
	 infile >> value >> error; 
	 m_deltax.push_back(value); 
	 m_errdeltax.push_back(error); 
       }
       for(int ib=0; ib<nybins; ib++){  
	 infile >> value >> error; 
	 m_deltay.push_back(value); 
	 m_errdeltay.push_back(error); 
       }       
       return;     
     } // and of check if version number is the first thing on file
  } // end of Load method

} // end of namespace 
