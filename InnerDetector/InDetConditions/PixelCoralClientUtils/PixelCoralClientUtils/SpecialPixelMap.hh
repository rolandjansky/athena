/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// SpecialPixelMap.hh
// 
// Classes to hold data about special pixels in the detector store and make it
// accessible to athena components.
//
// author Christoph Ruwiedel <ruwiedel@physik.uni-bonn.de>
//

#ifndef SPECIALPIXELMAP_HH
#define SPECIALPIXELMAP_HH

#include "CoralBase/Blob.h"
#include "CxxUtils/checker_macros.h"

#include<map>
#include<vector>
#include<string>
#include<iostream>

class ModuleSpecialPixelMap;
class SpecialPixelMapSvc;

namespace PixelCoralClientUtils {

  //static unsigned int columnsPerFEI3 = 18;   // number of columns per FEI3
  //static unsigned int rowsPerFEI3    = 164;   // number of rows per FEI3
  //static unsigned int FEI3sPerModule = 16; // number of FEI3s in a row
  //static unsigned int columnsPerFEI4 = 80;   // number of columns per FEI4
  //static unsigned int rowsPerFEI4    = 336;   // number of rows per FEI4
  const unsigned int nmtype(5);
  static const unsigned int columnsPerFEIX[5]={18,80,132,80,132}; // number of columns per FEI3, 4, 50, 51, 52 
  static const unsigned int rowsPerFEIX[5]={164, 336, 672, 339, 678}; // number of rows per FEI3, 4, 50, 51, 52
  static const unsigned int rowsRdoPerFEIX[5]={160, 336, 672, 336, 672}; // number of rows readout per FEI3, 4, 50, 51, 52

class ModuleSpecialPixelMap;

/** A class for a map of special pixels for the whole pixel detector,
    or any subgroup of modules. A DetectorSpecialPixelMap is
    std::vector of pointers to ModuleSpecialPixelMaps which hold the
    actual pixel status codes. A ModuleSpecialPixelMap is accessed by
    its IdentifierHash.
*/

class DetectorSpecialPixelMap : public std::vector<ModuleSpecialPixelMap*>{
   
 public: 
   DetectorSpecialPixelMap();
   DetectorSpecialPixelMap(const DetectorSpecialPixelMap& dspm);                 //!< construct a copy
   DetectorSpecialPixelMap(const std::vector<ModuleSpecialPixelMap*>& modules);  //!< construct from contents
   virtual ~DetectorSpecialPixelMap();

   DetectorSpecialPixelMap& operator=(const DetectorSpecialPixelMap& spm);
   DetectorSpecialPixelMap& operator+=(const DetectorSpecialPixelMap& spm);     //!< merge two pixel maps into one
   void merge(const DetectorSpecialPixelMap* spm);  //!< operation equivalent to operator+=, but more convenient for pointers

   const ModuleSpecialPixelMap* module(unsigned int moduleHash) const; //!< const access to a ModuleSpecialPixelMap*

   void markSpecialRegions();
   //!< automatically identify regions of pixels with the same status and mark them
   void setNeighbourFlags();
   //!< fill the information about special neighbouring pixels, bits 25 - 28

   void clear();

 private:
   void erase(iterator);
   void erase(iterator, iterator);

};

/** A class for a map of special pixels for a single module of the
    pixel detector. A ModuleSpecialPixelMap is a std::map of the
    status codes (unsigned integers) for all pixels with special
    properties on a module. The possible status codes are explained on
    a wiki page:
    https://twiki.cern.ch/twiki/bin/view/Atlas/PixelCOOLoffline#Dead_and_special_pixels_map

    The status for a pixel can be retrieved with
    pixelStatus(pixelID). The pixel ID can be calculated with
    encodePixelID. In addition to pixelStatus, several accessors for
    individual status bits are provided. 
    
    NOTE: pixelid = row*128*2 + column*2 + chip for FEI4
          unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 4) / 2) << 26 ) +
          ( m_pixelID->layer_disk(moduleID) << 24) +
          ( m_pixelID->phi_module(moduleID) << 18) +
          ( (m_pixelID->eta_module(moduleID) + 10) << 13);
 
          pixelid = row*32*16 + column*16 + chip for FEI3
                
          unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
          ( m_pixelID->layer_disk(moduleID) << 23) +
          ( m_pixelID->phi_module(moduleID) << 17) +
          ( (m_pixelID->eta_module(moduleID) + 6) << 13);

**/

class ModuleSpecialPixelMap : private std::map<unsigned int, unsigned int>{

