/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELITKCLUSTERERRORDATA_H
#define PIXELITKCLUSTERERRORDATA_H

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "CLIDSvc/CLASS_DEF.h"
#include <string>
#include <vector>


namespace PixelCalib {

class PixelITkClusterErrorData { 

  public: 
    PixelITkClusterErrorData(){ initialize(); }
    ~PixelITkClusterErrorData(){};

    /** Methods to access the calibration data */ 

    std::pair<double,double> getDelta(int itkregion, int layer) const;
    std::pair<double,double> getDeltaError(int itkregion, int layer) const;
    std::pair<double,double> getDelta(const InDetDD::SiDetectorElement* element) const;
    std::pair<double,double> getDeltaError(const InDetDD::SiDetectorElement* element) const;

    void setITkDeltaError(int xy, int itkregion, int layer, double delta, double error);
        
    void print(std::string file) const;
    void load(std::string file);

  private: 
     void initialize();

     // map to store all ITk Analogue Clustering constants and errors
     std::map< std::tuple<int,int,int> , std::tuple<double,double> > m_constmap;

}; 

}

#endif
