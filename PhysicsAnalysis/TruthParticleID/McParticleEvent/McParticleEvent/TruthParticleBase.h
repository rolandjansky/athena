///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleBase.h 
// Header file for class TruthParticleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHPARTICLEBASE_H 
#define MCPARTICLEEVENT_TRUTHPARTICLEBASE_H 

// STL includes

// HepMC / CLHEP includes
#include "AtlasHepMC/GenParticle.h"

// Gaudi includes

// EventKernel includes
#include "EventKernel/PdtPdg.h"
#include "EventKernel/IParticle.h" // for {Charge,ParticleDataType::Data}Type

// Navigation includes
#include "Navigation/AthenaBarCodeImpl.h"
#include "AthLinks/ElementLink.h"
#include "VxVertex/VxContainer.h"

// Forward declaration
namespace Trk   { class RecVertex; }
namespace Trk   { class VxCandidate;}

class TruthParticleBase
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleBase();

  /** Copy constructor: 
   */
  TruthParticleBase( const TruthParticleBase& rhs );

  /** Assignment operator: 
   */
  TruthParticleBase& operator=( const TruthParticleBase& rhs ); 

  /** Constructor with parameters: 
   */
  TruthParticleBase( const HepMC::GenParticlePtr mc );

  /** Destructor: 
   */
  ~TruthParticleBase(); 

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

  /** @}
   */

  /// return the @c HepMC::GenParticle which is being proxied
  HepMC::ConstGenParticlePtr genParticle() const;

  /// return the (atlas) unique identifier
  const AthenaBarCodeImpl& getAthenaBarCodeImpl() const {return m_abc;}

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// Set the charge of this TruthParticleBase
  void setCharge( const ChargeType charge );
  
  /// Fill the data members of ParticleBase from the GenParticle
  void setGenParticle( HepMC::ConstGenParticlePtr particle );

  /// return the (atlas) unique identifier
  AthenaBarCodeImpl& getAthenaBarCodeImpl() {return m_abc;}

  // Set functions (for IParticle) (all dummy)
  void set_dataType( ParticleDataType::DataType /*x*/ ) {std::abort();}
  void set_charge( ChargeType x ) {setCharge(x);}
  void set_pdgId( PDG::pidType /*x*/ ) {std::abort();}
  void set_origin( const VxContainer* /*theContainer*/, int /*index*/ )
  {std::abort();}
  void set_origin( const VxContainer* /*theContainer*/,
                   const Trk::VxCandidate * /*vertex*/ )
  {std::abort();}

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// The charge of this TruthParticle
  ChargeType m_charge;

  /// The @c HepMC::GenParticle we are proxying
  HepMC::ConstGenParticlePtr m_genParticle;

  /// The @c AthenaBarCodeImpl held to provide unique identification
  /// throughout the whole Atlas EDM
  AthenaBarCodeImpl m_abc;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
ParticleDataType::DataType TruthParticleBase::dataType() const
{
  return ParticleDataType::True;
}

inline 
bool TruthParticleBase::hasCharge() const
{
  return true;
}

inline 
ChargeType TruthParticleBase::charge() const
{
  return m_charge;
}
      
inline 
bool TruthParticleBase::hasPdgId() const
{
  return true;
}

inline 
PDG::pidType TruthParticleBase::pdgId() const
{
  return static_cast<PDG::pidType>(m_genParticle->pdg_id());
}

inline 
HepMC::ConstGenParticlePtr TruthParticleBase::genParticle() const
{
  return m_genParticle;
}

inline 
void TruthParticleBase::setCharge( const ChargeType charge )
{
  m_charge = charge;
}

inline 
void TruthParticleBase::setGenParticle(  HepMC::ConstGenParticlePtr mc )
{
  m_genParticle = mc;
}

#endif //> MCPARTICLEEVENT_TRUTHPARTICLEBASE_H
