/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ParticleBase.cxx 
// Implementation file for class ParticleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


/// STL includes

#include "VxVertex/RecVertex.h"

/// ParticleEvent includes
#include "ParticleEvent/ParticleBase.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
ParticleBase::ParticleBase() :
  INavigable          (  ),
  I4Momentum          (  ),
  INavigable4Momentum (  ),
  IParticle           (  ),
  m_origin(),
  m_charge( 0. ),
  m_hasCharge  ( false ),
  m_pdgId      ( PDG::null ),
  m_hasPdgId   ( false ),
  m_dataType   ( ParticleDataType::Data )
{}

ParticleBase::ParticleBase( const ParticleBase& rhs ) :
  IAthenaBarCode      ( rhs ),
  INavigable          ( rhs ),
  I4Momentum          ( rhs ),
  INavigable4Momentum ( rhs ),
  IParticle           ( rhs ),
  m_origin    ( rhs.m_origin ),
  m_charge    ( rhs.m_charge ),
  m_hasCharge ( rhs.m_hasCharge ),
  m_pdgId     ( rhs.m_pdgId ),
  m_hasPdgId  ( rhs.m_hasPdgId ),
  m_dataType  ( rhs.m_dataType )
{}

ParticleBase& ParticleBase::operator=( const ParticleBase& rhs )
{
  if ( this != &rhs ) {
    INavigable::operator=( rhs );
    I4Momentum::operator=( rhs );
    INavigable4Momentum::operator=( rhs );
    IParticle::operator=( rhs );
    m_origin    = rhs.m_origin;
    m_charge    = rhs.m_charge;
    m_hasCharge = rhs.m_hasCharge;
    m_pdgId     = rhs.m_pdgId;
    m_hasPdgId  = rhs.m_hasPdgId;
    m_dataType  = rhs.m_dataType;
  }
  return *this;
}

/// Destructor
///////////////
ParticleBase::~ParticleBase() {}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////
const Trk::RecVertex* ParticleBase::origin() const
{
  // check if the link is empty
  if (!m_origin.isValid()) return 0;
 return &(**m_origin).recVertex();
}


/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 
