/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness.h"
#include "MissingETGoodness/EtmissGoodnessConfig.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "TMath.h"

ClassImp(MET::EtmissGoodness)


MET::EtmissGoodness::EtmissGoodness()
  : TObject()
  , m_manager(MET::EtmissGoodnessManager::instance())
  , m_goodies(MET::Goodies::instance())
  , m_dummyd(-999.)
{
}


MET::EtmissGoodness::~EtmissGoodness()
{
}


MET::EtmissGoodness::EtmissGoodness(const MET::EtmissGoodness& other)
  : TObject(other)
  , m_manager(other.m_manager)
  , m_goodies(MET::Goodies::instance())
  , m_dummyd(other.m_dummyd)
{
}


MET::EtmissGoodness&
MET::EtmissGoodness::operator=(const MET::EtmissGoodness& other)
{
  if (&other==this) {
    return *this ;
  } 
  m_dummyd = other.m_dummyd;

  return *this ;
}


Bool_t 
MET::EtmissGoodness::IsGood( const TString& qualifier, const Quality& quality, Bool_t doLookupQualifier ) const
{
  return this->IsGood(quality,qualifier,doLookupQualifier);
}


Bool_t 
MET::EtmissGoodness::IsGood( const Quality& quality, const TString& qualifier, Bool_t doLookupQualifier ) const
{
  std::vector<TString> parList;
  if (doLookupQualifier) { 
    // assume qualifier describes a list of parameter names in EtmissGoodnessConfig
    parList = MET::GetEtmissGoodnessConfig(qualifier); 
  } else {
    // assume qualifier is a parameter name
    parList.push_back(qualifier);
  }
  return m_manager.Evaluate(quality,parList);
}


Long64_t
MET::EtmissGoodness::GetBitMask( const Quality& quality ) const
{
  return m_manager.GetEventsBitMask(quality);
}


void
MET::EtmissGoodness::Summary() const 
{
  MET::TMsgLogger logger("EtmissGoodness");
  logger << kINFO 
         << "Using Etmiss Goodness version = " << m_manager.GetVersion() 
         << " and quality = " << m_manager.GetQuality() 
         << GEndl;
  logger << kINFO << "isGood  = " << this->IsGood() << GEndl;
  logger << kINFO << "bitmask = " << this->GetBitMask() << GEndl;
  logger << kINFO << "etx     = " << this->etx() << GEndl;
  logger << kINFO << "ety     = " << this->ety() << GEndl;
  logger << kINFO << "et      = " << this->et() << GEndl;
  logger << kINFO << "sumet   = " << this->sumet() << GEndl;
  logger << kINFO << "phi     = " << this->phi() << GEndl;
}


// retrieve kinematics
double 
MET::EtmissGoodness::etx() const
{ 
  TString name = TString(m_manager.GetMETKey()) + "_etx";
  return m_goodies.getDoubleValue(name);
}


double 
MET::EtmissGoodness::ety() const
{ 
  TString name = TString(m_manager.GetMETKey()) + "_ety";
  return m_goodies.getDoubleValue(name);
}


double 
MET::EtmissGoodness::sumet() const
{ 
  TString name = TString(m_manager.GetMETKey()) + "_sumet";
  return m_goodies.getDoubleValue(name);
}


double 
MET::EtmissGoodness::et() const
{
  return TMath::Sqrt( this->etx()*this->etx() + this->ety()*this->ety() );
}


double 
MET::EtmissGoodness::phi() const
{
  return TMath::ATan2( this->ety(), this->etx() );
}

