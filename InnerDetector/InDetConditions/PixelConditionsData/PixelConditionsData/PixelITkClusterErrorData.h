/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELITKCLUSTERERRORDATA_H
#define PIXELITKCLUSTERERRORDATA_H

#include "CLIDSvc/CLASS_DEF.h"
#include <string>
#include <vector>


namespace PixelCalib {

class PixelITkClusterErrorData { 

  public: 
    PixelITkClusterErrorData(){ initialize(); }
    ~PixelITkClusterErrorData(){};

    /** Methods to access the calibration data */ 

    double getITkDelta(int xy, int itkregion, int layer) const;
    double getITkDeltaError(int xy, int itkregion, int layer) const;

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
