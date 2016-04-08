/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_PARTICLEBASE_H
#define PARTICLEEVENT_PARTICLEBASE_H

#include "EventKernel/IParticle.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"
#include "NavFourMom/IParticleContainer.h"

class ParticleBase : public virtual IParticle
{
 public:
  
  /// Default constructor
  ParticleBase();
  
  /** Copy constructor
   */
  ParticleBase( const ParticleBase& rhs );

  /** Assignement operator
   */
  ParticleBase& operator=( const ParticleBase& rhs );

  /// Destructor
  virtual ~ParticleBase();

  // standard IParticle methods 
  /** Return enum indicating real data, fast, or full simulation
      Return Type has a DataType enum with the following values:
     {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4}  */
  virtual ParticleDataType::DataType dataType()    const;

  /** Return a RecVertex corresponding to particle Origin  */
  virtual const Trk::RecVertex* origin() const ;

  /** Return an ElementLink corresponding to particle's Origin */
  virtual const ElementLink<VxContainer>& originLink () const { return m_origin; }

  /** method to check if charge information is available*/
  virtual bool hasCharge()         const;

  /** returns charge as a typedef ChargeType
      currently Charge Type is a double for jets
      this may be changed to an int and ask jets to extend this interface  */
  virtual ChargeType charge()      const;

  /** method to check if particle id information is available*/
  virtual bool hasPdgId()          const;

  /** Return enum indicating particle id
      the enum file is available in Event/EventKernel/PdtPdg.h  */
  virtual PDG::pidType pdgId()     const;
  
  // Set functions (for IParticle)
  void set_dataType(ParticleDataType::DataType x ) { m_dataType = x;}
  void set_charge(ChargeType x ) { m_charge = x; m_hasCharge = true;}
  void set_pdgId(PDG::pidType x ) { m_pdgId = x; m_hasPdgId = true;}
  void set_origin(const VxContainer* theContainer, int index);
  void set_origin(const VxContainer* theContainer, const Trk::VxCandidate * vertex );

 private:

  ElementLink< VxContainer > m_origin;
  ChargeType m_charge;
  bool m_hasCharge;
  PDG::pidType m_pdgId;
  bool m_hasPdgId;
  ParticleDataType::DataType m_dataType;  

};

inline void ParticleBase::set_origin(const VxContainer* const theContainer, int index)
{
  m_origin.toIndexedElement( *theContainer, index) ;
}
inline void ParticleBase::set_origin(const VxContainer* theContainer, const Trk::VxCandidate * vertex )
{
  m_origin.toContainedElement(*theContainer, const_cast<Trk::VxCandidate*> (vertex));
}
inline   ParticleDataType::DataType ParticleBase::dataType() const 
{
  return m_dataType;
}
inline bool ParticleBase::hasCharge()  const { return m_hasCharge;}
inline ChargeType ParticleBase::charge() const { return m_charge ;}
inline bool ParticleBase::hasPdgId() const {return m_hasPdgId ;}
inline PDG::pidType ParticleBase::pdgId() const {return m_pdgId;}

DATAVECTOR_VIRTBASES1 (ParticleBase, IParticle);

#endif // PARTICLEEVENT_PARTICLEBASE_H
