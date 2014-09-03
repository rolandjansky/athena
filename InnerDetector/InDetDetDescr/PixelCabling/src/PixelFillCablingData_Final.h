/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFillCablingData_Final.h
//   Header file for class PixelFillCablingData_Final
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// reading the final mapping file and depending on the selected bandwidth filling the maps
// - FillMaps() - call intialiseFromFile() with the correct file name
// - initialiseFromFile() - reading the cosmic mapping file and depending on the selected bandwidth filling the maps
// - commentOrBlankLine() - for finding whether the line is valid data or just a comment or blank line
///////////////////////////////////////////////////////////////////
//  Version 00-03-51 25/04/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef PIXELFILLCABLINGDATA_FINAL_H
#define PIXELFILLCABLINGDATA_FINAL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdContext.h"

#include <map>
#include <stdint.h>
#include <string>

class PixelCablingData;
class PixelID;

class PixelFillCablingData_Final : public AthAlgTool 
{

 public:
	 
   // constructor
   PixelFillCablingData_Final( const std::string& type, const std::string& name, const IInterface* parent);

   // destructor
   virtual ~PixelFillCablingData_Final();
  
   // AlgTool InterfaceID
   static const InterfaceID& interfaceID();
  
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   // Fills maps from associated mapping file in 'share' directory
   PixelCablingData* FillMaps();

   inline void setMappingFile(std::string file){ m_final_mapping_file = file; }

   std::map<uint32_t,bool> rodReadoutMap; // save readout speed for each ROD. Is set to 40 MBit (false) by default

 private:

   IdContext m_cntxpixel;

   // properties
   std::string m_final_mapping_file;  

   const PixelID* m_idHelper;
   PixelCablingData* m_cabling;
    
   // decide whether map definition file line is comment or blank
   bool commentOrBlankLine(std::string s) const;
  
   // fill maps from file in 'share' directory
   PixelCablingData* initialiseFromFile(const std::string fname);
  
};

#endif   // PIXELFILLCABLINGDATA_FINAL
