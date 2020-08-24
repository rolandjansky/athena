/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSDATA_SPECIALPIXELMAP_H
#define PIXELCONDITIONSDATA_SPECIALPIXELMAP_H

#include "PixelCoralClientUtils/SpecialPixelMap.hh"

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include "CxxUtils/checker_macros.h"
#include<map>
#include<string>


/** 
 * Classes to hold data about special pixels in the detector store and make it
 * accessible to athena components.
 *
 * ruwiedel@physik.uni-bonn.de 
 *
 */


namespace coral{
  class Blob;
}
class IdentifierHash;
class ModuleSpecialPixelMap;

/** A class for a map of special pixels for the whole pixel detector
    or any subgroup of modules. A DetectorSpecialPixelMap is an std::vector
    of pointers to ModuleSpecialPixelMaps which hold the actual pixel
    status codes. A ModuleSpecialPixelMap is accessed by its IdentifierHash.
 */

class DetectorSpecialPixelMap : public PixelCoralClientUtils::DetectorSpecialPixelMap{

 public: 
  DetectorSpecialPixelMap();
  DetectorSpecialPixelMap(const DetectorSpecialPixelMap& dspm);                 //!< construct a copy
  DetectorSpecialPixelMap(const std::vector<ModuleSpecialPixelMap*>& modules);  //!< construct from contents
  ~DetectorSpecialPixelMap();

  DetectorSpecialPixelMap& operator=(const DetectorSpecialPixelMap& spm);
  DetectorSpecialPixelMap& operator+=(const DetectorSpecialPixelMap& spm);     //!< merge two pixel maps into one
  
  const ModuleSpecialPixelMap* module(IdentifierHash moduleHash) const; //!< const access to a ModuleSpecialPixelMap*

};

/** A class for a map of special pixels for a single module of the pixel
    detector. A ModuleSpecialPixelMap is an std::map of the status codes 
    (unsigned integers) for all pixels with special properties on a module.
    The possible status codes are explained on the wiki page
    https://twiki.cern.ch/twiki/bin/view/Atlas/PixelCOOLoffline#Dead_and_special_pixels_map

    The status for a pixel can be retrieved with pixelStatus(pixelID).
    The pixel ID can be calculated with encodePixelID. In addition to 
    pixelStatus, several accessors for individual status bits are provided.
 */


class ModuleSpecialPixelMap : public PixelCoralClientUtils::ModuleSpecialPixelMap{

 public:
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
			 std::vector<std::vector<unsigned int> > column_pair_status, unsigned int mchips); 
   //!< construct from contents
   ~ModuleSpecialPixelMap();

   ModuleSpecialPixelMap& operator=(const ModuleSpecialPixelMap& mspm);
   ModuleSpecialPixelMap& operator+=(ModuleSpecialPixelMap mspm); 
   //!< merge two pixel maps into one
   
   void print(int component, unsigned int layer, unsigned int phi, int eta,
              bool verbosePixelID = true, bool binaryPixelStatus = true) const; 
   //!< print map to screen

   StatusCode fill_from_string(const std::string& clob); 
   //!< fills the map from a string, used for conversion of data from the database
   StatusCode fill_from_blob(const coral::Blob& blob); 
   //!< fills the map from a blob, used for conversion of data from the database
};

CLASS_DEF(DetectorSpecialPixelMap, 203854058, 1) 
CLASS_DEF(ModuleSpecialPixelMap, 84153642, 1)

CONDCONT_DEF( DetectorSpecialPixelMap, 70046420); 
CONDCONT_DEF( ModuleSpecialPixelMap, 153445648);

#endif