 public:
   friend class ::ModuleSpecialPixelMap;
   friend class ::SpecialPixelMapSvc;

   ModuleSpecialPixelMap();
   ModuleSpecialPixelMap(const ModuleSpecialPixelMap& mspm);                     
   //!< construct a copy
   ModuleSpecialPixelMap(const char* filename, unsigned int mchips);                                
   //!< construct from text file (for details on the format see the README file in PixelConditionsServices/doc)
   ModuleSpecialPixelMap(const std::string& clob, unsigned int mchips);                              
   //!< construct from string
   ModuleSpecialPixelMap(const coral::Blob& blob, unsigned int mchips);
   //!< construct from blob
   ModuleSpecialPixelMap(const std::map<unsigned int, unsigned int>& pixels, 
			 unsigned int module_status,
			 std::vector<unsigned int> chip_status,
			 std::vector<std::vector<unsigned int> > column_pair_status,unsigned int mchips = 16); 
   //!< construct from contents
   virtual ~ModuleSpecialPixelMap();

   ModuleSpecialPixelMap& operator=(const ModuleSpecialPixelMap& mspm);
   ModuleSpecialPixelMap& operator+=(ModuleSpecialPixelMap mspm); 
   //!< merge two pixel maps into one
   
   void merge(const ModuleSpecialPixelMap* mspm); //!< operation equivalent to operator+=, but more convenient for pointers

   unsigned int pixelStatus(unsigned int pixelID) const;              //!< retrieve pixel status as unsigned int
   bool pixelStatusBit(unsigned int pixelID, unsigned int bit) const; //!< retrieve a single status bit
   bool pixelUseful(unsigned int pixelID) const;                      //!< retrieve negation of bit 0 (pixel useful or not)
   bool pixelNoisy(unsigned int pixelID) const;                       //!< retrieve content of bit 15 (pixel noisy or not)
   
   /** only different from 0 if all pixels on the module have the same status and special regions have been marked */
   unsigned int moduleStatus() const;                                                 //!< retrieve module status as unsigned int
   /** only different from 0 if all pixels on the chip have the same status and special regions have been marked */
   unsigned int chipStatus(unsigned int chip) const;                                  //!< retrieve chip status as unsigned int
   /** only different from 0 if all pixels in the column pair have the same status and special regions have been marked */
   unsigned int columnPairStatus(unsigned int chip, unsigned int column_pair) const;  //!< retrieve column pair status as unsigned int

   std::string to_string() const;         
   //!< convert pixel map to string, used when preparing database write
   coral::Blob* to_blob() const;         
   //!< convert pixel map to blob, used when preparing database write
   virtual void print(int component, unsigned int layer, unsigned int phi, int eta,
		      bool verbosePixelID = true, bool binaryPixelStatus = true) const; 
   //!< print map to screen

  static unsigned int pixelType(unsigned int column, unsigned int row, unsigned int mchips = 16);
  //static unsigned int pixelType(unsigned int column, unsigned int row);
             
   //!< returns pixel type (normal, long etc.) -- global 
  static std::vector<unsigned int> decodePixelID(unsigned int pixelID, unsigned int type);
  static unsigned int encodePixelID(unsigned int chip, unsigned int column, unsigned int row, unsigned int mchips); 
  static unsigned int encodePixelID(unsigned int chip, unsigned int column, unsigned int row);

   //!< returns pixel ID to be used in accessors
   static unsigned int encodePixelID(int component, unsigned int module_phi_index,
				     unsigned int pixel_eta_index, unsigned int pixel_phi_index, unsigned int mchips);
   // static unsigned int encodePixelID(int component, unsigned int module_phi_index,
   //                                  unsigned int pixel_eta_index, unsigned int pixel_phi_index);
 
   //!< returns pixel ID to be used in accessors

