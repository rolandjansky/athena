///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowClone.h
// @author: Karsten Koeneke - February 2013
///////////////////////////////////////////////////////////////////
#ifndef PARTICLEEVENT_PARTICLESHALLOWCLONE_H
#define PARTICLEEVENT_PARTICLESHALLOWCLONE_H 1

#include "AthLinks/ElementLink.h"
#include "EventKernel/IParticle.h"
#include "NavFourMom/IParticleContainer.h"
#include "FourMom/P4PxPyPzE.h"

// VxVertex includes
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"


/* OPEN QUESTIONS:
   I could template this class with two template arguemnts:
   One being the type of 4Momentum (P4PxPyPzE, P4PtEtaPhiM,...)
   and the other being the type of the master particle (egamma, Muon, TauJet,...)
   I could then implement the specific methods for each particle as well,
   i.e., forward them to the original master. And the underlying 4Momentum
   implementation would be identical to the one of the master.
   However, I could still not dynamic_cast to the type of the master particle
   nor could I use the containers of this and the master interchangably.
   So it may not be worth the effort? Too little benefit?
*/



// Typedefs
typedef IParticle Master_t;
typedef IParticleContainer MasterContainer_t;
typedef ElementLink<MasterContainer_t> MasterLink_t;
typedef P4PxPyPzE P4Momentum_t;

class ParticleShallowCloneCnv_p1;

namespace Analysis
{

  class ParticleShallowClone : public P4Momentum_t,
                               virtual public IParticle
  {

    // Public typedefs
  public:
    /// publish the type of the base class (ie: 'traits-itize' it)
    typedef P4PxPyPzE momentum_type;


  public:

    /** The default constructor */
    ParticleShallowClone( ) :
      P4Momentum_t ( ),
      m_masterLink ( ),
      m_originLink ( ),
      m_hasCharge  ( false ),
      m_charge     ( 0.0 )
    {
    }


    /** Standard constructor with an ElementLink to the original particle.
        The user must ensure that the ElementLink is vaild, i.e., that the
        particle it is pointing to still exists, before calling the constructor! */
    explicit ParticleShallowClone( const MasterLink_t& theMasterLink ) :
      P4Momentum_t ( *theMasterLink ),
      m_originLink ( (*theMasterLink)->originLink() ),
      m_hasCharge  ( (*theMasterLink)->hasCharge() ),
      m_charge     ( (*theMasterLink)->charge() )
    {
      // Assign the ElementLink to the original master particle,
      // including searching until we don't have a ParticleShallowClone any more
      const ParticleShallowClone* pscPtr = dynamic_cast<const ParticleShallowClone*>(*theMasterLink);
      m_masterLink = pscPtr ? pscPtr->getMasterLink() : theMasterLink;
    }
    

    /** Standard constructor with an ElementLink to the original particle
        and all other parameters explicitly given.
        The user must ensure that the ElementLink is vaild, i.e., that the
        particle it is pointing to still exists, before calling the constructor! */
    ParticleShallowClone( const MasterLink_t& theMasterLink,
                          const I4Momentum* the4Mom,
                          const ChargeType theCharge = std::numeric_limits<ChargeType>::quiet_NaN(),
                          const ElementLink<VxContainer>* theOriginLink = 0 ) :
      P4Momentum_t ( the4Mom )
    {
      // Assign the ElementLink to the original master particle,
      // including searching until we don't have a ParticleShallowClone any more
      const ParticleShallowClone* pscPtr = dynamic_cast<const ParticleShallowClone*>(*theMasterLink);
      m_masterLink = pscPtr ? pscPtr->getMasterLink() : theMasterLink;

      // Check if a charge was given. Is so, use it. If not, get the one from the orignal master particle
      if ( std::isnan(theCharge) )
        {
          m_hasCharge = (*theMasterLink)->hasCharge();
          m_charge    = (*theMasterLink)->charge();
        }
      else
        {
          m_hasCharge = true;
          m_charge    = theCharge;
        }

      // Check if an origin was given. Is so, use it. If not, get the one from the orignal master particle
      if ( theOriginLink )
        {
          m_originLink = *theOriginLink;
        }
      else
        {
          m_originLink = (*theMasterLink)->originLink();
        }
    }



    /** Copy constructor */
    // Use default.
    


    /** The default destructor */
    //~ParticleShallowClone( );



    /// All public methods that are new to this class
  public:
    
    /// Get a const ElementLink reference to the original "master" particle
    const MasterLink_t& getMasterLink() const;
    
    /// Get a const pointer to the original "master" particle
    const Master_t* getMasterPtr() const;
    
    


    // All public methods that come from IAthenaBarCode
    // QUESTION: Should the ParticleShallowClone own a new barcode? For now, no.
  public:

    /// Public getter
    virtual AthenaBarCode_t getAthenaBarCode() const;
 
    /// Comparing & Versioning Control
    virtual bool hasSameAthenaBarCode(const IAthenaBarCode &/*obj*/) const;
 
    /// Comparing & Versioning Control
    virtual bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &/*obj*/) const;
 
