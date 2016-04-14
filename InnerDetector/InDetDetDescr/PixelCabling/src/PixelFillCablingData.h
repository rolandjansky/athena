/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFillCablingData.h
//   Header file for class PixelFillCablingData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELFILLCABLINGDATA_H
#define PIXELFILLCABLINGDATA_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdContext.h"

#include <map>
#include <stdint.h>
#include <string>
#include <istream>

class PixelCablingData;
class PixelID;

/**
 * Class to fill the maps in PixelCablingData, either from a text file or from
 * the COOL database.
 *
 * The old classes PixelFillCablingData_Final and PixelFillCablingData_CoralDB
 * have been replaced by this one.
 */
class PixelFillCablingData : public AthAlgTool
{

 public:

   /** Constructor */
   PixelFillCablingData( const std::string& type, const std::string& name, const IInterface* parent);

   /** Destructor */
   virtual ~PixelFillCablingData();

   /** AlgTool InterfaceID */
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /** Fill cabling map from text file (mainly for MC and Run-1)
    * @param filename The text file to be read, must be located
    * in the PixelCabling/share/ directory
    * @return Pointer to filled instance of PixelCablingData
    */
   bool fillMapFromFile(const std::string filename,  PixelCablingData* cabling);

   /** Fill cabling map from COOL database (for Run-2 onwards,
    *  where the CablingMap folder is available)
    * @param data Pointer to the starting address of the
    * coral::Blob to be read
    * @param dump_map_to_file Option to write the mapping info
    * read from COOL into a text file for debugging
    * @return Pointer to filled instance of PixelCablingData
    */
   bool fillMapFromCool(const char* data, unsigned int size, PixelCablingData* cabling, bool dump_map_to_file = false);

   /** Function to fill the map by parsing input istream */
   bool parseAndFill(std::istream& instr,  PixelCablingData* cabling ,bool dump_map_to_file = false);


   /** Set the mapping file to read from */
   inline void setMappingFile(std::string file) {m_mapping_file = file;}

   std::map<uint32_t,bool> rodReadoutMap; // save readout speed for each ROD. Is set to 40 MBit (false) by default

 private:

   IdContext m_cntxpixel;
   std::string m_mapping_file;
   const PixelID* m_idHelper;


};

#endif   // PIXELFILLCABLINGDATA_H
