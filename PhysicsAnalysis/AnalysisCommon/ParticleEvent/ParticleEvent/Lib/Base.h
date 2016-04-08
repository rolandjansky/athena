///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base.h 
// Header file for class Base
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_PARTICLEEVENTBASE_H 
#define PARTICLEEVENT_PARTICLEEVENTBASE_H 

// STL includes
#include <iosfwd>

// HepMC / CLHEP includes

// Gaudi includes

// EventKernel includes
#include "EventKernel/PdtPdg.h"
#include "EventKernel/IParticle.h" // for {Charge,ParticleDataType::Data}Type

// VxVertex includes
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"

// AthenaBarCode includes
#include "Navigation/AthenaBarCodeImpl.h"
#include "AthLinks/ElementLink.h"

// Forward declaration

namespace ParticleEvent {

class Base
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor:
   */
  Base();

  /** Copy constructor:
   */
  explicit Base( const Base& rhs );

  /** Assignment operator:
   */
  Base& operator=( const Base& rhs );

  /** Constructor with parameters:
   */

  /** Destructor: 
   */
  virtual ~Base(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @{ IParticle interface-like declaration
   */

  /** Return enum indicating real data, fast, or full simulation
      Return Type has a DataType enum with the following values:
     {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4}  */
  ParticleDataType::DataType dataType() const;

  /** Return a RecVertex corresponding to particle Origin  */
  const Trk::RecVertex* origin() const;

  /** Return the ElementLink to particle Origin  */
  const ElementLink<VxContainer>& originLink() const;

  /** method to check if charge information is available*/
  bool hasCharge() const;

  /** returns charge as a typedef ChargeType
      currently Charge Type is a double for jets
      this may be changed to an int and ask jets to extend this interface  */
  ChargeType charge() const;

  /** method to check if particle id information is available*/
  bool hasPdgId() const;

  /** Return enum indicating particle id
      the enum file is available in Event/EventKernel/PdtPdg.h  */
  PDG::pidType pdgId() const;

  /// Print @c IParticle content
  std::ostream& dump( std::ostream& out ) const;

  const AthenaBarCodeImpl& getAthenaBarCodeImpl() const {return m_abc;}
  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  // Set functions (for IParticle)
  void set_dataType( ParticleDataType::DataType x );
  void set_charge( ChargeType x );
  void set_pdgId( PDG::pidType x );
  void set_origin( const VxContainer* theContainer, int index );
  void set_origin( const VxContainer* theContainer, 
                   const Trk::VxCandidate * vertex );
  void set_origin( const ElementLink<VxContainer>& origin );

  AthenaBarCodeImpl& getAthenaBarCodeImpl() {return m_abc;}
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /** the enum storing the (data) type of this particle
   */
  ParticleDataType::DataType m_dataType;

  /** Persistent pointer to the origin of this particle
   */
  ElementLink< VxContainer > m_origin;

  /** Switch to tell if this particle has a charge
   */
  bool m_hasCharge;

  /** Value of the charge of the particle (this has real meaning, only
   *  if hasCharge() returns true)
   */
  ChargeType m_charge;

  /** Switch to tell if this particle has a PDG Identity
   */
  bool m_hasPdgId;

  /** Value for the PDG identity (this has real meaning, only
   *  if hasPdgId() returns true)
   */
  PDG::pidType m_pdgId;

  /// The AthenaBarCode Part
  AthenaBarCodeImpl m_abc;


}; 

/////////////////////////////////////////////////////////////////// 
// I/O functions
/////////////////////////////////////////////////////////////////// 
std::ostream& operator<<( std::ostream& out, const Base& base );

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
Base::Base() :
  m_dataType ( ),
  m_origin   ( ),
  m_hasCharge( false ),
  m_charge   ( 0. ),
  m_hasPdgId ( false ),
  m_pdgId    ( PDG::null ),
  m_abc      ()
{}

inline
Base::Base( const Base& rhs ) :
  m_dataType ( rhs.m_dataType  ),
  m_origin   ( rhs.m_origin    ),
  m_hasCharge( rhs.m_hasCharge ),
  m_charge   ( rhs.m_charge    ),
  m_hasPdgId ( rhs.m_hasPdgId  ),
  m_pdgId    ( rhs.m_pdgId     ),
  m_abc      ( rhs.m_abc)
{}

inline
Base& Base::operator=( const Base& rhs )
{
  if ( this != &rhs ) {
    m_dataType  = rhs.m_dataType;
    m_origin    = rhs.m_origin;
    m_hasCharge = rhs.m_hasCharge;
    m_charge    = rhs.m_charge;
    m_hasPdgId  = rhs.m_hasPdgId;
    m_pdgId     = rhs.m_pdgId;
    m_abc       = rhs.m_abc;
  }
  return *this;
}

inline
Base::~Base()
{}

inline ParticleDataType::DataType 
Base::dataType() const 
{
  return m_dataType;
}

inline const Trk::RecVertex* 
Base::origin() const
{
  return (!m_origin.isValid()) 
    ? 0 
    : &(**m_origin).recVertex();
}

inline bool 
Base::hasCharge()  const 
{ 
  return m_hasCharge;
}

inline ChargeType 
Base::charge() const 
{ 
  return m_charge;
}

inline bool 
Base::hasPdgId() const 
{
  return m_hasPdgId;
}

inline PDG::pidType 
Base::pdgId() const 
{
  return m_pdgId;
}

// --------------------
// non-const interface
// --------------------

inline 
void 
Base::set_dataType( ParticleDataType::DataType x ) 
{ 
  m_dataType = x;
}

inline 
void 
Base::set_charge( ChargeType x ) 
{ 
  m_charge = x; 
  m_hasCharge = true;
}

inline
void 
Base::set_pdgId( PDG::pidType x ) 
{ 
  m_pdgId = x; 
  m_hasPdgId = true;
}

inline
void 
Base::set_origin( const VxContainer* theContainer, int index )
{
  m_origin = ElementLink< VxContainer >(*theContainer, index);
}

inline
void 
Base::set_origin( const VxContainer* theContainer, 
                  const Trk::VxCandidate * vertex )
{
  m_origin = ElementLink< VxContainer >
    ( const_cast<Trk::VxCandidate*> (vertex),
      *theContainer );
}

inline
void 
Base::set_origin( const ElementLink<VxContainer>& origin )
{
  m_origin = origin;
}

} //> end namespace ParticleEvent

#endif //> PARTICLEEVENT_PARTICLEEVENTBASE_H
