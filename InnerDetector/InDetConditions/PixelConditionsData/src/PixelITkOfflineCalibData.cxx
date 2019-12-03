/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelITkOfflineCalibData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelConditionsData/PixelITkOfflineCalibData.h"

namespace PixelCalib{

  std::vector<float> PixelITkOfflineCalibData::getConstants() const {

    int n_xy = 2; // x + y
    int n_regions = 3; // barrel, inclined, endcap
    int n_layers = 5;

    std::vector<float> constants;
    int data_size = n_xy*n_regions*n_layers;
    constants.reserve(data_size);

    for(int region=0; region<n_regions; region++){
      for(int layer=0; layer<n_layers; layer++){

	std::pair<double,double> delta = m_clustererrordata->getDelta(region,layer);
	std::pair<double,double> delta_err = m_clustererrordata->getDeltaError(region,layer);

	constants.push_back(0); // 0 = phi
	constants.push_back(region);
	constants.push_back(layer);
	constants.push_back(delta.first);
	constants.push_back(delta_err.first);

	constants.push_back(1); // 1 = eta
	constants.push_back(region);
	constants.push_back(layer);
	constants.push_back(delta.second);
	constants.push_back(delta_err.second);
	    
      }
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

      int xy = constants[i*entry_size];
      int region = constants[i*entry_size + 1];
      int layer = constants[i*entry_size + 2];
      float delta = constants[i*entry_size + 3];
      float delta_err = constants[i*entry_size + 4];
      
      m_clustererrordata->setITkDeltaError(xy,region,layer,delta,delta_err);

    }

    return;

  } 
  
  

} // end of namespace 
