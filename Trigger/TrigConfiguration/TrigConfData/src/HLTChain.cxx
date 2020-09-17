/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTChain.h"

TrigConf::Chain::Chain()
{}

TrigConf::Chain::Chain(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   load();
}

TrigConf::Chain::Chain(const std::string & name, const boost::property_tree::ptree & data) 
   : DataStructure(name, data)
{
   load();
}

void
TrigConf::Chain::update()
{
   load();
}

void
TrigConf::Chain::load()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name", true, m_name);
}

TrigConf::Chain::~Chain()
{}

std::string
TrigConf::Chain::className() const {
   return "Chain";
}


unsigned int
TrigConf::Chain::counter() const
{
   return getAttribute<unsigned int>("counter");
}

unsigned int
TrigConf::Chain::namehash() const
{
   return getAttribute<unsigned int>("nameHash");
}

const std::string &
TrigConf::Chain::l1item() const
{
   return getAttribute("l1item");
}


std::vector<std::string>
TrigConf::Chain::l1thresholds() const
{

   std::vector<std::string> thrV;
   const auto & thrs = getList("l1thresholds");
   if( !thrs.empty() ) {
      thrV.reserve(thrs.size());
      for( auto & thr : thrs ) {
         thrV.emplace_back( thr.getValue<std::string>() );
      }
   } 
   return thrV;
}


std::vector<std::string>
TrigConf::Chain::streams() const
{

   std::vector<std::string> strlist;
   const auto & streams = getList("streams");
   if( !streams.empty() ) {
      strlist.reserve(streams.size());
      for( auto & stream : streams ) {
         strlist.emplace_back( stream.getValue<std::string>() );
      }
   }
   return strlist;
}


std::vector<std::string>
TrigConf::Chain::groups() const
{

   std::vector<std::string> grouplist;
   const auto & groups = getList("groups", /*ignoreIfMissing=*/ true);
   if( !groups.empty() ) {
      grouplist.reserve(groups.size());
      for( auto & group : groups ) {
         if (group.hasAttribute("name")) {
            grouplist.emplace_back( group["name"] );
         } else if (group.isValue()) {
            grouplist.emplace_back( group.getValue<std::string>() );
         }
      }
   } 

   return grouplist;
}

std::vector<std::string>
TrigConf::Chain::sequencers() const
{

   std::vector<std::string> seqlist;
   const auto & seqs = getList("sequencers");
   if( !seqs.empty() ) {
      seqlist.reserve(seqs.size());
      for( auto & seq : seqs ) {
         seqlist.emplace_back( seq.getValue<std::string>() );
      }
   }
   return seqlist;
}

