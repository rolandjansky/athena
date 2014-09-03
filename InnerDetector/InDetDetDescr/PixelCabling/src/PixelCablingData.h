/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCablingData.h
//   Header file for class PixelCablingData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// Class used to add and search entries in the maps:
// - add_entry_onoff() - add entries to m_idMap_onoff
// - add_entry_offon() - add entries in m_idMap_offon
// - add_entry_offrob() - add entries in m_idMap_offrob
// - add_entry_rodrob() - add entries in map m_idMap_rodrob
// - find_entry_onoff() - search entry in the map m_idMap_onoff - get the offlineId from the onlineId
// - find_entry_offon() - search entry in the map m_idMap_offon - get the onlineId from the offlineId
// - find_entry_offrob() - search entry in the map m_idMap_offrob - get the ROBId from the offlineId
// - find_entry_rodrob() - search entry in the map m_idMap_rodrob - get the ROBId from the RODId
// and to the list vector:
// - add_entry_offlineList() - add entries in m_offlineListVect
// - find_entry_offlineList() - return entries from m_offlineListVect for a ROBId
///////////////////////////////////////////////////////////////////
//  Version 00-03-26 15/01/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef PIXELCABLINGDATA_H
#define PIXELCABLINGDATA_H


//#define PIXEL_DEBUG
#include "CLIDSvc/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include <map>
#include <deque>
#include <stdint.h>
#include <utility>
#include <string>

class PixelCablingData
{

 public:

   // constructor
   PixelCablingData();

   // destructor
   virtual ~PixelCablingData();

   void add_entry_onoff(const uint64_t, const Identifier);   // add entries to m_idMap_onoff
   void add_entry_offon(const Identifier, const uint64_t);   // add entries in m_idMap_offon
   void add_entry_offrob(const Identifier, const uint32_t);   // add entries in m_idMap_offrob
   void add_entry_rodrob(int rodid, int robid);               // add entries in m_idMap_rodrob
   void add_entry_robrod(int robid, int rodid);
   void add_entry_offlineList(const uint32_t, const Identifier);   // add entries in m_offlineListVect
   void add_entry_DCSoffline(std::string, const Identifier);   // add entries in m_offlineListVect
   Identifier find_entry_onoff(const uint64_t) const;   // search entry in the map m_idMap_onoff - get the offlineId from the onlineId
   uint64_t find_entry_offon(const Identifier) const;   //  search entry in the map m_idMap_offon - get the onlineId from the offlineId
   uint32_t find_entry_offrob(const Identifier) const;   // search entry in the map m_idMap_offrob - get the ROBId from the offlineId
   int find_entry_rodrob(const int rodId);
   int find_entry_robrod(const int robId);
   std::deque<Identifier> find_entry_offlineList(uint32_t);   // return entries from m_offlineListVect for a ROBId
   Identifier find_entry_DCSoffline(std::string);
   std::string find_entry_offlineDCS(const Identifier);
   uint64_t getOnlineIdFromRobId(const uint32_t robid, const uint32_t link);

   std::vector<uint32_t>& get_allRods();
   std::vector<uint32_t>& get_allRobs();
   unsigned int get_size_onoff() {return m_idMap_onoff.size();}    // get the size of the on-off map

   void set_readout_map( std::map<uint32_t,bool> rodReadoutMap);
   /* void copy_offonMap (std::map< Identifier, uint32_t> &outputMap); */
   /* void copy_DCSoffMap (std::map< std::string, Identifier> &outputMap); */
   /* void copy_offrobMap(std::map< Identifier, uint32_t> &outputMap); */

   // Provide access to the cabling maps
   std::map< uint64_t, Identifier > get_idMap_onoff();
   std::map< Identifier, uint64_t> get_idMap_offon();
   std::map< Identifier, uint32_t> get_idMap_offrob();
   std::map< int,int> get_idMap_rodrob();
   std::map< int,int> get_idMap_robrod();
   std::map< std::string, Identifier> get_idMapDCSoff();



 private:

   std::map< uint64_t, Identifier > m_idMap_onoff;   // offline identifier -> online identifier map
   std::map< Identifier, uint64_t> m_idMap_offon;   // online identifier -> offline identifier map
   std::map< Identifier, uint32_t> m_idMap_offrob;   // offline identifier -> ROBId map
   std::map< int,int> m_idMap_rodrob;                    // RODId -> ROBId map
   std::map< int,int> m_idMap_robrod;                    // ROBId -> RODId map (reverse of m_idMap_rodrob)
   std::map< uint32_t, std::deque<Identifier> > m_offlineListVect;   // ROBId -> offline identifier list
   std::map< std::string, Identifier> m_idMapDCSoff; // DCS name -> offline identifier

   std::map<uint32_t,bool> m_rodReadoutMap; // save readout speed for each ROD. false=40MBit, true=80MBit


   std::vector<uint32_t>* m_allRods;   // vector containing all RODs in the m_idMap_rodrob
   std::vector<uint32_t>* m_allRobs;   // vector containing all ROBs

};

CLASS_DEF( PixelCablingData , 107266278 , 1 )   // class definition with CLID

#endif   // PIXELCABLINGDATA_H