   void setPixelStatus(unsigned int pixelID, unsigned int status); 
   void setPixelStatusBit(unsigned int pixelID, unsigned int bit);
   void clearPixelStatusBit(unsigned int pixelID, unsigned int bit);
   void setPixelNotUseful(unsigned int pixelID); //!< set pixel status bit 0 
   void setPixelUseful(unsigned int pixelID);    //!< clear pixel status bit 0
   void setPixelNoisy(unsigned int pixelID);     //!< set pixel status bit 15 
   void setPixelNotNoisy(unsigned int pixelID);  //!< clear pixel status bit 15 

   void setModuleStatus(unsigned int status);          
   //!< set module status, clears single pixel information if argument != 0
   void setChipStatus(unsigned int chip, unsigned int status);  
   //!< set chip status, clears single pixel information if argument != 0
   void setColumnPairStatus(unsigned int chip, unsigned int column_pair, unsigned int status); 
   //!< set column pair status, clears single pixel information if argument != 0

   bool fill_from_string(const std::string& clob); 
   //!< fills the map from a string, used for conversion of data from the database
   bool fill_from_blob(const coral::Blob& blob); 
   //!< fills the map from a blob, used for conversion of data from the database

   bool validPixelID(unsigned int chip, unsigned int column, unsigned int row) const;
   //!< checks whether the pixel ID is in the allowed range
   bool validPixelID(unsigned int pixel_eta_index, unsigned int pixel_phi_index) const;
   //!< checks whether the pixel ID is in the allowed range
   bool validPixelID(unsigned int pixelID) const;
   //!< checks whether the pixel ID is in the allowed range
   
   void markSpecialRegions();
   //!< automatically identify regions of pixels with the same status and mark them
   void setNeighbourFlags();
   //!< fill the information about special neighbouring pixels, bits 25 - 28

   typedef std::map<unsigned int, unsigned int>::iterator iterator;
   //!< std::map iterators are forwarded for access to all special pixels on a module
   //!< These iterate *only* over the special pixels that are *not* in a special region, one has to check special regions independently
   typedef std::map<unsigned int, unsigned int>::const_iterator const_iterator;
   //!< std::map iterators are forwarded for access to all special pixels on a module
   //!< These iterate *only* over the special pixels that are *not* in a special region, one has to check special regions independently

   typedef std::map<unsigned int, unsigned int>::size_type size_type;

   size_type size() const;
   //!< only for special pixels that are not in a special region
   bool empty() const;
   //!< only for special pixels that are not in a special region

   iterator begin();
   //!< only for special pixels that are not in a special region, need to check special regions separately
   const_iterator begin() const;
   //!< only for special pixels that are not in a special region, need to check special regions separately

   iterator end();
   //!< only for special pixels that are not in a special region, need to check special regions separately
   const_iterator end() const;
   //!< only for special pixels that are not in a special region, need to check special regions separately

   unsigned int chipsPerModule() const;
   unsigned int chipType() const;

   void setchipsPerModule(unsigned int chipsPerModule);
   unsigned int columnsPerChip() const;
   unsigned int rowsPerChip() const;
   unsigned int rowsrdoPerChip() const;

   
   bool hasSpecialChips() const;
   //!< true if there are any special frontends on the module
   bool hasSpecialColumnPairs() const;
   //!< true if there are any special column pairs on the module
   bool hasSpecialColumnPairs(unsigned int chip) const;
   //!< true if there are any special column pairs on frontend 'chip'

 private:
   unsigned int m_chipsPerModule; // decoded the number of chips*10 + chip type(0=FEI3, 1=FEI4, 2=FEI50, 3=FEI51, 4=FEI52)
   unsigned int m_module_status;
   std::vector<unsigned int> m_chip_status;
   std::vector<std::vector<unsigned int> > m_column_pair_status;

