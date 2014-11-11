/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "EventShapeEvent/StaticConstants.h"

#include "EventShapeEvent/EventFeature.h"

#include <algorithm>
#include <cmath>

std::map<EventFeature::FeatureType,std::string> EventFeature::m_featureTags = 
  std::map<EventFeature::FeatureType,std::string>();

const std::map<EventFeature::FeatureType,std::string>& 
EventFeature::fillFeatureTags()
{
  if ( m_featureTags.empty() )
    {
      m_featureTags[ETDENSITY]             = "ETDENSITY";
      m_featureTags[ETDENSITY_JETAREA]     = "ETDESNSITY_JETAREA";
      m_featureTags[ETDENSITY_JETAREA_ETA] = "ETDENSITY_JETAREA_ETA";
      m_featureTags[ETDENSITY_ETA_WINDOWS] = "ETDENSITY_ETA_WINDOWS";
      m_featureTags[ETDENSITY_USER]        = "ETDENSITY_USER"; 
      m_featureTags[UNKNOWN]               = "UNKNOWN";
      m_featureTags[DEFAULT]               = "DEFAULT";
    }
  return m_featureTags;
}

EventFeature::FeatureType EventFeature::featureType()
{ return EventFeature::DEFAULT; }

EventFeature::FeatureType EventFeature::featureType(const std::string& tag)
{
  const std::map<FeatureType,std::string>& map = fillFeatureTags();
  std::map<FeatureType,std::string>::const_iterator fMap(map.begin());
  std::map<FeatureType,std::string>::const_iterator lMap(map.end());
  while ( fMap != lMap && tag != fMap->second ) { ++ fMap; }
  return fMap != lMap ? fMap->first : UNKNOWN; 
}

const std::string& EventFeature::featureTag()
{
  const std::map<FeatureType,std::string>& map = fillFeatureTags();
  return map.find(DEFAULT)->second;
}

const std::string& EventFeature::featureTag(FeatureType type)
{
  const std::map<FeatureType,std::string>& map = fillFeatureTags();
  std::map<FeatureType,std::string>::const_iterator fMap(map.find(type));
  return fMap != map.end() ? fMap->second : map.find(UNKNOWN)->second;
}

EventFeature::EventFeature() 
  : m_type(UNKNOWN)
  , m_tag("Default")
  , m_dataWords(0)
  , m_dataBlocks(0)
  , m_featureStructure(0)
  , m_dataList(std::vector<double>())
  , m_structureFixed(false)
{ }

EventFeature::EventFeature(FeatureType type,
			   const std::string& tag,
			   const std::vector<double>& dataList)
  : m_type(type)
  , m_tag(tag)
  , m_dataWords(dataList.size())
  , m_dataBlocks(1)
  , m_dataList(dataList.begin(),dataList.end())
  , m_structureFixed(true)
{
  this->setFeatureStructure(); 
}

EventFeature::EventFeature(FeatureType type,const std::string& tag,
			   unsigned int nWords,
			   const std::vector<double>& dataList)
  : m_type(type)
  , m_tag(tag)
  , m_dataWords(nWords)
  , m_dataBlocks(dataList.size()/m_dataWords)
  , m_dataList(dataList.begin(),dataList.end())
  , m_structureFixed(true)
{
  this->setFeatureStructure();
}

EventFeature::EventFeature(FeatureType type,const std::string& tag,
			   unsigned int nWords)
  : m_type(type)
  , m_tag(tag)
  , m_dataWords(nWords)
  , m_dataBlocks(0)
  , m_dataList(std::vector<double>())
  , m_structureFixed(true)
{
  this->setFeatureStructure(); 
}

EventFeature::EventFeature(const EventFeature& feature)
  : m_type(feature.m_type)
  , m_tag(feature.m_tag)
  , m_dataWords(feature.m_dataWords)
  , m_dataBlocks(feature.m_dataBlocks)
  , m_dataList(feature.dataList().begin(),feature.dataList().end())
  , m_structureFixed(feature.m_structureFixed)
{
  this->setFeatureStructure(); 
}

EventFeature::~EventFeature()
{ }

