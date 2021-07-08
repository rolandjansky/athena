/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCABLINGCONDDATA_H
#define PIXELCABLINGCONDDATA_H

//#define PIXEL_DEBUG
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/Identifier.h"
#include <map>
#include <deque>
#include <stdint.h>
#include <utility>
#include <string>
#include <unordered_map>

struct idHasher {
  std::size_t operator()(const Identifier& id) const {
    return std::hash<Identifier::value_type>{}(id.get_compact());
  }
};

class PixelCablingCondData {
  public:
    PixelCablingCondData();
    virtual ~PixelCablingCondData();
    PixelCablingCondData(const PixelCablingCondData &other) = delete;
    PixelCablingCondData& operator= (const PixelCablingCondData &other) = delete;

    void add_entry_onoff(const uint64_t, const Identifier);   // add entries to m_idMap_onoff
    void add_entry_offon(const Identifier, const uint64_t);   // add entries in m_idMap_offon
    void add_entry_offrob(const Identifier, const uint32_t);   // add entries in m_idMap_offrob
    void add_entry_rodrob(int rodid, int robid);               // add entries in m_idMap_rodrob
    void add_entry_robrod(int robid, int rodid);
    void add_entry_offlineList(const uint32_t, const Identifier);   // add entries in m_offlineListVect
    void add_entry_DCSoffline(const std::string&, const Identifier);   // add entries in m_offlineListVect

    Identifier find_entry_onoff(const uint64_t) const;   // search entry in the map m_idMap_onoff - get the offlineId from the onlineId
    uint64_t find_entry_offon(const Identifier) const;   //  search entry in the map m_idMap_offon - get the onlineId from the offlineId
    uint32_t find_entry_offrob(Identifier offlineId) const;   // search entry in the map m_idMap_offrob - get the ROBId from the offlineId
    int find_entry_rodrob(const int rodid) const;
    int find_entry_robrod(const int robid) const;
    std::deque<Identifier> find_entry_offlineList(uint32_t robid) const; // return entries from m_offlineListVect for a ROBId
    Identifier find_entry_DCSoffline(const std::string& DCSname) const;
    std::string find_entry_offlineDCS(const Identifier offlineId) const;
    uint64_t getOnlineIdFromRobId(uint32_t robid, uint32_t link) const;

    const std::vector<uint32_t>& get_allRods() const;
    const std::vector<uint32_t>& get_allRobs() const;

    unsigned int get_size_onoff() {return m_idMap_onoff.size();}

    void set_readout_map(std::map<uint32_t,bool> rodReadoutMap);

    const std::unordered_map<uint64_t, Identifier>& get_idMap_onoff() const;
    const std::unordered_map<Identifier, uint64_t, idHasher>& get_idMap_offon() const;
    const std::unordered_map<Identifier, uint32_t, idHasher>& get_idMap_offrob() const;
    const std::unordered_map<int,int>& get_idMap_rodrob() const;
    const std::unordered_map<int,int>& get_idMap_robrod() const;
    const std::unordered_map<std::string, Identifier>& get_idMapDCSoff() const;

    void clear();

  private:

    std::unordered_map<uint64_t, Identifier> m_idMap_onoff;        ///< offline identifier -> online identifier map
    std::unordered_map<Identifier, uint64_t, idHasher> m_idMap_offon;        ///< online identifier -> offline identifier map
    std::unordered_map<Identifier, uint32_t, idHasher> m_idMap_offrob;       ///< offline identifier -> ROBId map
    std::unordered_map<int,int> m_idMap_rodrob;                    ///< RODId -> ROBId map
    std::unordered_map<int,int> m_idMap_robrod;                    ///< ROBId -> RODId map (reverse of m_idMap_rodrob)
    std::map<uint32_t, std::deque<Identifier> > m_offlineListVect;   ///< ROBId -> offline identifier list
    std::unordered_map<std::string, Identifier> m_idMapDCSoff;     ///< DCS name -> offline identifier
    std::map<uint32_t,bool> m_rodReadoutMap;     ///< Readout speed for each ROD. false=40MBit, true=80MBit

    std::vector<uint32_t> m_allRods;   // vector containing all RODs in the m_idMap_rodrob
    std::vector<uint32_t> m_allRobs;   // vector containing all ROBs
};

CLASS_DEF( PixelCablingCondData , 107566278 , 1 )   // class definition with CLID

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelCablingCondData, 107566279 );

#endif
