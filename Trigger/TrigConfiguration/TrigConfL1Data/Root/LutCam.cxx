/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/LutCam.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace TrigConf;

LutCam::LutCam() : L1DataBaseclass() {
   // init vectors with zeros
   m_lut = std::vector<uint32_t>(ALL_LUT_SIZE,0);
   m_cam = std::vector<uint32_t>(ALL_CAM_SIZE,0);
}

LutCam::~LutCam()
{}

const std::vector<uint32_t> &
LutCam::lut() const {
   return m_lut;
}

void
LutCam::setLut(const u_int l[], const int size) {
   // set vector element with range checking - or check range before?
   for (int i=0; i<size; i++) m_lut.at(i) = l[i];
}

const std::vector<uint32_t> &
LutCam::cam() const {
   return m_cam;
}

void
LutCam::setCam(const u_int c[], const int size) {
   // set vector element with range checking
   for (int i=0; i<size; i++) m_cam.at(i) = c[i];
}

void
LutCam::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "LutCam: " << endl;
   printNameIdV(indent);
   cout << indent << "Print the LUT" << endl;
   for (unsigned int i=0; i<ALL_LUT_SIZE; i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
                << hex << m_lut[i] << dec << endl;
   cout << indent << "Print the CAM" << endl;
   for (unsigned int i=0; i<ALL_CAM_SIZE; i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
                << hex << m_cam[i] << dec << endl;
}
