/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/CTPFiles.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

TrigConf::CTPFiles::CTPFiles() : 
   L1DataBaseclass(),
   m_Lvl1MasterTableId(0),
   m_smxId(0),      
   m_LoadCtpcoreFiles(true),
   m_LoadCtpinFiles(true),
   m_LoadCtpmonFiles(true)
{}
  
TrigConf::CTPFiles::~CTPFiles() {}

// get the files 
std::vector<uint32_t>
TrigConf::CTPFiles::ctpcoreLUT() {
   return m_ctpcoreLUT;
}

void
TrigConf::CTPFiles::setCtpcoreLUT( const std::vector<u_int>& vec)
{
   if(vec.size() != ALL_CTPCORELUT_SIZE) 
      throw std::runtime_error( "CTPFiles: LUT size incorrect" );
   m_ctpcoreLUT = vec;
}     

std::vector<uint32_t> 
TrigConf::CTPFiles::ctpcoreCAM() {
   return m_ctpcoreCAM;
}

void
TrigConf::CTPFiles::setCtpcoreCAM( const std::vector<u_int>& vec ) {
   if(vec.size() != ALL_CTPCORECAM_SIZE) 
      throw std::runtime_error( "CTPFiles: CAM size incorrect" );
   m_ctpcoreCAM = vec;
}

// get the different decoder & selection files 
std::vector<uint32_t>
TrigConf::CTPFiles::ctpinMonSelectorSlot7() {
   return m_ctpinMonSelectorSlot7;
}

void
TrigConf::CTPFiles::setCtpinMonSelectorSlot7( const std::vector<u_int>& vec ) {
   if(vec.size() != ALL_CTPINMONSEL_SIZE) 
      throw std::runtime_error( "CTPFiles: MonSel size slot 7 incorrect" );
   m_ctpinMonSelectorSlot7 = vec;
}

std::vector<uint32_t>
TrigConf::CTPFiles::ctpinMonSelectorSlot8() {
   return m_ctpinMonSelectorSlot8;
}

void
TrigConf::CTPFiles::setCtpinMonSelectorSlot8( const std::vector<u_int>& vec ) {
   if(vec.size() != ALL_CTPINMONSEL_SIZE) 
      throw std::runtime_error( "CTPFiles: MonSel size slot 8 incorrect" );
   m_ctpinMonSelectorSlot8 = vec;
}


std::vector<uint32_t>
TrigConf::CTPFiles::ctpinMonSelectorSlot9() {
   return m_ctpinMonSelectorSlot9;
}

void
TrigConf::CTPFiles::setCtpinMonSelectorSlot9( const std::vector<u_int>& vec ) {
   if(vec.size() != ALL_CTPINMONSEL_SIZE) 
      throw std::runtime_error( "CTPFiles: MonSel size slot 9 incorrect" );
   m_ctpinMonSelectorSlot9 = vec;
}


std::vector<uint32_t>
TrigConf::CTPFiles::ctpmonSelector() {
   return m_ctpmonSelector;
}


void
TrigConf::CTPFiles::setCtpmonSelector( const std::vector<u_int>& vec ) {
   if(vec.size() != ALL_CTPMONSELECTOR_SIZE) 
      throw std::runtime_error( "CTPFiles: CTPMON Selector size incorrect" );
   m_ctpmonSelector = vec;
}
 

std::vector<uint32_t>
TrigConf::CTPFiles::ctpinMonDecoderSlot7() {
   return m_ctpinMonDecoderSlot7;
}

void TrigConf::CTPFiles::setCtpinMonDecoderSlot7( const std::vector<u_int>& vec ) {
   if(vec.size() != ALL_CTPINMONDEC_SIZE) 
      throw std::runtime_error( "CTPFiles: MonSel size slot 7 incorrect" );
   m_ctpinMonDecoderSlot7 = vec;
}

std::vector<uint32_t>
TrigConf::CTPFiles::ctpinMonDecoderSlot8() {
   return m_ctpinMonDecoderSlot8;
}

void
TrigConf::CTPFiles::setCtpinMonDecoderSlot8(const std::vector<u_int>& vec) {
   if(vec.size() != ALL_CTPINMONDEC_SIZE) 
      throw std::runtime_error( "CTPFiles: MonSel size slot 8 incorrect" );
   m_ctpinMonDecoderSlot8 = vec;
}


std::vector<uint32_t> TrigConf::CTPFiles::ctpinMonDecoderSlot9() {
   return m_ctpinMonDecoderSlot9;
}

