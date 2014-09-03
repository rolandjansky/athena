/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFillCablingData_CoralDB.h
//   Header file for class PixelFillCablingData_CoralDB
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// reading the final mapping file and depending on the selected bandwidth filling the maps
// - FillMaps() - call intialiseFromFile() with the correct file name
// - initialiseFromFile() - reading the cosmic mapping file and depending on the selected bandwidth filling the maps
// - commentOrBlankLine() - for finding whether the line is valid data or just a comment or blank line
///////////////////////////////////////////////////////////////////
// 14/05/2008 Florian Hirsch
///////////////////////////////////////////////////////////////////

#ifndef PIXELFILLCABLINGDATA_CORALDB_H
#define PIXELFILLCABLINGDATA_CORALDB_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CoralDB/CoralDB.h"
#include "Identifier/IdContext.h"

#include <map>
#include <stdint.h>
#include <string>


class PixelID;
class PixelCablingData;
class Connection;
class Alias;
class IdContext;

class PixelFillCablingData_CoralDB : public AthAlgTool 
{

 public:
	 
   // constructor
   PixelFillCablingData_CoralDB( const std::string& type, const std::string& name, const IInterface* parent);

   // destructor
   ~PixelFillCablingData_CoralDB();
    
   // AlgTool InterfaceID
   static const InterfaceID& interfaceID();
  
   StatusCode initialize();
   StatusCode finalize();

   // Fills maps from associated mapping file in 'share' directory
   PixelCablingData* FillMaps();

   inline void setBandwidth(int bandwidth){ m_bandwidth = bandwidth; }
   inline void setCoralConnString(std::string connString){ m_coraldbconnstring = connString; }
   inline void setDictTag(std::string tag){ m_dictTag = tag; }
   inline void setConnTag(std::string tag){ m_connTag = tag; }

  
 private:

   IdContext m_cntxpixel;

   // properties
   int m_bandwidth; 

   const PixelID* m_idHelper;
   PixelCablingData* m_cabling;

   // the CoralDB object
   std::string m_dictTag;
   std::string m_connTag;
   CoralDB::CoralDB* m_coraldb;
   std::string m_coraldbconnstring;
   CoralDB::CompoundTag* m_optCompoundTag;

   // OB and ROD maps
   std::map<std::string, std::map<int, int> > m_oblinkmaps_dto2;
   std::map<std::string, std::map<int, int> > m_oblinkmaps_dto;
   std::map<std::string, std::map<int, int> > m_rodbocmaps_40;
   std::map<std::string, std::map<int, int> > m_rodbocmaps_80a;
   std::map<std::string, std::map<int, int> > m_rodbocmaps_80b;
};

#endif   // PIXELFILLCABLINGDATA_CORALDB_H
