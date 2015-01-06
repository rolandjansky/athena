/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetEvent/JetMomentMap.h"


JetMomentMap::JetMomentMap() : base_t()
{ 
}


JetMomentMap::~JetMomentMap()
{ 
}

bool 
JetMomentMap::setMoment(size_t jetIndex,size_t keyIndex,float_t moment)
{
  data_t ds(moment,true);
  this->addData(jetIndex,keyIndex,ds);
  return true;
}

bool 
JetMomentMap::setMoment(size_t jetIndex,size_t keyIndex,float_t moment) const
{
  data_t ds(moment,true);
  this->addData(jetIndex,keyIndex,ds);
  return true;
}

bool 
JetMomentMap::getMoment(size_t jetIndex,size_t keyIndex,float_t& moment) const
{
  data_t ds(JetMoment::invalidMoment(),false);
  if ( !this->retrieveData(jetIndex,keyIndex,ds) ) return false;
  // successfully retrieved
  moment = ds.moment();
  return ds.tag();
}

JetMomentMap::float_t 
JetMomentMap::getMoment(size_t jetIndex,size_t keyIndex) const
{
  data_t ds(JetMoment::invalidMoment(),false);
  return this->retrieveData(jetIndex,keyIndex,ds) && ds.tag()
    ? ds.moment() : JetMoment::invalidMoment(); 
}

OrphanJetMomentMap OrphanJetMomentMap::instance;
