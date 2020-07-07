/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// standard library includes:
#include <iostream>
#include <stdexcept>
#include <sstream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODCaloRings/versions/CaloRings_v1.h"
#include "xAODCaloRings/RingSetContainer.h"

namespace xAOD {

static const SG::AuxElement::Accessor< RingSetLinks >
  accRingSetLinks( "ringSetLinks" );
static const SG::AuxElement::ConstAccessor< RingSetLinks >
  constAccRingSetLinks( "ringSetLinks" );

/// @name RingSet Collection direct interation methods:
/// @{
//==============================================================================
AUXSTORE_OBJECT_SETTER_AND_GETTER(CaloRings_v1,
    RingSetLinks,
    ringSetLinks,
    setRingSetLinks)

//==============================================================================
unsigned CaloRings_v1::nRingSets() const 
{
  return (constAccRingSetLinks.isAvailable( *this ) )?
              (constAccRingSetLinks( *this ).size()):
              (0);
}


//==============================================================================
void CaloRings_v1::addRingSetEL(const ElementLink<RingSetContainer_v1> &rsEL)
{
  accRingSetLinks( *this ).push_back(rsEL);
}
//==============================================================================
void CaloRings_v1::clear()
{
  accRingSetLinks( *this ).clear();
}

//==============================================================================
RingSetLinks::iterator CaloRings_v1::begin()
{
  return (accRingSetLinks.isAvailable( *this ) )?
              (accRingSetLinks( *this ).begin()):
              (RingSetLinks::iterator());
}

//==============================================================================
RingSetLinks::iterator CaloRings_v1::end()
{
  return (accRingSetLinks.isAvailable( *this ) )?
              (accRingSetLinks( *this ).end()):
              (RingSetLinks::iterator());
}

//==============================================================================
RingSetLinks::const_iterator CaloRings_v1::begin() const
{
  return (constAccRingSetLinks.isAvailable( *this ) )?
              (constAccRingSetLinks( *this ).begin()):
              (RingSetLinks::const_iterator());
}

//==============================================================================
RingSetLinks::const_iterator CaloRings_v1::end() const
{
  return (constAccRingSetLinks.isAvailable( *this ) )?
              (constAccRingSetLinks( *this ).end()):
              (RingSetLinks::const_iterator());
}

//==============================================================================
RingSet *CaloRings_v1::at(const unsigned int i)
{
  if ( i > nRingSets() ) 
    return 0;
  ElementLink<RingSetContainer_v1> &rsEL = accRingSetLinks( *this ).at(i);
  if ( !rsEL.isValid() ) {
    return 0;
  }
  // If we want to use it as an left operand, we must do this:
  return const_cast<RingSet*>(*rsEL);
}

//==============================================================================
RingSet *CaloRings_v1::operator[](const unsigned int i)
{
  if ( i > nRingSets() ) 
    return 0;
  ElementLink<RingSetContainer_v1> &rsEL = accRingSetLinks( *this )[i];
  if ( !rsEL.isValid() ) {
    return 0;
  }
  // If we want to use it as an left operand, we must do this:
  return const_cast<RingSet*>(*rsEL);
}

//==============================================================================
const RingSet *CaloRings_v1::at(const unsigned int i) const
{
  if ( i > nRingSets() ) 
    return 0;
  const ElementLink<RingSetContainer_v1> &rsEL = 
    constAccRingSetLinks( *this ).at(i);
  if ( !rsEL.isValid() ) {
    return 0;
  }
  return *rsEL;
}

//==============================================================================
const RingSet *CaloRings_v1::operator[](const unsigned int i) const
{
  if ( i > nRingSets() ) 
    return 0;
  const ElementLink<RingSetContainer_v1> &rsEL = 
    constAccRingSetLinks( *this )[i];
  if ( !rsEL.isValid() ) {
    return 0;
  }
  return *rsEL;
}
/// @}

/// Navigate through eT rings methods
/// @{
//==============================================================================
float CaloRings_v1::ringAt(const unsigned rsIdx, const unsigned ringIdx) const 
{
  const RingSet *rs = this->at(rsIdx);
  if (!rs) {
    throw std::runtime_error("The element link is invalid.");
  }
  return rs->at(ringIdx);
}

//==============================================================================
float CaloRings_v1::ringAt(const unsigned ringIdx) const 
{
  unsigned ringStripPartialSize = 0;
  for (unsigned rsELIdx = 0; rsELIdx < this->nRingSets(); ++rsELIdx){
    const RingSet *rs = this->at(rsELIdx);
    if ( !rs ) {
      throw std::runtime_error("There is an invalid element link.");
    }
    unsigned cRsSize = rs->size();
    if ( ringStripPartialSize + cRsSize > ringIdx ) {
      // Found the RingSet that contains the nth ring, return it using the
      // amount that lacks from the previous RingSet to give the nth Ring:
      return rs->at( ringIdx - ( ringStripPartialSize ) );
    }
    ringStripPartialSize += cRsSize;
  } 
  // If we reached here, then the requested size is greater than the total ring
  // strip
  std::stringstream ss;
  ss << "Requested ring at index: \"" 
    << ringIdx << "\", but total rings size is: " 
    << ringStripPartialSize;
  throw std::overflow_error( ss.str() );
}
/// @}

/// Export rings eT in vectorized representation
/// @{
//==============================================================================
void CaloRings_v1::exportRingsTo(
    std::vector<float> &ringStrip,
    const unsigned rsIdxStart, 
    const unsigned rsIdxEnd) const 
{

  // Check if arguments are within range:
  checkRingSetIndexWithinRange(rsIdxStart);
  checkRingSetIndexWithinRange(rsIdxEnd);

  // Prepare RingStrip vector to receive ring strip information:
  ringStrip.clear();

  for ( unsigned idx = rsIdxStart; idx <= rsIdxEnd ; ++idx ) {
    const RingSet *rs = this->at( idx );
    if ( !rs ) {
      throw std::runtime_error("Found invalid ElementLink");
    }
    rs->copyTo(ringStrip);
  }
}

//==============================================================================
void CaloRings_v1::exportRingsTo(
    std::vector<float> &ringStrip,
    const RingSetConf_v1::RawConfCollection &clRingsRawConfCol,
    const Ringer::CalJointLayer layerTypeStart, 
    const Ringer::CalJointLayer layerTypeEnd) const 
{

  if ( layerTypeEnd < layerTypeStart ) {
    throw std::invalid_argument( std::string(
          "Input layerTypeStart greater than layerTypeEnd") );
  }

  unsigned nRingSets = this->nRingSets();

  if ( nRingSets && clRingsRawConfCol.size() != nRingSets ) {
    throw std::invalid_argument( std::string(
          "The configuration struct seems not to be valid."));
  }

  // Prepare RingStrip vector to receive Rings energy vectorized representation
  ringStrip.clear();

  bool foundStartLayer = false;

  for ( unsigned rsIdx = 0; rsIdx < nRingSets; ++rsIdx ) {
    const RingSet* rs = this->at(rsIdx);
    if ( !rs ) {
      throw std::runtime_error( std::string(
          "Found invalid ElementLink") );
    }
    if ( !foundStartLayer ) {
      // Check if we found start layer:
      if ( clRingsRawConfCol[rsIdx].calJointLayer == layerTypeStart ) 
      {
        foundStartLayer = true;
        rs->copyTo(ringStrip);
      }
    } else {
      rs->copyTo(ringStrip);
    }
    // Check if we are at the last requested layer:
    if ( clRingsRawConfCol[rsIdx].calJointLayer == layerTypeEnd ) {
      break;
    }
  }
}

//==============================================================================
void CaloRings_v1::exportRingsTo(
    std::vector<float> &ringStrip,
    const RingSetConf_v1::RawConfCollection &clRingsRawConfCol,
    const Ringer::CalJointSection sectionType) const 
{

  unsigned nRingSets = this->nRingSets();

  if ( nRingSets && clRingsRawConfCol.size() != nRingSets ) {
    throw std::invalid_argument( std::string(
          "The configuration struct seems not to be valid."));
  }

  // Prepare RingStrip vector to receive Rings energy vectorized representation
  ringStrip.clear();

  bool foundSection = false;

  for ( unsigned rsIdx = 0; rsIdx < nRingSets; ++rsIdx) {
    if ( clRingsRawConfCol[rsIdx].calJointSection == sectionType) {
      foundSection = true;
      const RingSet* rs = this->at(rsIdx);
      if ( !rs ) {
        throw std::runtime_error( std::string(
            "Found invalid ElementLink") );
      }
      rs->copyTo(ringStrip);
    } else if (foundSection) {
      break;
    }
  }

}
/// @}

/// @name CaloRings_v1 operators overload:
/// @{
//==============================================================================
CaloRings_v1& CaloRings_v1::operator=(const CaloRings_v1& cl_rings )
{
  if (this != &cl_rings){ // protect against invalid self-assignment
    if (!this->container() && !this->hasStore() ) {
      makePrivateStore();
    }
    this->SG::AuxElement::operator=( cl_rings );
  }
  // by convention, always return *this
  return *this;
}
/// @}


/// Print-out methods:
/// @{

//==============================================================================
void CaloRings_v1::print( std::ostream &stream ) const 
{
  stream << "CaloRings are : " << std::endl;
  for (unsigned rsIdx = 0; rsIdx < this->nRingSets(); ++rsIdx) {
    stream << "Ringset #" << rsIdx << " : "; 
    const RingSet* rs = this->at(rsIdx);
    if ( !rs ) {
      throw std::runtime_error( std::string(
          "Found invalid ElementLink") );
    }
    rs->print(stream);
  }
}
/// @}

/// Utilities methods:
/// @{
//==============================================================================
void CaloRings_v1::checkRingSetIndexWithinRange( unsigned index ) const 
{
  if ( index >= this->nRingSets() ) {
    throw std::overflow_error("Out of RingSet ElementLink vector range.");
  }
}
/// @}
//
} // End of namespace xAOD
