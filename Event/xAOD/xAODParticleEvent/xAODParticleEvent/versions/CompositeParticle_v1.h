// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticle_v1.h 696023 2015-09-21 17:09:08Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLE_V1_H
#define XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLE_V1_H

// STL includes
#include <vector>

// xAOD include(s):
#include "AthContainers/ConstDataVector.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "xAODMissingET/MissingET.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "xAODTruth/TruthParticle.h"

// Local include(s):
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

  /// Description of a composite particle, i.e. a particle
  /// that is composed out of several other particles/objects,
  /// e.g., a Z boson that consists out of two muons.
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revision: 696023 $
  /// $Date: 2015-09-21 19:09:08 +0200 (Mon, 21 Sep 2015) $
  ///
  class CompositeParticle_v1 : public IParticle {

  public:
    /// Default constructor
    CompositeParticle_v1();

    /// @name Functions implementing the xAOD::IParticle interface
    /// @{

    /// The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const;
    /// The invariant mass of the particle
    virtual double           m() const;
    /// The total energy of the particle
    virtual double           e() const;
    /// The true rapidity (y) of the particle
    virtual double           rapidity() const;

    /// Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// The full 4-momentum of the particle
    virtual FourMom_t        p4() const;

    /// Base 4 Momentum type (GenVector)
    typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > GenVecFourMom_t;
    
    /// The full 4-momentum of the particle : GenVector form
    /// Note: can be slower than just p4 for this class; added for consistency
    GenVecFourMom_t          genvecP4() const;

    ///  The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    /// @}


    /// @name Functions implementing additional 4-momentum properties
    /// @{

    /// Get the px momentum component of the particle
    double   px() const;
    /// Get the py momentum component of the particle
    double   py() const;
    /// Get the pz momentum component of the particle
    double   pz() const;
    /// The transverse energy (\f$e_T\f$) of the particle.
    /// Note that this differs from (\f$p_T\f$) for massive particles.
    double   et() const;


    /// Set the 4-vec
    void setP4(      const FourMom_t& vec );
    /// Set the 4-vec
    void setPxPyPzE( double px, double py, double pz, double e );
    /// Set the x-component of the momentum
    void setPx(      double px );
    /// Set the y-component of the momentum
    void setPy(      double py );
    /// Set the z-component of the momentum
    void setPz(      double pz );
    /// Set the energy
    void setE(       double e  );
    /// @}


    /// @name Functions implementing other particly-type properties
    /// @{

    /// Check if this composite particle has its charge set
    bool hasCharge() const;
    /// Return the electric charge.
    float charge() const;
    /// Set the electric charge
    void setCharge( float charge );

    /// Check if this composite particle has its charge set
    bool hasPdgId() const;
    /// Return the PDG identifier
    int pdgId() const;
    /// Set the PDG identifier
    void setPdgId( int pdgID );

    /// @}


    /// @name Functions returning variables that are calculated from 4-momentum
    ///       information from constituents.
    ///       The 4-momentum is calculated as the sum of 4-momenta of all the
    ///       constituent particles whos indices are given in the vector.
    ///       If one of the indices is -1, then missingET will also be used
    ///       with pz=0 and E=sqrt(mpx*mpx + mpy*mpy).
    /// @{

    /// The total 4-momentum
    FourMom_t p4( const std::vector<int>& partIndices ) const;

    /// Get the four-momentum with two indices.
    /// This specialization to the above method is needed since ROOT 6.02 doesn't
    /// yet support C++11 in the TFormula stuff with JIT compilation, see:
    /// https://sft.its.cern.ch/jira/browse/ROOT-5083
    inline FourMom_t p4( int partIndexA, int partIndexB ) const {
      return this->p4( std::vector<int>{partIndexA, partIndexB} );
    }
    /// Get the four-momentum with three indices.
    inline FourMom_t p4( int partIndexA, int partIndexB, int partIndexC ) const {
      return this->p4( std::vector<int>{partIndexA, partIndexB, partIndexC} );
    }
    /// Get the four-momentum with four indices.
    inline FourMom_t p4( int partIndexA, int partIndexB, int partIndexC, int partIndexD ) const {
      return this->p4( std::vector<int>{partIndexA, partIndexB, partIndexC, partIndexD} );
    }

    /// The transverse momentum (\f$p_T\f$)
    double pt( const std::vector<int>& partIndices ) const;
    /// The pseudorapidity (\f$\eta\f$)
    double eta( const std::vector<int>& partIndices ) const;
    /// The azimuthal angle (\f$\phi\f$)
    double phi( const std::vector<int>& partIndices ) const;
    /// The invariant mass
    double m( const std::vector<int>& partIndices ) const;
    /// The total 3-momentum
    double p( const std::vector<int>& partIndices ) const;
    /// The total energy
    double e( const std::vector<int>& partIndices ) const;
    /// The true rapidity (y)
    double rapidity( const std::vector<int>& partIndices ) const;

    /// Get the px momentum component
    double px( const std::vector<int>& partIndices ) const;
    /// Get the py momentum component
    double py( const std::vector<int>& partIndices ) const;
    /// Get the pz momentum component
    double pz( const std::vector<int>& partIndices ) const;
    /// The transverse energy (\f$e_T\f$)
    /// Note that this differs from (\f$p_T\f$) for massive particles.
    double et( const std::vector<int>& partIndices ) const;



    /// Define the enumeration of calculation methods for the transverse mass
    struct MT {
      enum Method {
        DEFAULT = 0
      };
    };

    /// Get the transverse mass.
    /// Specify which calculation method to use as an optional additional argument.
    double mt( const std::vector<int>& partIndices, MT::Method method=MT::DEFAULT ) const;


    /// Get the transverse mass with only one constituent particle.
    /// Specify which calculation method to use as an optional additional argument.
    /// This specialization to the above method is needed since ROOT 6.02 doesn't
    /// yet support C++11 in the TFormula stuff with JIT compilation, see:
    /// https://sft.its.cern.ch/jira/browse/ROOT-5083
    inline double mt( int partIndexA, int partIndexB=-2, MT::Method method=MT::DEFAULT ) const {
      if ( partIndexB==-2 ) {
        return this->mt( std::vector<int>{partIndexA}, method );
      }
      return this->mt( std::vector<int>{partIndexA, partIndexB}, method );
    }

    /// @}




    /// @name Functions returning variables that are calculated from
    ///       information from constituents (not only using their 4-momenta).
    /// @{

    /// Get the weight for this @c xAOD::CompositeParticle by providing a vector of
    /// variable names that will be looked for. These variables, if found, are
    /// assumed to be of type float and to hold a multiplicative weight variable.
    /// If a given variable name is not found for this @c xAOD::CompositeParticle,
    /// then, the whole constituent tree will be searched for iteratively. The
    /// same variable name can be found and used for several constituents. But if
    /// a @c xAOD::CompositeParticle has that variable, the iterative search
    /// will not look into the constituents of that @c xAOD::CompositeParticle.
    /// The second, optional, argument is a list of indices such that one can
    /// restrict the variable name search to only the given constituent particles.
    float weight( const std::vector<std::string>& varNames,
                  const std::vector<int>& partIndices={} ) const;


    /// Get the weight for this @c xAOD::CompositeParticle.
    /// This specialization to the above method is needed since ROOT 6.02 doesn't
    /// yet support C++11 in the TFormula stuff with JIT compilation, see:
    /// https://sft.its.cern.ch/jira/browse/ROOT-5083
    inline float weight( const std::string& varNameA ) const {
      return this->weight( std::vector<std::string>{varNameA} );
    }
    inline float weight( const std::string& varNameA,
                         const std::string& varNameB ) const {
      return this->weight( std::vector<std::string>{varNameA,varNameB} );
    }
    inline float weight( const std::string& varNameA,
                         const std::string& varNameB,
                         const std::string& varNameC ) const {
      return this->weight( std::vector<std::string>{varNameA,varNameB,varNameC} );
    }
    inline float weight( const std::string& varNameA,
                         const std::string& varNameB,
                         const std::string& varNameC,
                         const std::string& varNameD ) const {
      return this->weight( std::vector<std::string>{varNameA,varNameB,varNameC,varNameD} );
    }
    inline float weight( const std::string& varNameA,
                         const std::string& varNameB,
                         const std::string& varNameC,
                         const std::string& varNameD,
                         const std::string& varNameE ) const {
      return this->weight( std::vector<std::string>{varNameA,varNameB,varNameC,varNameD,varNameE} );
    }


  private:
    /// This is a private helper method to calculate the weight
    float weightHelper( const xAOD::IParticle* part,
                        const xAOD::IParticle::ConstAccessor<float>& varAcc ) const;

  public:
    /// @}




    /// @name Functions implementing handling of constituents
    /// @{

    /// @brief Add a particle as an additional constituent.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void addPart( const xAOD::IParticle* part, bool updateFourMom=true );

    /// @brief Add a particle as an additional constituent.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void addPart( const xAOD::IParticleLink& partLink, bool updateFourMom=true );

    /// @brief Add many constituents at the same time
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void addParts( const xAOD::IParticleLinkContainer& partLinkCont,
                   bool updateFourMom=true );

    /// @brief Remove a particle as a constituent from this
    ///        CompostiteParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    ///        If the given IParticle is not already a constituent
    ///        of this CompositeParticle, nothing happens
    void removePart( const xAOD::IParticle* part, bool updateFourMom=true );

    /// @brief Remove a particle as a constituent from this
    ///        CompostiteParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    ///        If the given IParticle is not already a constituent
    ///        of this CompositeParticle, nothing happens
    void removePart( const xAOD::IParticleLink& partLink, bool updateFourMom=true );



    /// @brief Get the MissingET object that is associated with this
    ///        CompositeParticle. If no (valid) MissingET object is associated
    ///        with this CompositeParticle, a null pointer will be returned
    const xAOD::MissingET* missingET() const;

    /// @brief Set the MissingET for this CompositeParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void setMissingET( const xAOD::MissingET* met, bool updateFourMom=true );

    /// @brief Remove the existing MissingET object from this CompositeParticle.
    ///        Note that the 4-momentum of this CompositeParticle
    ///        will be updated accordingly, if updateFourMom=true.
    void removeMissingET( bool updateFourMom=true );


    /// Check if a given xAOD::MissingET object is part of this CompositeParticle
    bool contains( const xAOD::MissingET* met          ) const;
    /// Check if a given xAOD::IParticle is a constituent
    bool contains( const xAOD::IParticle* part         ) const;
    /// Check if a given xAOD::IParticle is a constituent
    bool contains( const xAOD::IParticleLink& partLink ) const;


    /// Number of constituent particles.
    /// Note that MissingET is NOT counted as a constituent in this context
    std::size_t nParts() const;

    /// Number of constituent CompositeParticles.
    std::size_t nCompParts() const;

    /// Number of constituent photons.
    std::size_t nPhotons() const;

    /// Number of constituent truth photons.
    std::size_t nTruthPhotons() const;

    /// Number of constituent electrons.
    std::size_t nElectrons() const;

    /// Number of constituent truth electrons.
    std::size_t nTruthElectrons() const;

    /// Number of constituent muons.
    std::size_t nMuons() const;

    /// Number of constituent truth muons.
    std::size_t nTruthMuons() const;

    /// Number of constituent taus.
    std::size_t nTaus() const;

    /// Number of constituent truth taus.
    std::size_t nTruthTaus() const;

    /// Number of constituent leptons (electrons, muons, and taus).
    std::size_t nLeptons() const;

    /// Number of constituent truth leptons (truth electrons, truth muons, and truth taus).
    std::size_t nTruthLeptons() const;

    /// Number of constituent jets.
    std::size_t nJets() const;

    /// Number of constituent xAOD::TruthParticles.
    std::size_t nTruthParts() const;


    /// Get the constituent IParticle number i
    const xAOD::IParticle* part( std::size_t index = 0 ) const;

    /// Get the constituent IParticle number i as an ElementLink
    const xAOD::IParticleLink& partLink( std::size_t index = 0 ) const;

    /// Get all constituents in one go
    const xAOD::IParticleLinkContainer& partLinks() const;


    /// Method to return a DataVector<T>, e.g., a MuonContainer, for all the
    /// constituent particles, e.g., of type xAOD::Muon, that this CompositeParticle
    /// has. This DataVector be a simple SG::VIEW_ELEMENTS container, i.e., it
    /// does NOT own the elements, e.g., muons, but has simple pointers to them.
    /// However, the user is still required to take care of its memory management
    /// by either recording it to StoreGate (and thus transfering ownership), or
    /// by deleting this view-container when done with it.
    template<typename CONTTYPE>
    ConstDataVector<CONTTYPE>* parts( ) const;



    /// Get a CompositeParticle that is build on the fly from the constituent
    /// particles associated to the indices given (missingET is at index=-1).
    /// If the second list of idices is given, also the "other" particles will
    /// be used in building this CompositeParticle.
    xAOD::CompositeParticle_v1* compPart( const std::vector<int>& partIndices,
                                          const std::vector<int>& otherPartIndices=std::vector<int>{},
                                          bool updateFourMom=true ) const;


    /// Get a CompositeParticle that is build on the fly from the constituent
    /// particles associated to the indices given (missingET is at index=-1).
    /// This specialization to the above method is needed since ROOT 6.02 doesn't
    /// yet support C++11 in the TFormula stuff with JIT compilation, see:
    /// https://sft.its.cern.ch/jira/browse/ROOT-5083
    xAOD::CompositeParticle_v1* compPart( int partIndexA, int partIndexB, int partIndexC=-2,
                                          bool updateFourMom=true ) const {
      if ( partIndexC==-2 ) {
        return this->compPart( std::vector<int>{partIndexA, partIndexB}, std::vector<int>{}, updateFourMom );
      }
      return this->compPart( std::vector<int>{partIndexA, partIndexB, partIndexC}, std::vector<int>{}, updateFourMom );
    }


    /// Get the constituent number i as a CompositeParticle.
    /// If the cast to CompositeParticle fails for a constituent, a null pointer is returned.
    const xAOD::CompositeParticle_v1* compPart( std::size_t index = 0 ) const;

    /// Get the constituent number i as a Photon.
    /// If the cast to Photon fails for a constituent, a null pointer is returned.
    const xAOD::Photon* photon( std::size_t index = 0 ) const;

    /// Get the constituent number i as an Electron.
    /// If the cast to Electron fails for a constituent, a null pointer is returned.
    const xAOD::Electron* electron( std::size_t index = 0 ) const;

    /// Get the constituent number i as an Muon.
    /// If the cast to Muon fails for a constituent, a null pointer is returned.
    const xAOD::Muon* muon( std::size_t index = 0 ) const;

    /// Get the constituent number i as a tau.
    /// If the cast to tau fails for a constituent, a null pointer is returned.
    const xAOD::TauJet* tau( std::size_t index = 0 ) const;

    /// Get the constituent number i as a Jet.
    /// If the cast to Jet fails for a constituent, a null pointer is returned.
    const xAOD::Jet* jet( std::size_t index = 0 ) const;

    /// Get the constituent number i as a TruthParticle.
    /// If the cast to TruthParticle fails for a constituent, a null pointer is returned.
    const xAOD::TruthParticle* truthPart( std::size_t index = 0 ) const;

    /// The constituent iterators are missing for the moment

    /// @}



    /// @name Functions implementing handling of other constituents. These
    ///       other particles are stored separetely from the main constituent
    ///       particles. They will NOT be used to determine the four-momentum
    ///       of this CompositeParticle. And they will NOT be used for determining
    ///       the weight (and other properties) of this CompositeParticle.
    ///       The main idea is that these other particles can be used for keeping
    ///       track of, e.g., a third lepton to veto on, or sub-threshold jets.
    /// @{

    /// @brief Add a particle as an additional other constituent.
    void addOtherPart( const xAOD::IParticle* part );

    /// @brief Add a particle as an additional other constituent.
    void addOtherPart( const xAOD::IParticleLink& partLink );

    /// @brief Add many other constituents at the same time
    void addOtherParts( const xAOD::IParticleLinkContainer& partLinkCont );


    /// @brief Remove a particle as an other constituent from this
    ///        CompostiteParticle.
    void removeOtherPart( const xAOD::IParticle* part );

    /// @brief Remove a particle as an other constituent from this
    ///        CompostiteParticle.
    void removeOtherPart( const xAOD::IParticleLink& partLink );



    /// Check if a given xAOD::IParticle is an other constituent
    bool containsOther( const xAOD::IParticle* part         ) const;
    /// Check if a given xAOD::IParticle is an other constituent
    bool containsOther( const xAOD::IParticleLink& partLink ) const;


    /// Number of other constituent particles.
    std::size_t nOtherParts() const;

    /// Number of other constituent CompositeParticles.
    std::size_t nOtherCompParts() const;

    /// Number of other constituent photons.
    std::size_t nOtherPhotons() const;

    /// Number of other constituent truth photons.
    std::size_t nOtherTruthPhotons() const;

    /// Number of other constituent electrons.
    std::size_t nOtherElectrons() const;

    /// Number of other constituent truth electrons.
    std::size_t nOtherTruthElectrons() const;

    /// Number of other constituent muons.
    std::size_t nOtherMuons() const;

    /// Number of other constituent truth muons.
    std::size_t nOtherTruthMuons() const;

    /// Number of other constituent taus.
    std::size_t nOtherTaus() const;

    /// Number of other constituent truth taus.
    std::size_t nOtherTruthTaus() const;

    /// Number of other constituent leptons (electrons, muons, and taus).
    std::size_t nOtherLeptons() const;

    /// Number of other constituent truth leptons (truth electrons, truth muons, and truth taus).
    std::size_t nOtherTruthLeptons() const;

    /// Number of other constituent jets.
    std::size_t nOtherJets() const;

    /// Number of other constituent xAOD::TruthParticles.
    std::size_t nOtherTruthParts() const;


    /// Get the other constituent IParticle number i
    const xAOD::IParticle* otherPart( std::size_t index = 0 ) const;

    /// Get the other constituent IParticle number i as an ElementLink
    const xAOD::IParticleLink& otherPartLink( std::size_t index = 0 ) const;

    /// Get all other constituents in one go
    const xAOD::IParticleLinkContainer& otherPartLinks() const;


    /// Get the other constituent number i as a CompositeParticle.
    /// If the cast to CompositeParticle fails for a constituent, a null pointer is returned.
    const xAOD::CompositeParticle_v1* otherCompPart( std::size_t index = 0 ) const;

    /// Get the other constituent number i as a Photon.
    /// If the cast to Photon fails for a constituent, a null pointer is returned.
    const xAOD::Photon* otherPhoton( std::size_t index = 0 ) const;

    /// Get the other constituent number i as an Electron.
    /// If the cast to Electron fails for a constituent, a null pointer is returned.
    const xAOD::Electron* otherElectron( std::size_t index = 0 ) const;

    /// Get the other constituent number i as an Muon.
    /// If the cast to Muon fails for a constituent, a null pointer is returned.
    const xAOD::Muon* otherMuon( std::size_t index = 0 ) const;

    /// Get the other constituent number i as a tau.
    /// If the cast to tau fails for a constituent, a null pointer is returned.
    const xAOD::TauJet* otherTau( std::size_t index = 0 ) const;

    /// Get the other constituent number i as a Jet.
    /// If the cast to Jet fails for a constituent, a null pointer is returned.
    const xAOD::Jet* otherJet( std::size_t index = 0 ) const;

    /// Get the other constituent number i as a TruthParticle.
    /// If the cast to TruthParticle fails for a constituent, a null pointer is returned.
    const xAOD::TruthParticle* otherTruthPart( std::size_t index = 0 ) const;

    /// The constituent iterators are missing for the moment

    /// @}


    /// @name Functions implementing read-only access to auxdata (for python).
    ///       This specialization is needed since ROOT 6.02 doesn't
    ///       yet support C++11 in the TFormula stuff with JIT compilation, see:
    ///       https://sft.its.cern.ch/jira/browse/ROOT-5083
    /// @{

    /// Access to get a variable of type bool (actually, it is stored using type char)
    inline bool getBool( const std::string& varName ) const {
      return static_cast<bool>( this->auxdata<char>(varName) );
    }

    /// Access to get a variable of type int
    inline int getInt( const std::string& varName ) const {
      return this->auxdata<int>(varName);
    }

    /// Access to get a variable of type unsigned int
    inline unsigned int getUInt( const std::string& varName ) const {
      return this->auxdata<unsigned int>(varName);
    }

    /// Access to get a variable of type float
    inline float getFloat( const std::string& varName ) const {
      return this->auxdata<float>(varName);
    }

    /// Access to get a variable of type double
    inline double getDouble( const std::string& varName ) const {
      return this->auxdata<double>(varName);
    }


    /// @}


  protected:
    /// @brief Set all constituents in one go.
    ///        Warning: This is meant for usage internal to this class only
    ///        because no addition of the constituent properties will be done!
    void setPartLinks( const xAOD::IParticleLinkContainer& constitLinks );

    /// @brief Set all other constituents in one go.
    void setOtherPartLinks( const xAOD::IParticleLinkContainer& otherPartLinks );


  public:

    /// Function preparing the object to be persistified
    void toPersistent();


  }; // class CompositeParticle_v1


} // namespace xAOD


