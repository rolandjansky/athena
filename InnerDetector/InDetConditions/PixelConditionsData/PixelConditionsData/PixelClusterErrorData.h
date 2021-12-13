/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCLUSTERERRORDATA_H
#define PIXELCLUSTERERRORDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>

/** @class PixelOfflineCalibData 
  It manages the parametrizations of the uncertainty on the 
  position of pixel clusters 
 
  @author Tommaso Lari <lari@mi.infn.it>
  Incorporating IBL plannar and 3D Clustering dependent -- wm yao (2/11/2013) with a special version number (-2) ?
   
**/  

namespace PixelCalib {

class PixelClusterErrorData { 

  public: 
    PixelClusterErrorData(){ Initialize(); }
    ~PixelClusterErrorData(){};

    /** Methods to access the calibration data */ 

    // get/set version number
    int getVersion() const;
    void setVersion(int version);

    float getPixelBarrelPhiError(int ibin) const;
    float getPixelBarrelEtaError(int ibin) const;
    float getPixelEndcapPhiError(int ibin) const;
    float getPixelEndcapRError(int ibin) const;
    float getPixelIBLPhiError(int ibin) const;
    float getPixelIBLEtaError(int ibin) const;

    // returns an error code (wether ibin is in parametrization range)
    int setPixelBarrelPhiError(int ibin, float error);
    int setPixelBarrelEtaError(int ibin, float error);
    int setPixelEndcapPhiError(int ibin, float error);
    int setPixelEndcapRError(int ibin, float error);
    int setPixelIBLPhiError(int ibin, float error);
    int setPixelIBLEtaError(int ibin, float error);

    // get the parametrization bin 
    int getEndcapBin(int etaClusterSize, int phiClusterSize) const;
    int getBarrelBin(double eta, int etaClusterSize, int phiClusterSize) const;

    // assuming phi and eta parametrizations have the same number of bins...
    int getNumberOfBarrelBins() const { return m_barrelphierror.size(); }
    int getNumberOfEndcapBins() const { return m_endcapphierror.size(); }

    void Print(const std::string& file) const;
    void Load(const std::string& file);

  private: 
     void Initialize();
     // parametrization of errors
    std::vector<float> m_barrelphierror;  
    std::vector<float> m_barreletaerror; 
    std::vector<float> m_endcapphierror;  
    std::vector<float> m_endcapetaerror;
    std::vector<float> m_iblphierror;
    std::vector<float> m_ibletaerror;

    // number of bins of parametrization (details)
    int m_nPhi;
    int m_nZ;
    int m_version;
    // eta values of parametrization
    std::vector<float> m_etaref;
}; 

}

#endif