void
TrigConf::CTPFiles::setCtpinMonDecoderSlot9(const std::vector<u_int>& vec) {
   if(vec.size() != ALL_CTPINMONDEC_SIZE) 
      throw std::runtime_error( "CTPFiles: MonSel size slot 9 incorrect" );
   m_ctpinMonDecoderSlot9 = vec;
}

std::vector<uint32_t>
TrigConf::CTPFiles::ctpmonDecoder() {
   return m_ctpmonDecoder;
}


void
TrigConf::CTPFiles::setCtpmonDecoder(const std::vector<u_int>& vec) {
   if(vec.size() != ALL_CTPMONDECODER_SIZE) 
      throw std::runtime_error( "CTPFiles: CTPMON Decoder size incorrect" );
   m_ctpmonDecoder = vec;
}
 
// get files stop 

void
TrigConf::CTPFiles::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "CTPFiles: " << endl; 
   cout << indent << "id:\t\t\t"                 << id() << endl; 
   cout << indent << "Name: \t\t\t"               << name() << endl;
   cout << indent << "Version: \t\t"            << version() << endl;
   cout << indent << "Print the LUT" << endl;

   for (unsigned int i=0; i<(ALL_CTPCORELUT_SIZE); i++) 
      cout << indent << i << "\t:t0x" << setw(8) << setfill('0') 
           << hex << m_ctpcoreLUT[i] << dec << endl;

   cout << indent << "Print the CAM" << endl;
   for (unsigned int i=0; i<(ALL_CTPCORECAM_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpcoreCAM[i] << dec << endl;
    
   cout << indent << "Print the CTPIN mon decoders slot 7" << endl;
   for (unsigned int i=0; i<(ALL_CTPINMONDEC_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpinMonDecoderSlot7[i] << dec << endl;
   cout << indent << "Print the CTPIN mon decoders slot 8" << endl;
   for (unsigned int i=0; i<(ALL_CTPINMONDEC_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpinMonDecoderSlot8[i] << dec << endl;
   cout << indent << "Print the CTPIN mon decoders slot 9" << endl;
   for (unsigned int i=0; i<(ALL_CTPINMONDEC_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpinMonDecoderSlot9[i] << dec << endl;

   cout << indent << "Print the CTPIN mon selectors slot 7" << endl;
   for (unsigned int i=0; i<(ALL_CTPINMONSEL_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpinMonSelectorSlot7[i] << dec << endl;
   cout << indent << "Print the CTPIN mon selectors slot 8" << endl;
   for (unsigned int i=0; i<(ALL_CTPINMONSEL_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpinMonSelectorSlot8[i] << dec << endl;
   cout << indent << "Print the CTPIN mon selectors slot 9" << endl;
   for (unsigned int i=0; i<(ALL_CTPINMONSEL_SIZE); i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpinMonSelectorSlot9[i] << dec << endl;
    
   cout << indent << "Print the CTPMON selector" << endl;
   for (unsigned int i=0; i < ALL_CTPMONSELECTOR_SIZE; i++) 
      cout << indent << i << "\t:\t" << setw(2) << setfill(' ') 
           << m_ctpmonSelector[i] << endl;
   cout << indent << "Print the CTPMON decoder" << endl;
   for (unsigned int i=0; i < ALL_CTPMONDECODER_SIZE; i++) 
      cout << indent << i << "\t:\t0x" << setw(8) << setfill('0') 
           << hex << m_ctpmonDecoder[i] << dec << endl;

   cout << indent << "SMX no " << m_smxId << " '" << m_smxName << "'" << endl;
   cout << indent << "VHDL slot 7 : " << endl;
   cout << m_ctpinSmxVhdlSlot7 << endl;
   cout << indent << "VHDL slot 8 : " << endl;
   cout << m_ctpinSmxVhdlSlot8 << endl;
   cout << indent << "VHDL slot 9 : " << endl;
   cout << m_ctpinSmxVhdlSlot9 << endl;

   cout << indent << "SVFI slot 7 : " << endl;
   cout << m_ctpinSmxSvfiSlot7 << endl;
   cout << indent << "SVFI slot 8 : " << endl;
   cout << m_ctpinSmxSvfiSlot8 << endl;
   cout << indent << "SVFI slot 9 : " << endl;
   cout << m_ctpinSmxSvfiSlot9 << endl;
      
   cout << indent << "SMX Output file: " << endl;
   cout << m_ctpinSmxOutput << endl;
} 
