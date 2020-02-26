/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELITKCLUSTERERRORDATA_H
#define PIXELITKCLUSTERERRORDATA_H

#include "Identifier/Identifier.h"

#include "CLIDSvc/CLASS_DEF.h"
#include <string>
#include <vector>


namespace PixelCalib {

class PixelITkClusterErrorData { 

  public: 
    PixelITkClusterErrorData(){ initialize(); }
    ~PixelITkClusterErrorData(){};

    /** Methods to access the calibration data */ 

    std::pair<double,double> getDelta(const Identifier* pixelId) const;
    std::pair<double,double> getDeltaError(const Identifier* pixelId) const;
    std::map< const Identifier, std::tuple<double,double,double,double> > getConstMap() const {return m_constmap;}

    void setDeltaError(const Identifier* pixelId, double delta_x, double error_x, double delta_y, double error_y );
        
    void print(std::string file) const;
    void load(std::string file);

  private: 
     void initialize();

     // map to store all ITk Analogue Clustering constants and errors
     std::map< const Identifier, std::tuple<double,double,double,double> > m_constmap;

}; 

}

#endif
