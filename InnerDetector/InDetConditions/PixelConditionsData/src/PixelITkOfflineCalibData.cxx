/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelITkOfflineCalibData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelConditionsData/PixelITkOfflineCalibData.h"
#include "Identifier/Identifier.h"

#include <sstream>


namespace PixelCalib{

  std::vector<float> PixelITkOfflineCalibData::getConstants() const {

    std::map< const Identifier, std::vector<double> > constMap = m_clustererrordata->getConstMap();

    int entry_size = 9; // pixel Id + period_phi + period_sinheta + delta_x_slope + delta_x_offset + delta_err_x + delta_y_slope + delta_y_offset +  delta_err_y
    int data_size = entry_size*constMap.size();

    std::vector<float> constants;
    constants.reserve(data_size);

    for(auto& x : constMap){

      long long pixelId(x.first.get_compact());
      std::vector<double> value = x.second;

      constants.push_back(pixelId);
      for(auto& y : value) constants.push_back(y);

    }

    return constants;

  }

  void PixelITkOfflineCalibData::dump(){
    m_clustererrordata->print("PixelITkClusterDump.txt");
  }


  void PixelITkOfflineCalibData::setConstants(const std::vector<float> &constants){

    int entry_size = 9;
    int map_size = constants.size()/entry_size;

    for(int i=0;i<map_size;i++){

      long long pixelId_long = constants[i*entry_size];
      std::ostringstream ss;
      ss << "0x" << std::hex << pixelId_long;
      std::string pixelId_str(ss.str());
      Identifier pixelId;
      pixelId.set(pixelId_str);

      double period_phi = constants[i*entry_size + 1];
      double period_sinheta = constants[i*entry_size + 2];
      double delta_x_slope = constants[i*entry_size + 3];
      double delta_x_offset = constants[i*entry_size + 4];
      double delta_err_x = constants[i*entry_size + 5];
      double delta_y_slope = constants[i*entry_size + 6];
      double delta_y_offset = constants[i*entry_size + 7];
      double delta_err_y = constants[i*entry_size + 8];
      
      m_clustererrordata->setDeltaError(&pixelId, period_phi, period_sinheta,
					delta_x_slope, delta_x_offset, delta_err_x,
					delta_y_slope, delta_y_offset, delta_err_y);

    }

    return;

  } 
  
  

} // end of namespace 
