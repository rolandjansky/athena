/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLCTPFilesLoader.h"
#include "TrigConfL1Data/CTPFiles.h"
#include <fstream>
#include <iostream>

#include <typeinfo>
#include <cstring>

bool TrigConf::XMLCTPFilesLoader::load(CTPFiles& ctpFiles) {
   char line[1024];
   std::vector<u_int> ctpcoreLUT(CTPFiles::ALL_CTPCORELUT_SIZE);
   std::vector<u_int> ctpcoreCAM(CTPFiles::ALL_CTPCORECAM_SIZE);
   int n=0;
      
   std::ifstream in(m_ctpcoreCAMFile.c_str());
   while (!in.eof() && in.getline(line, 1024)) {
      if (std::strlen(line)>0) line[std::strlen(line)-1] = '\0';
      if (std::strlen(line)==0) continue;
      sscanf(line, "0x%x", &ctpcoreCAM[n]);
   }
   msg() << "N ctpcoreCAM = " << n << std::endl;
   in.close();
   ctpFiles.setCtpcoreCAM(ctpcoreCAM);
      
   in.open(m_ctpcoreLUTFile.c_str());
   while (!in.eof() && in.getline(line, 1024)) {
      if (std::strlen(line)>0) line[std::strlen(line)-1] = '\0';
      if (std::strlen(line)==0) continue;
      sscanf(line, "0x%x", &ctpcoreLUT[n]);
   }
   msg() << "N ctpcoreLUT = " << n << std::endl;
   in.close();
   ctpFiles.setCtpcoreLUT(ctpcoreLUT);
    
      
   return true;
} 

