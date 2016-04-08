/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSet_v1.cxx 708163 2015-11-16 16:03:55Z ssnyder $ 
#include "xAODCaloRings/versions/RingSet_v1.h"
#include "xAODCaloRings/tools/PrintHelperFcns.h"

namespace xAOD {

namespace {
// Instantiate the needed accessors:
SG::AuxElement::Accessor< std::vector<float> > accRingsE("ringsE");
SG::AuxElement::ConstAccessor< std::vector<float> > constAccRingsE("ringsE");
} // private namespace

// @name RingSet_v1 accessors:
//==============================================================================
float &RingSet_v1::at(const unsigned int i) { 
  return accRingsE( *this ).at(i); 
}

//==============================================================================
float &RingSet_v1::operator [](const unsigned int i) { 
  return accRingsE( *this )[i]; 
}

float RingSet_v1::at(const unsigned int i) const { 
  return constAccRingsE( *this ).at(i); 
}

//==============================================================================
float RingSet_v1::operator [](const unsigned int i) const { 
  return constAccRingsE( *this )[i]; 
}

//==============================================================================
AUXSTORE_OBJECT_SETTER_AND_GETTER(RingSet_v1,
    std::vector<float>,
    ringsE,
    setRingsE)

// @name RingSet_v1 rings interation:
//==============================================================================
unsigned RingSet_v1::size() const { 
  return (constAccRingsE.isAvailable( *this ) )?
    constAccRingsE( *this ).size() : 0;
}

//==============================================================================
void RingSet_v1::clear() { 
  return accRingsE( *this ).clear(); 
}

//==============================================================================
void RingSet_v1::addRing(float r) { 
  return accRingsE( *this ).push_back(r); 
}

//==============================================================================
RingSet_v1::iterator RingSet_v1::begin() {
  return (accRingsE.isAvailable( *this ) )?
    accRingsE( *this ).begin() : RingSet_v1::iterator();
}

//==============================================================================
RingSet_v1::iterator RingSet_v1::end() {
  return (accRingsE.isAvailable( *this ) )?
    accRingsE( *this ).end() : RingSet_v1::iterator();
}

//==============================================================================
RingSet_v1::const_iterator RingSet_v1::begin() const {
  return (constAccRingsE.isAvailable( *this ) )?
    constAccRingsE( *this ).begin() : RingSet_v1::const_iterator();
}

//==============================================================================
RingSet_v1::const_iterator RingSet_v1::end() const {
  return (constAccRingsE.isAvailable( *this ) )?
    constAccRingsE( *this ).end() : RingSet_v1::const_iterator();
}

//==============================================================================
void RingSet_v1::copyTo(std::vector<float> &vec) const {
  const std::vector<float> &ringsE = constAccRingsE( *this );
  vec.insert(vec.end(), ringsE.begin(), ringsE.end());
}


//==============================================================================
void RingSet_v1::print( MsgStream &stream, MSG::Level level ) const {
  const std::vector<float> &ringsE = constAccRingsE( *this );
  if( stream.level() <= level ) {
    stream << ringsE << endreq;
  }
}

//==============================================================================
void RingSet_v1::print( std::ostream &stream ) const {
  const std::vector<float> &ringsE = constAccRingsE( *this );
  stream << ringsE << std::endl;
}

//==============================================================================
RingSet_v1& RingSet_v1::operator=(const RingSet_v1& cl_rings ){         
  if (this != &cl_rings){ // protect against invalid self-assignment
    if (!this->container() && !this->hasStore() ) {
      makePrivateStore();
    }
    this->SG::AuxElement::operator=( cl_rings );
  }
  // by convention, always return *this
  return *this;
}
 
} // End of namespace xAOD
