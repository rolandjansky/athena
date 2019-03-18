/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCHARGEINTERPOLATIONPARAMETERS_H
#define PIXELCHARGEINTERPOLATIONPARAMETERS_H

#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>

/** @class PixelChargeInterpolationParameters
  It holds the charge interpolation parameters used in PixelClusterOnTrackTool

  @author Tommaso Lari <lari@mi.infn.it>
  Incorporating IBL as layer 00, exiting barrel as 1,2,3
  No IBL, layer will set to 1 as default 
**/  

namespace PixelCalib {

class PixelChargeInterpolationParameters { 

  public: 
    PixelChargeInterpolationParameters();
   ~PixelChargeInterpolationParameters() = default;
    PixelChargeInterpolationParameters(const PixelChargeInterpolationParameters&) = delete;

    // get/set version number
    int getVersion() const;
    void setVersion(int version);


    /** Methods to access the calibration data as a function of 
        the cluster size, angle/pseudorapidity, layer number */

    float getDeltaXbarrel(int nRows, float angle, int ilayer=0) const; 
    float getDeltaYbarrel(int nCol, float eta, int ilayer=0) const; 
    float getDeltaXIBL(int nRows, float angle) const;
    float getDeltaYIBL(int nCol, float eta) const;

    float getDeltaXendcap() const; 
    float getDeltaYendcap() const; 

    /** methods to get/set the calibration data as a function of 
        the bin index for the various variables separately */
    float getDeltaX(int iangle, int iclustersize, int ilayer) const;
    float getDeltaY(int ieta, int iclustersize, int ilayer) const;
    int setDeltaX(int iangle, int iclustersize, int ilayer, float value);
    int setDeltaY(int ieta, int iclustersize, int ilayer, float value);

    /** methods to get/set the calibration data as a function of 
        the bin index for the various variables separately */
    float getErrDeltaX(int iangle, int iclustersize, int ilayer) const;
    float getErrDeltaY(int ieta, int iclustersize, int ilayer) const;
    int setErrDeltaX(int iangle, int iclustersize, int ilayer, float value);
    int setErrDeltaY(int ieta, int iclustersize, int ilayer, float value);

    /** methods to get/set the calibration data as a function of 
        the global bin index (the one used i the internal vector
        rapresentation as well as in the database, combining the various 
        variables */
    float getDeltaX(int i) const;
    float getDeltaY(int i) const;
    int setDeltaX(int i, float value);
    int setDeltaY(int i, float value);

    /** methods to get/set the calibration data errors as a function of 
        the global bin index (the one used i the internal vector
        rapresentation as well as in the database, combining the various 
        variables */
    float getErrDeltaX(int i) const;
    float getErrDeltaY(int i) const;
    int setErrDeltaX(int i, float value);
    int setErrDeltaY(int i, float value);

    int getNumberOfXbins() const;
    int getNumberOfYbins() const;

    const std::vector<float>& getEtaBins() const{return m_etabins;}
    const std::vector<float>& getAngleBins() const{return m_phibins;}
    const std::vector<float>& getClusterSizeXBins() const{return m_csx;}
    const std::vector<float>& getClusterSizeYBins() const{return m_csy;}
    
    void setParameters(const int ncsx, const int ncsy, const int neta, 
                       const int nalpha, int offset, std::vector<float> constants);

    void Print(std::string filename) const;
    void Load(std::string filename);

    // IBL
    const std::vector<float>& getIBLEtaBins() const{return m_ibletabins;}
    const std::vector<float>& getIBLAngleBins() const{return m_iblphibins;}
    int getIBLcsxbins() const {return m_csxbinsibl;}
    int getIBLcsybins() const {return m_csybinsibl;}
    int getIBLetabins() const {return m_etaibl;}
    int getIBLphibins() const {return m_alphaibl;}
    void setIBLcsxbins(int icsx){m_csxbinsibl = icsx;}
    void setIBLcsybins(int icsy){m_csybinsibl = icsy;}
    void setIBLetabins(int ieta){m_etaibl = ieta;}
    void setIBLphibins(int ialpha){m_alphaibl = ialpha;}
    //

  private: 

    /** Get the global bin index as a function of the value of the 
        variables of the parametrization */
    int getBarrelBinX(int iclustersize, float angle, int ilayer) const;
    int getBarrelBinY(int iclustersize, float eta, int ilayer) const;


    /** Get global bin index as a function of the separate bin index 
        for each variable of the parametrization */
    int getBarrelBinX(int iclustersize, int iangle, int ilayer) const;
    int getBarrelBinY(int iclustersize, int ieta, int ilayer) const;


    // Parametrization bins
    int m_version;
    int m_nlayerbins; // number of layers 
    std::vector<float> m_etabins;
    std::vector<float> m_phibins;
    std::vector<float> m_csx;
    std::vector<float> m_csy;
    // parametrization values 
    std::vector<float> m_deltax;
    std::vector<float> m_deltay;
    // error on parametrization values
    std::vector<float> m_errdeltax;
    std::vector<float> m_errdeltay;
    // IBL 
    int m_csxbinsibl; // IBL csx bins
    int m_csybinsibl; // IBL csy bins 
    int m_etaibl;   // IBL eta bins
    int m_alphaibl; // IBL phi bins   
    std::vector<float> m_ibletabins;
    std::vector<float> m_iblphibins;

}; 

}

#endif