// Inline methods
inline
const xAOD::CompositeParticle_v1*
xAOD::CompositeParticle_v1::compPart( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::CompositeParticle ){ return 0; }
  return static_cast<const xAOD::CompositeParticle_v1*>(ipart);
}


inline
const xAOD::Photon*
xAOD::CompositeParticle_v1::photon( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::Photon ){ return 0; }
  return static_cast<const xAOD::Photon*>(ipart);
}


inline
const xAOD::Electron*
xAOD::CompositeParticle_v1::electron( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::Electron ){ return 0; }
  return static_cast<const xAOD::Electron*>(ipart);
}


inline
const xAOD::Muon*
xAOD::CompositeParticle_v1::muon( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::Muon ){ return 0; }
  return static_cast<const xAOD::Muon*>(ipart);
}


inline
const xAOD::TauJet*
xAOD::CompositeParticle_v1::tau( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::Tau ){ return 0; }
  return static_cast<const xAOD::TauJet*>(ipart);
}


inline
const xAOD::Jet*
xAOD::CompositeParticle_v1::jet( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::Jet ){ return 0; }
  return static_cast<const xAOD::Jet*>(ipart);
}


inline
const xAOD::TruthParticle*
xAOD::CompositeParticle_v1::truthPart( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->part(index);
  if ( ipart->type() != xAOD::Type::TruthParticle ){ return 0; }
  return static_cast<const xAOD::TruthParticle*>(ipart);
}



