/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/LutCam.h"
#include <iostream>
#include <iomanip>

using namespace std;

TrigConf::LutCam::LutCam() : L1DataBaseclass() {
   // init vectors with zeros
   m_lut = std::vector<uint32_t>(ALL_LUT_SIZE,0);
   m_cam = std::vector<uint32_t>(ALL_CAM_SIZE,0);
}

TrigConf::LutCam::~LutCam()
{}

std::vector<uint32_t> TrigConf::LutCam::lut() {
   return m_lut;
}

void
TrigConf::LutCam::setLut(const u_int l[], const int size) {
   // set vector element with range checking - or check range before?
   for (int i=0; i<size; i++) m_lut.at(i) = l[i];
}

std::vector<uint32_t>
TrigConf::LutCam::cam() {
   return m_cam;
}

void
TrigConf::LutCam::setCam(const u_int c[], const int size) {
   // set vector element with range checking
   for (int i=0; i<size; i++) m_cam.at(i) = c[i];
}

void
TrigConf::LutCam::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << "LutCam: " << std::endl;
   printNameIdV(indent);
   std::cout << indent << "Print the LUT" << std::endl;
   for (unsigned int i=0; i<ALL_LUT_SIZE; i++) 
      std::cout << indent << i << "\t:\t0x" << std::setw(8) << std::setfill('0') 
                << std::hex << m_lut[i] << std::dec << std::endl;
   std::cout << indent << "Print the CAM" << std::endl;
   for (unsigned int i=0; i<ALL_CAM_SIZE; i++) 
      std::cout << indent << i << "\t:\t0x" << std::setw(8) << std::setfill('0') 
                << std::hex << m_cam[i] << std::dec << std::endl;
}