    /// Comparing & Versioning Control
    virtual AthenaBarCodeVersion_t getVersion() const;

    /* These setters are not implemented! Users should get the master particle and modify its barcode directly...
    /// Public setter
    virtual void setAthenaBarCode(AthenaBarCode_t id);
 
    /// Comparing & Versioning Control
    virtual void newVersion();
 
    /// Comparing & Versioning Control
    virtual void setVersion(AthenaBarCodeVersion_t newversion);
    */



    // All public methods that come from INavigable
  public:

    /// Implement method from INavigable (forwareded to the orignal "master" particle)
    virtual void fillToken(INavigationToken&) const;

    /// Implement method from INavigable (forwareded to the orignal "master" particle)
    virtual void fillToken(INavigationToken&, const boost::any&) const;

    

    // All public methods that come from IParticle
  public:

    /** Return enum indicating real data, fast, or full simulation
        Return Type has a DataType enum with the following values:
        {Data = 0, Full = 1, FastShower = 2, Fast = 3, True = 4}
        (forwareded to the orignal "master" particle) */
    virtual ParticleDataType::DataType dataType() const;

    /** Return a RecVertex corresponding to particle's Origin
        (used directly from this shallow clone particle) */
    virtual const Trk::RecVertex* origin() const;

    /** Return an ElementLink corresponding to particle's Origin
        (used directly from this shallow clone particle) */
    virtual const ElementLink<VxContainer>& originLink() const ; // FIXME: WHY NOT RETURN A REFERENCE???

    /** method to check if charge information is available
        (used directly from this shallow clone particle) */
    virtual bool hasCharge() const;

    /** returns charge as a typedef ChargeType
        currently Charge Type is a double for jets
        this may be changed to an int and ask jets to extend this interface
        (used directly from this shallow clone particle) */
    virtual ChargeType charge() const;

    /** method to check if particle id information is available
        (forwareded to the orignal "master" particle) */
    virtual bool hasPdgId() const;

    /** Return enum indicating particle id
        the enum file is available in Event/EventKernel/PdtPdg.h
        (forwareded to the orignal "master" particle)  */
    virtual PDG::pidType pdgId() const;


    // Set functions (for IParticle)

    /// Set method to set the charge of this shallow clone particle (the original "master" particle remains unaffected)
    virtual void set_charge( ChargeType theCharge );

    /// Set method to set the charge of this shallow clone particle (the original "master" particle remains unaffected)
    virtual void set_origin( const VxContainer* theContainer, int index );

    /// Set method to set the charge of this shallow clone particle (the original "master" particle remains unaffected)
    virtual void set_origin( const VxContainer* theContainer,
                             const Trk::VxCandidate * vertex );

    /* These setters are not implemented! 
       Users should get the master particle and modify its PDG_ID or ParticleDataType directly... */
    //virtual void set_dataType( ParticleDataType::DataType x );
    //virtual void set_pdgId( PDG::pidType x );
    


    /// access to underlying base type (I4Momentum-like)
    const momentum_type& momentumBase() const;



  public:
    /* Need to implement this as the union of all others
    virtual std::ostream& dump(std::ostream&) const; // FIXME: Need to actually re-implement it here for full correctness!
    */    



    // All private data memebers that this class needs on top of the ones inherited
  private:
    friend class ::ParticleShallowCloneCnv_p1;
    
    /// The persistent pointer (ElementLink) to the original "master" particle
    MasterLink_t m_masterLink;
    
