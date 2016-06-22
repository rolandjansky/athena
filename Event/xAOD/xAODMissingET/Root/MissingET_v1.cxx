/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingET_v1.h"
#include "xAODMissingET/versions/MissingETBase.h"

using namespace xAOD;

MissingET_v1::MissingET_v1( bool createStore )
  : SG::AuxElement() {

   if( createStore ) {
      createPrivateStore();
   }
}

MissingET_v1::MissingET_v1( const std::string& name,
                            MissingETBase::Types::bitmask_t src )
   : SG::AuxElement() {

   createPrivateStore();
   setName( name );
   setSource( src );

}

MissingET_v1::MissingET_v1( double mpx, double mpy, double sumet,
                            const std::string& name,
                            MissingETBase::Types::bitmask_t src )
   : SG::AuxElement() {

   createPrivateStore();
   setName( name );
   setSource( src );
   this->f_mpx() = mpx;
   this->f_mpy() = mpy;
   this->f_sumet() = sumet;
}

MissingET_v1::MissingET_v1( const IParticle* particle, const std::string& name,
                            MissingETBase::Types::bitmask_t src )
   : SG::AuxElement() {

   createPrivateStore();
   setName( name );
   setSource( src );
   add( particle->p4().Px(), particle->p4().Py(), particle->pt() );
}

// CHECK use makePrivateStore to control that behaviour ??
MissingET_v1::MissingET_v1(const MissingET_v1& met)
  : SG::AuxElement()
{ this->makePrivateStore(&met); }

MissingET_v1::~MissingET_v1()
{ }

////////// ----
// Note: the add(...) methods are virtual and should therefore not be inlined.
////////// ----

void MissingET_v1::add(const IParticle* particle)
{
  // retrieve kinematics
  EXTRACT_PX( double, px, (*particle) );
  EXTRACT_PY( double, py, (*particle) );
  // add
  this->add(px,py,particle->pt());
}

void MissingET_v1::add(const IParticle* particle,double scale)
{
  // retrieve kinematics
  EXTRACT_SCALED_PX( double, px, (*particle), scale);
  EXTRACT_SCALED_PY( double ,py, (*particle), scale);
  double pt(particle->pt()*scale);
  // add
  this->add(px,py,pt);
}

void xAOD::MissingET_v1::add(double px,double py, double pt)
{ this->f_mpx() -= px; this->f_mpy() -= py; this->f_sumet() += pt; }

MissingET_v1& MissingET_v1::operator=(const MissingET_v1& met)
{
  setName( met.name() );
  setSource( met.source() );
  this->f_mpx() = met.mpx();
  this->f_mpy() = met.mpy();
  this->f_sumet() = met.sumet();
  return *this;
}

MissingET_v1& MissingET_v1::operator-=(const IParticle* particle)
{
  EXTRACT_PX( double, px, (*particle) );
  EXTRACT_PY( double, py, (*particle) );
  this->add(-px,-py,-particle->pt());
  return *this;
}

/// This function is added because in standalone mode we can't use C++11. So we
/// can't rely on constructors calling each other. Which means that the same
/// functionality is instead now implemented in this old style.
///
void MissingET_v1::createPrivateStore() {

   makePrivateStore();
   setName( MissingETBase::Strings::unknownString() );
   setSource( MissingETBase::Source::unknown() );
   clear();
}
