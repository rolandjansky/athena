/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IParticleFilter.cxx 
// Implementation file for class IParticleFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AnalysisUtils includes
#include "AnalysisUtils/IParticleFilter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
IParticleFilter::IParticleFilter() :
  IFilterCuts(),
  IFilter<IParticle>(),
  MomentumFilter<IParticle>(),
  PdgIdFilter<IParticle>(),
  ChargeFilter<IParticle>(),
  CategoryIdFilter<IParticle>(),
  IIParticleFilter<IParticle>()
{}

IParticleFilter::IParticleFilter( const IParticleFilter& rhs ) :
  IFilterCuts(rhs),
  IFilter<IParticle>(rhs),
  MomentumFilter<IParticle>(rhs),
  PdgIdFilter<IParticle>(rhs),
  ChargeFilter<IParticle>(rhs),
  CategoryIdFilter<IParticle>(rhs),
  IIParticleFilter<IParticle>(rhs)
{}

/// Destructor
///////////////
IParticleFilter::~IParticleFilter() {}

IParticleFilter& IParticleFilter::operator=( const IParticleFilter& rhs )
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    IFilter<IParticle>::operator=(rhs);
    MomentumFilter<IParticle>::operator=(rhs);
    PdgIdFilter<IParticle>::operator=(rhs);
    ChargeFilter<IParticle>::operator=(rhs);
    CategoryIdFilter<IParticle>::operator=(rhs);
    IIParticleFilter<IParticle>::operator=(rhs);
  }
  return *this;
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 
/*
MsgStream& operator<<( MsgStream& msg, const IParticleFilter& obj )
{
  msg << "Pt Min= "  << obj.ptMin() << endreq
      << "PdgIds: "
      << endreq;
  obj.pdgIds()->dropList();
  return msg;
}
*/