inline
const xAOD::CompositeParticle_v1*
xAOD::CompositeParticle_v1::otherCompPart( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::CompositeParticle ){ return 0; }
  return static_cast<const xAOD::CompositeParticle_v1*>(ipart);
}


inline
const xAOD::Photon*
xAOD::CompositeParticle_v1::otherPhoton( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::Photon ){ return 0; }
  return static_cast<const xAOD::Photon*>(ipart);
}


inline
const xAOD::Electron*
xAOD::CompositeParticle_v1::otherElectron( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::Electron ){ return 0; }
  return static_cast<const xAOD::Electron*>(ipart);
}


inline
const xAOD::Muon*
xAOD::CompositeParticle_v1::otherMuon( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::Muon ){ return 0; }
  return static_cast<const xAOD::Muon*>(ipart);
}


inline
const xAOD::TauJet*
xAOD::CompositeParticle_v1::otherTau( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::Tau ){ return 0; }
  return static_cast<const xAOD::TauJet*>(ipart);
}


inline
const xAOD::Jet*
xAOD::CompositeParticle_v1::otherJet( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::Jet ){ return 0; }
  return static_cast<const xAOD::Jet*>(ipart);
}


inline
const xAOD::TruthParticle*
xAOD::CompositeParticle_v1::otherTruthPart( std::size_t index ) const
{
  const xAOD::IParticle* ipart = this->otherPart(index);
  if ( ipart->type() != xAOD::Type::TruthParticle ){ return 0; }
  return static_cast<const xAOD::TruthParticle*>(ipart);
}




#endif // XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLE_V1_H