EventFeature& EventFeature::operator=(const EventFeature& feature)
{
  if (this != &feature) {
    m_type           = feature.m_type;
    m_tag            = feature.m_tag;
    m_dataWords      = feature.m_dataWords;
    m_dataBlocks     = feature.m_dataBlocks;
    m_structureFixed = feature.m_structureFixed; 
    if ( feature.m_dataList.empty() )
    {
      m_dataList.clear();
    }
    else
    {
      m_dataList.resize(feature.m_dataList.size());
      std::copy(feature.m_dataList.begin(),feature.m_dataList.end(),
		m_dataList.begin());
    }
    this->setFeatureStructure();
  }
  return *this;
}

bool EventFeature::dataBlock(size_t blockIdx,std::vector<double>& data) const
{

  // get index range
  index_t idx(0,0);
  if ( !this->indexRange(blockIdx,idx) ) return false;
  // copy data
  for ( size_t i(idx.first); i<idx.second; ++i )
    {
      data.push_back(m_dataList.at(i));
    }
  return !data.empty();
}

bool EventFeature::setDataBlock(size_t blockIdx,
				const std::vector<double>& data,
				bool replace)
{
  // get index range and check if replacement 
  index_t idx(0,0);
  bool inRange(this->indexRange(blockIdx,idx));
  if (  inRange && !replace ) return false;
  // replace or add
  if ( !inRange ) return this->addDataBlock(idx.first,idx.second,data);
  return this->insertDataBlock(idx.first,idx.second,data);
}

bool EventFeature::addDataBlock(const std::vector<double>& data)
{
  index_t idx(0,0);
  this->indexRange(m_dataBlocks+1,idx);
  return addDataBlock(idx.first,idx.second,data);
}

bool EventFeature::addDataBlock(size_t startIdx,size_t endIdx,
				const std::vector<double>& data)
{
  m_dataList.resize(endIdx,EventShapeConstants::nullValue());
  size_t imax(std::min(endIdx-startIdx,data.size()));
  for ( size_t i(0); i<imax; ++i ) { m_dataList[i+startIdx] = data.at(i); }
  this->setFeatureStructure();
  return imax>0;
} 

bool EventFeature::insertDataBlock(size_t startIdx,size_t endIdx,
				   const std::vector<double>& data)
{
  size_t imax(std::min(endIdx-startIdx,data.size()));
  for ( size_t i(0); i<imax; ++i ) { m_dataList[i+startIdx] = data.at(i); } 
  return imax>0;
}

void EventFeature::setNumberDataWords(size_t nWords)
{
  if ( !m_structureFixed &&  nWords <= (unsigned int)NDATAWORDS )
    {
      m_dataWords  = nWords;
      m_dataBlocks = 0;
      this->setFeatureStructure(true);
      m_structureFixed = true;
    }
}

void EventFeature::setNumberDataBlocks(size_t nBlocks)
{
  if ( nBlocks <= (size_t)
       (((unsigned int)NDATABLOCKS)>>(unsigned int)BITSHIFT) )
    {
      m_dataBlocks = nBlocks;
      this->setFeatureStructure(true);
    }
}

void EventFeature::setFeatureStructure(bool updateCache) const
{
  if ( updateCache )
    {
      size_t cacheSize(m_dataWords*m_dataBlocks);
      m_dataList.resize(cacheSize,EventShapeConstants::invalidValue());
    }
  // update 
  if ( m_dataWords > 0 ) m_dataBlocks = m_dataList.size()/m_dataWords;
  // construct feature structure
  this->buildFeatureStructure();
}

unsigned int EventFeature::buildFeatureStructure() const
{
  m_featureStructure = 
    (m_dataWords&(unsigned int)NDATAWORDS) |
    ((m_dataBlocks&(unsigned int)NDATABLOCKS)<<(unsigned int)BITSHIFT);
  return m_featureStructure;
}

bool EventFeature::indexRange(size_t startIdx,index_t& idx) const
{
  this->setFeatureStructure(false);
  size_t locIdx(startIdx*m_dataWords+m_dataBlocks);
  if ( locIdx > m_dataList.size() ) return false;
  idx.first  = startIdx;
  idx.second = locIdx; 
  return true;
}
