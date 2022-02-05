/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1BunchGroupSet.h"



/*****************************************

  L1BunchGroup

*****************************************/

TrigConf::L1BunchGroup::L1BunchGroup( const boost::property_tree::ptree & data ) 
   : DataStructure(data)
{
   load();
}

TrigConf::L1BunchGroup::~L1BunchGroup()
{}

void
TrigConf::L1BunchGroup::load()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name", true, m_name);
   m_id = getAttribute<size_t>("id");
   for( auto & train : getList("bcids") ) {
      size_t first = train.getAttribute<size_t>("first");
      size_t length = train.getAttribute<size_t>("length");
      m_bunchdef.emplace_back(first,length);
   }
}

size_t
TrigConf::L1BunchGroup::size() const {
   size_t nb = 0;
   for ( const auto & train : m_bunchdef ) {
      nb += train.second;
   }
   return nb;
}

bool
TrigConf::L1BunchGroup::contains(size_t bcid) const {
   for ( const auto & train : m_bunchdef ) {
      if( bcid >= train.first and bcid < train.first+train.second ) {
         return true;
      }
   }
   return false;
}

std::vector<uint16_t>
TrigConf::L1BunchGroup::bunches() const
{
   std::vector<uint16_t> bunches;
   bunches.reserve(size());
   for (const auto &train : m_bunchdef)
   {
      // trains are pairs (first bunch crossing id, number of bunchcrossings)
      for (uint16_t i = 0; i < train.second; ++i)
      {
         bunches.push_back(train.first + i);
      }
   }
   return bunches;
}


/*****************************************

  L1BunchGroupSet

*****************************************/


TrigConf::L1BunchGroupSet::L1BunchGroupSet()
{
}

TrigConf::L1BunchGroupSet::L1BunchGroupSet( const boost::property_tree::ptree & data ) 
   : DataStructure(data)
{
   load();
}

TrigConf::L1BunchGroupSet::~L1BunchGroupSet()
{}

void
TrigConf::L1BunchGroupSet::load()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name", true, m_name);
   m_bunchGroups.resize(s_maxBunchGroups, std::shared_ptr<L1BunchGroup>(nullptr));
   for( auto & bg : data().get_child("bunchGroups") ) {
      auto bg_sptr = std::make_shared<L1BunchGroup>(bg.second);
      m_bunchGroupsByName.emplace( std::piecewise_construct,
                                   std::forward_as_tuple(bg.first),
                                   std::forward_as_tuple(bg_sptr->id()) );
      m_bunchGroups[bg_sptr->id()] = bg_sptr;
   }
}

void
TrigConf::L1BunchGroupSet::clear()
{
   m_bgsk = 0;
   m_bunchGroupsByName.clear();
   m_bunchGroups.clear();
}

const std::shared_ptr<TrigConf::L1BunchGroup> &
TrigConf::L1BunchGroupSet::getBunchGroup(const std::string & name) const {
   return m_bunchGroups[ m_bunchGroupsByName.at(name) ];
}

const std::shared_ptr<TrigConf::L1BunchGroup> &
TrigConf::L1BunchGroupSet::getBunchGroup(size_t id) const {
   return m_bunchGroups[id];
}

std::size_t
TrigConf::L1BunchGroupSet::size() const {
   return std::count_if(m_bunchGroups.begin(), m_bunchGroups.end(), [](const auto &bg){return bg!=nullptr;});
}

std::size_t
TrigConf::L1BunchGroupSet::sizeNonEmpty() const {
   return std::count_if(m_bunchGroups.begin(), m_bunchGroups.end(), [](const auto &bg){return bg->size()!=0;});
}

TrigConf::L1BunchGroupSet::bgPattern_t
TrigConf::L1BunchGroupSet::bgPattern(size_t bcid) const {
   // sanity check that we have enough bits in bgPattern_t for all bunchgroups
   static_assert(sizeof(bgPattern_t)*8 >= s_maxBunchGroups);

   bgPattern_t p{0};
   for (size_t i = 0; i < s_maxBunchGroups; ++i) {
      if (m_bunchGroups[i] && m_bunchGroups[i]->contains(bcid)) p += (1 << i);
   }
   return p;
}

void
TrigConf::L1BunchGroupSet::printSummary(bool detailed) const {
   std::cout << "Name '" << name() << "'" << std::endl;
   std::cout << "Number of non-empty bunchgroups: " << sizeNonEmpty() << std::endl;
   if(detailed) {
      for( const auto & bg : m_bunchGroups ) {
         if (bg) {
            std::cout << "  " << bg->name() << " (id " << bg->id() << ") has " << bg->size() << " bunches" << std::endl;
         }
      }
   } else {
      const auto & paired = getBunchGroup(1);
      if (paired) {
         std::cout << "Bunchgroup " << paired->name() << " has " << paired->size() << " bunches" << std::endl;
      }
   }
}
