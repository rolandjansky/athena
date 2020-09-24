
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELITKCLUSTERERRORDATA_H
#define PIXELITKCLUSTERERRORDATA_H

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLIDSvc/CLASS_DEF.h"
#include <string>
#include <vector>

namespace PixelCalib {

class PixelITkClusterErrorData { 

  public: 
    PixelITkClusterErrorData();
    ~PixelITkClusterErrorData(){};

    /** Methods to access the calibration data */ 

    std::pair<double,double> getDelta(const Identifier* pixelId,
                                      int sizePhi, double angle,
                                      int sizeZ, double eta) const;
    std::pair<double,double> getDeltaError(const Identifier* pixelId) const;
    std::map< const Identifier, std::vector<double> > getConstMap() const {return m_constmap;}

    void setDeltaError(const Identifier* pixelId,
                       double period_phi, double period_sinheta,
                       double delta_x_slope, double delta_x_offset, double error_x,
                       double delta_y_slope, double delta_y_offset, double error_y );
        
    void print(std::string file) const;
    void load(std::string file);

  private: 
     void initialize();

     // map to store all ITk Analogue Clustering constants and errors
     std::map< const Identifier, std::vector<double> > m_constmap;

     StoreGateSvc* m_detStore;
     const PixelID* m_pixelID;

}; 

}

#endif
