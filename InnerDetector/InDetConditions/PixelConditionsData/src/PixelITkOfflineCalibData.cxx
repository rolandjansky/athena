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

    std::map< const Identifier, std::tuple<double,double,double,double> > constMap = m_clustererrordata->getConstMap();

    int entry_size = 5; // pixel Id + delta_x +  delta_err_x + delta_y +  delta_err_y
    int data_size = entry_size*constMap.size();

    std::vector<float> constants;
    constants.reserve(data_size);

    for(auto& x : constMap){

      long long pixelId(x.first.get_compact());
      std::tuple<double,double,double,double> value = x.second;

      constants.push_back(pixelId);
      constants.push_back(std::get<0>(value));
      constants.push_back(std::get<1>(value));
      constants.push_back(std::get<2>(value));
      constants.push_back(std::get<3>(value));

    }

    return constants;

  }

  void PixelITkOfflineCalibData::dump(){
    m_clustererrordata->print("PixelITkClusterDump.txt");
  }


  void PixelITkOfflineCalibData::setConstants(const std::vector<float> &constants){

    int entry_size = 5;
    int map_size = constants.size()/entry_size;

    for(int i=0;i<map_size;i++){

      std::ostringstream ss;
      ss << constants[i*entry_size];
      std::string pixelId_str(ss.str());
      Identifier pixelId;
      pixelId.set(pixelId_str);

      double delta_x = constants[i*entry_size + 1];
      double delta_err_x = constants[i*entry_size + 2];
      double delta_y = constants[i*entry_size + 3];
      double delta_err_y = constants[i*entry_size + 4];
      
      m_clustererrordata->setDeltaError(&pixelId,delta_x,delta_err_x,delta_y,delta_err_y);

    }

    return;

  } 
  
  

} // end of namespace 