    /// Persistent pointer (ElementLink) to the origin of this particle
    ElementLink< VxContainer > m_originLink;

    /// Switch to tell if this particle has a charge
    bool m_hasCharge;

    /** Value of the charge of the particle (this has real meaning, only
     *  if hasCharge() returns true)
     */
    ChargeType m_charge;


  };

} // End of namespace Analysis


/////////////////////////////////////////////////////////////////// 
// Inline methods: 
///////////////////////////////////////////////////////////////////
inline
const MasterLink_t& Analysis::ParticleShallowClone::getMasterLink() const
{
  return m_masterLink;
}


inline
const Master_t* Analysis::ParticleShallowClone::getMasterPtr() const
{
  if ( m_masterLink.isValid() )
    {
      return *m_masterLink;
    }
  else
    {
      //ATH_MSG_ERROR ( "You tried to implicitly cast from an ParticleShallowClone to a INavigable4Momentum, but the pointed-at INavigable4Momentum doesn't exist! Returning NULL..." );
      return 0;
    }
}


// Implementations for methods comming from IAthenaBarCode
inline 
AthenaBarCode_t Analysis::ParticleShallowClone::getAthenaBarCode() const
{
  return (*m_masterLink)->getAthenaBarCode();
}

inline 
bool Analysis::ParticleShallowClone::hasSameAthenaBarCode(const IAthenaBarCode &obj) const
{
  return (*m_masterLink)->hasSameAthenaBarCode(obj);
}
 
inline 
bool Analysis::ParticleShallowClone::hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &obj) const
{
  return (*m_masterLink)->hasSameAthenaBarCodeExceptVersion(obj);
}
 
inline 
AthenaBarCodeVersion_t Analysis::ParticleShallowClone::getVersion() const
{
  return (*m_masterLink)->getVersion();
}


// Implementations for methods comming from INavigable
inline 
void Analysis::ParticleShallowClone::fillToken(INavigationToken& theToken) const
{
  (*m_masterLink)->fillToken(theToken);
  return;
}

inline 
void Analysis::ParticleShallowClone::fillToken(INavigationToken& theToken, const boost::any& theOther) const
{
  (*m_masterLink)->fillToken(theToken, theOther);
  return;
}


// Implementations for methods comming from IParticle
inline 
ParticleDataType::DataType Analysis::ParticleShallowClone::dataType() const
{
  return (*m_masterLink)->dataType();
}

inline 
const Trk::RecVertex* Analysis::ParticleShallowClone::origin() const
{
  return (!m_originLink.isValid()) 
    ? 0 
    : &(**m_originLink).recVertex();
}

inline 
const ElementLink<VxContainer>& Analysis::ParticleShallowClone::originLink() const
{
  return m_originLink;
}

inline 
bool Analysis::ParticleShallowClone::hasCharge() const
{
  return m_hasCharge;
}

inline 
ChargeType Analysis::ParticleShallowClone::charge() const
{
  return m_charge;
}

inline 
bool Analysis::ParticleShallowClone::hasPdgId() const
{
  return (*m_masterLink)->hasPdgId();
}

inline
PDG::pidType Analysis::ParticleShallowClone::pdgId() const
{
  return (*m_masterLink)->pdgId();
}

inline
void Analysis::ParticleShallowClone::set_charge( ChargeType theCharge )
{
  m_charge = theCharge;
  m_hasCharge = true;
  return;
}

inline
void Analysis::ParticleShallowClone::set_origin( const VxContainer* theContainer, int index )
{
  m_originLink = ElementLink< VxContainer >(*theContainer, index);
  return;
}

inline
void Analysis::ParticleShallowClone::set_origin( const VxContainer* theContainer,
                                                 const Trk::VxCandidate * vertex )
{
 m_originLink = ElementLink< VxContainer >
   ( const_cast<Trk::VxCandidate*> (vertex),
     *theContainer );
 return;
}

inline
const Analysis::ParticleShallowClone::momentum_type& Analysis::ParticleShallowClone::momentumBase() const
{
  return *this;
}


/*
inline 
std::ostream& Analysis::ParticleShallowClone::dump(std::ostream& val) const
{
  return (*m_masterLink)->dump(val);
}
*/

#endif // PARTICLEEVENT_PARTICLESHALLOWCLONE_H
