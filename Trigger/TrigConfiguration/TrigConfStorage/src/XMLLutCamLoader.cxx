/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLLutCamLoader.h"
#include <fstream>
#include <iostream>

#include <typeinfo>
#include <cstring>

bool TrigConf::XMLLutCamLoader::load(LutCam& lutcam) {

   char line[1024];
   u_int lut[LutCam::ALL_LUT_SIZE];
   u_int cam[LutCam::ALL_CAM_SIZE];
   int n;
      
   std::ifstream in(m_camFile.c_str());
   n = 0;
   while (!in.eof() && in.getline(line, 1024)) {
      if (std::strlen(line)>0) line[std::strlen(line)-1] = '\0';
      if (std::strlen(line)==0) continue;
      sscanf(line, "0x%x", &cam[n]);
      n ++;
   }
   msg() << "N cam = " << n << std::endl;
   in.close();
   lutcam.setCam(cam, n);
      
   in.open(m_lutFile.c_str());
   n = 0;
   while (!in.eof() && in.getline(line, 1024)) {
      if (std::strlen(line)>0) line[std::strlen(line)-1] = '\0';
      if (std::strlen(line)==0) continue;
      sscanf(line, "0x%x", &lut[n]);
      n ++;
   }
   msg() << "N lut = " << n << std::endl;
   in.close();
   lutcam.setLut(lut, n);
      
   return true;
}
