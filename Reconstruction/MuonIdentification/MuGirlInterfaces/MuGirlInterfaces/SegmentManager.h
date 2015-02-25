/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSSEGMENTMANAGER_H
#define MUGIRLNSSEGMENTMANAGER_H

#include "MuonSegment/MuonSegment.h"

namespace MuGirlNS
{
class SegmentManager
{
public:
    enum Type { NONE, CSC, RPC, TGC, MDT, MDT_STAU };

    void addSegment(const Muon::MuonSegment* pSeg, Type type)
    {
        InfoMap::iterator itSeg = m_map.find(pSeg);
        if (itSeg == m_map.end()) {
            m_map.insert(InfoMap::value_type(pSeg, Info(type, false)));
            //std::cout << "Added pSegment=" << pSeg << ",  type=" << type << ",  map size: " << m_map.size() << std::endl;
        }
        else
        {
            itSeg->second.type = type;
            //std::cout << "Modified pSegment=" << pSeg << ",  type=" << type << ",  map size: " << m_map.size() << std::endl;
        }
    }
    void setInStore(const Muon::MuonSegment* pSeg, bool inStore = true)
    {
        InfoMap::iterator itSeg = m_map.find(pSeg);
        if (itSeg != m_map.end()) {
            itSeg->second.inStore = inStore;
            //std::cout << "Modified pSegment=" << pSeg << ",  isStore=" << itSeg->second->inStore << ",  map size: " << m_map.size() << std::endl;
        }
    }
  void setInStore(const std::vector<const Muon::MuonSegment*>& segs, bool inStore = true)
    {
      for( auto seg : segs ) setInStore(seg,inStore);
    }

    bool isNotMDT(const Muon::MuonSegment* pSeg) const
    {
        InfoMap::const_iterator itSeg = m_map.find(pSeg);
        if (itSeg != m_map.end())
        {
            Type type = itSeg->second.type;
            return type != MDT && type != MDT_STAU;
        }
        return false;
    }
    void clear()
    {
        //std::cout << "Segment map size: " << m_map.size() << std::endl;
        for (InfoMap::iterator itSeg = m_map.begin(); itSeg != m_map.end(); itSeg++)
        {
            //std::cout << "pSegment=" << itSeg->first << ",  inStore=" << itSeg->second->inStore
            //          << ",  type=" << itSeg->second->type << std::endl;
            if (!itSeg->second.inStore) {
                delete itSeg->first;
            }
        }
        m_map.clear();
    }

private:
    struct Info
    {
        Type type;
        bool inStore;
        Info(Type type, bool inStore) : type(type), inStore(inStore) {}
    };
    typedef std::map<const Muon::MuonSegment*, Info> InfoMap;
    InfoMap m_map;
}; // class SegmentManager
} // namespace MuGirlNS

#endif // MUGIRLNSSEGMENTMANAGER_H
