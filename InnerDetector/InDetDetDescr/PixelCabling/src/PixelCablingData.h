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
#include <boost/unordered_map.hpp>


// Provide hash_value function for unordered maps
std::size_t hash_value(Identifier const& id);



/** Class containing maps with the relationships between online-to-offline
 *  identifiers, ROD-to-ROB etc.
 */
class PixelCablingData
{

 public:

   // constructor
   PixelCablingData();

   // destructor
   virtual ~PixelCablingData();

   // avoid default copy ctor / assignment operator
   PixelCablingData(const PixelCablingData &other) = delete;
   PixelCablingData& operator= (const PixelCablingData &other) = delete;

   /** @name Functions to add entries to the maps */
   ///@{
   void add_entry_onoff(const uint64_t, const Identifier);   // add entries to m_idMap_onoff
   void add_entry_offon(const Identifier, const uint64_t);   // add entries in m_idMap_offon
   void add_entry_offrob(const Identifier, const uint32_t);   // add entries in m_idMap_offrob
   void add_entry_rodrob(int rodid, int robid);               // add entries in m_idMap_rodrob
   void add_entry_robrod(int robid, int rodid);
   void add_entry_offlineList(const uint32_t, const Identifier);   // add entries in m_offlineListVect
   void add_entry_DCSoffline(std::string, const Identifier);   // add entries in m_offlineListVect
   ///@}

   /** @name Functions to search for entry in the maps */
   ///@{
   Identifier find_entry_onoff(const uint64_t) const;   // search entry in the map m_idMap_onoff - get the offlineId from the onlineId
   uint64_t find_entry_offon(const Identifier) const;   //  search entry in the map m_idMap_offon - get the onlineId from the offlineId
   uint32_t find_entry_offrob(const Identifier) const;   // search entry in the map m_idMap_offrob - get the ROBId from the offlineId
   int find_entry_rodrob(const int rodId);
   int find_entry_robrod(const int robId);
   std::deque<Identifier> find_entry_offlineList(uint32_t);   // return entries from m_offlineListVect for a ROBId
   Identifier find_entry_DCSoffline(std::string);
   std::string find_entry_offlineDCS(const Identifier);
   uint64_t getOnlineIdFromRobId(const uint32_t robid, const uint32_t link);
   ///@}

   /** Clear all the maps **/
   void clear_all_maps();

   /** @name FE-I4 HitDiscCnfg functions */
   ///@{
   void setCommonHitDiscCngf(int hitdisccnfg) {m_commonHitDiscCnfg = hitdisccnfg;}
   void add_entry_HitDiscCnfg(const uint32_t, const int);
   int getHitDiscCnfg(const uint32_t frontendId);
   void printHitDiscCnfg(); // for debugging
   ///@}

   /** Return vector containing all RODs */
   std::vector<uint32_t>& get_allRods();
   /** Return vector containing all ROBs **/
   std::vector<uint32_t>& get_allRobs();

   /** Get the size og the on-off map */
   unsigned int get_size_onoff() {return m_idMap_onoff.size();}

   /** Set the readoutspeed map equal an input map */
   void set_readout_map( std::map<uint32_t,bool> rodReadoutMap);

   /** Functions providing access to the cabling maps. These
    * should onlt be accessed by the corresponding wrapper
    * functions in PixelCablingSvc.
    * (return std::map rather than the boost ones)
    */
   ///@{
   std::map<uint64_t, Identifier> get_idMap_onoff();
   std::map<Identifier, uint64_t> get_idMap_offon();
   std::map<Identifier, uint32_t> get_idMap_offrob();
   std::map<int,int> get_idMap_rodrob();
   std::map<int,int> get_idMap_robrod();
   std::map<std::string, Identifier> get_idMapDCSoff();
   ///@}



 private:

   /** @name These are all the maps */
   ///@{
   boost::unordered_map<uint64_t, Identifier> m_idMap_onoff;        ///< offline identifier -> online identifier map
   boost::unordered_map<Identifier, uint64_t> m_idMap_offon;        ///< online identifier -> offline identifier map
   boost::unordered_map<Identifier, uint32_t> m_idMap_offrob;       ///< offline identifier -> ROBId map
   boost::unordered_map<int,int> m_idMap_rodrob;                    ///< RODId -> ROBId map
   boost::unordered_map<int,int> m_idMap_robrod;                    ///< ROBId -> RODId map (reverse of m_idMap_rodrob)
   std::map<uint32_t, std::deque<Identifier> > m_offlineListVect;   ///< ROBId -> offline identifier list
   boost::unordered_map<std::string, Identifier> m_idMapDCSoff;     ///< DCS name -> offline identifier
   std::map<uint32_t,bool> m_rodReadoutMap;     ///< Readout speed for each ROD. false=40MBit, true=80MBit
   ///@}


   /** Map of HitDiscCnfg values for each IBL FE
    * Format: [0xLDDRRR, H], where L = link, DD = subdet id, RRRR = rob id, H = HitDiscCnfg
    */
   boost::unordered_map<uint32_t, int> m_HitDiscCnfgMap;

   /** The most common HDC setting */
   int m_commonHitDiscCnfg;


   std::vector<uint32_t>* m_allRods;   // vector containing all RODs in the m_idMap_rodrob
   std::vector<uint32_t>* m_allRobs;   // vector containing all ROBs

};

CLASS_DEF( PixelCablingData , 107266278 , 1 )   // class definition with CLID

#endif   // PIXELCABLINGDATA_H
