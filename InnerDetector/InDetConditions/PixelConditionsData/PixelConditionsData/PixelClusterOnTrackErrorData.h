/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCLUSTERONTRACKERRORDATA_H
#define PIXELCLUSTERONTRACKERRORDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>

/** @class PixelClusterOnTrackErrorData 
  It holds the parametrizations of the uncertainty on the 
  position of pixel clusters when track candidate is available 

  @author Tommaso Lari <lari@mi.infn.it>
**/  

namespace PixelCalib {

class PixelClusterOnTrackErrorData { 
     
  public:
 
    PixelClusterOnTrackErrorData();
    ~PixelClusterOnTrackErrorData();
    
    /** Methods to access the calibration data */ 

    // get/set version number
    int getVersion() const;
    void setVersion(int version);

    // get the number of bins
    int getNumberOfPhiBarrelBins() const;
    int getNumberOfEtaBarrelBins() const;
    // IBL
    int getNumberOfPhiIBLBins() const;
    int getNumberOfEtaIBLBins() const;


    // get parametrizations
    const std::vector<float> getClusterSizeBinsX() const {return m_csx;}
    const std::vector<float> getClusterSizeBinsY() const {return m_csy;}
    const std::vector<float> getEtaBins() const {return m_etaref;}
    const std::vector<float> getIncidenceAngleBins() const {return m_phibins;}
    // IBL
    const std::vector<float> getEtaIBLBins() const {return m_ibletaref;}
    const std::vector<float> getIncidenceAngleIBLBins() const {return m_iblphibins;}


    // old parametrization (analytical formula)
    double getPixelBarrelPhiError(double ang, int phiClusterSize) const; 

    // new parametrization (read from DB)
    float getPixelBarrelEtaError(int ibin) const;
    float getPixelBarrelPhiError(int ibin) const;
    // IBL
    float getPixelIBLEtaError(int ibin) const;
    float getPixelIBLPhiError(int ibin) const;

    void setParameters(const int ncsx, const int ncsy, const int neta, const int nalpha, 
                       int offset, std::vector<float> constants);
    void setPixelBarrelPhiError(int ibin, double error){ 
             m_barrelphierror[ibin] = error; }
    void setPixelBarrelEtaError(int ibin, double error){ 
             m_barreletaerror[ibin] = error; }
    // IBL
    void setPixelIBLPhiError(int ibin, double error){
      m_iblphierror[ibin] = error; }
    void setPixelIBLEtaError(int ibin, double error){
      m_ibletaerror[ibin] = error; }


    int getBarrelBinPhi(double angle, int phiClusterSize) const;

    int getBarrelBinEta(double eta, int etaClusterSize, 
		      int phiClusterSize) const;
    // IBL
    int getIBLBinPhi(double angle, int phiClusterSize) const;
    int getIBLBinEta(double eta, int etaClusterSize) const;

    // save constants on text file
    void Print(std::string file) const;
    // load constants from text file
    void Load(std::string file);
    // IBL

    int getIBLcsxbins() const {return m_csxbinsibl;}
    int getIBLcsybins() const {return m_csybinsibl;}
    int getIBLetabins() const {return m_etabinsibl;}
    int getIBLphibins() const {return m_phibinsibl;}

    void setIBLcsxbins(int icsx){m_csxbinsibl = icsx;}
    void setIBLcsybins(int icsy){m_csybinsibl = icsy;}
    void setIBLetabins(int ieta){m_etabinsibl = ieta;}
    void setIBLphibins(int iphi){m_phibinsibl = iphi;}
 
  private: 
     void Initialize();

     int m_version;    
   // parametrization of errors
    std::vector<float> m_barrelphierror;  
    std::vector<float> m_barreletaerror;
    std::vector<float> m_endcapphierror;  
    std::vector<float> m_endcapetaerror;

    // The bins of parametrization
    std::vector<float> m_csx;  // x cluster size
    std::vector<float> m_csy;  // y cluster size
    std::vector<float> m_etaref; // eta values
    std::vector<float> m_phibins; // Incidence angle values
    // IBL
    int m_csxbinsibl; // IBL csx bins
    int m_csybinsibl; // IBL csy bins 
    int m_etabinsibl;   // IBL eta bins
    int m_phibinsibl; // IBL phi bins   
    std::vector<float> m_ibletaref; // eta values of IBL
    std::vector<float> m_iblphibins; // Incidence angle values of IBL
    std::vector<float> m_iblphierror;
    std::vector<float> m_ibletaerror;

}; 

}

#endif