   void resetSpecialRegions();
   void condResetSpecialRegions(unsigned int pixelID, unsigned status);

};



// inline functions

inline bool ModuleSpecialPixelMap::pixelUseful(unsigned int pixelID) const{
  return !pixelStatusBit(pixelID, 0);
}

inline bool ModuleSpecialPixelMap::pixelNoisy(unsigned int pixelID) const{
  return pixelStatusBit(pixelID, 15);
}

inline unsigned int ModuleSpecialPixelMap::moduleStatus() const{
  return m_module_status;
}

inline unsigned int ModuleSpecialPixelMap::chipStatus(unsigned int chip) const{
  if(chip < m_chipsPerModule/10){
    if(m_chip_status.size() != 0){
      return m_chip_status[chip];
    }
    else{
      return 0;
    }
  }
  else{
    std::cout << "In ModuleSpecialPixelMap::chipStatus(unsigned int): Chip number "<<chip<< " > "<<m_chipsPerModule<< " requested " << std::endl;
    return 0;
  }
}

inline unsigned int ModuleSpecialPixelMap::columnPairStatus(unsigned int chip, unsigned int column_pair) const{
  unsigned int itype = std::min(m_chipsPerModule%10,nmtype-1);
  if(chip < (m_chipsPerModule/10) && column_pair < (columnsPerFEIX[itype]/2)){
    if(m_column_pair_status.size() != 0 && m_column_pair_status[chip].size() != 0){
      return m_column_pair_status[chip][column_pair];
    }
    else{
      return 0;
    }
  }
  else{
    std::cout << "Invalid chip/column pair combination requested" << std::endl;
    return 0;
  }
}

inline void ModuleSpecialPixelMap::setPixelNotUseful(unsigned int pixelID){
  setPixelStatusBit(pixelID, 0);
  return;
}

inline void ModuleSpecialPixelMap::setPixelUseful(unsigned int pixelID){
  clearPixelStatusBit(pixelID, 0);
  return;
}

inline void ModuleSpecialPixelMap::setPixelNoisy(unsigned int pixelID){
  setPixelStatusBit(pixelID, 15);
  return;
}

inline void ModuleSpecialPixelMap::setPixelNotNoisy(unsigned int pixelID){
  clearPixelStatusBit(pixelID, 15);
  return;
}

inline bool ModuleSpecialPixelMap::validPixelID(unsigned int pixelID) const{
  int i = m_chipsPerModule%10;  
  return i==0 ? validPixelID(pixelID % 16, (pixelID & (31 << 4))/16, pixelID/512): validPixelID(pixelID % 16, (pixelID & (255 << 4))/16, pixelID/4096);
}

inline bool ModuleSpecialPixelMap::validPixelID(unsigned int chip, unsigned int column, unsigned int row) const{
  unsigned int itype = std::min(m_chipsPerModule%10,nmtype-1);
  return (chip<(m_chipsPerModule/10)&&column<columnsPerFEIX[itype]&&row<rowsPerFEIX[itype]) ? true : false;
} 

inline bool ModuleSpecialPixelMap::validPixelID(unsigned int pixel_eta_index, unsigned int pixel_phi_index) const{
  unsigned int i = std::min(m_chipsPerModule%10,nmtype-1);
  int m =  m_chipsPerModule/10;
  unsigned int rowsMax = m>2 ? 2*rowsPerFEIX[i] : rowsPerFEIX[i];
  unsigned int columnsMax = m>2 ? m/2*columnsPerFEIX[i] : m*columnsPerFEIX[i]; 
  return (pixel_eta_index<columnsMax&&pixel_phi_index<rowsMax)?true:false;
}

inline unsigned int ModuleSpecialPixelMap::chipsPerModule() const{
  return m_chipsPerModule/10;
}

inline unsigned int ModuleSpecialPixelMap::chipType() const{
  return m_chipsPerModule%10;
}

inline void ModuleSpecialPixelMap::setchipsPerModule(unsigned int chipsPerModule){
  m_chipsPerModule = chipsPerModule;
}

inline unsigned int ModuleSpecialPixelMap::columnsPerChip() const{ 
  unsigned int itype = std::min(m_chipsPerModule%10,nmtype-1);
  return columnsPerFEIX[itype]; 
}

inline unsigned int ModuleSpecialPixelMap::rowsPerChip() const{ 
  unsigned int itype = std::min(m_chipsPerModule%10,nmtype-1); 
  return rowsPerFEIX[itype]; 
}

inline unsigned int ModuleSpecialPixelMap::rowsrdoPerChip() const{
  unsigned int itype = std::min(m_chipsPerModule%10,nmtype-1);
  return rowsRdoPerFEIX[itype]; 
}

} // namespace PixelCoralClientUtils
#endif // SPECIALPIXELMAP_HH
