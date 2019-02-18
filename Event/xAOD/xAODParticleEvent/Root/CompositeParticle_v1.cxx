/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticle_v1.cxx 677733 2015-06-23 19:31:10Z kkoeneke $

// standard includes
#include <math.h>       /* remainder and M_PI */

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODBase/IParticle.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticle_v1.h"

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

  CompositeParticle_v1::CompositeParticle_v1()
    : IParticle() {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing four-momentum
  //

  static const SG::AuxElement::Accessor< float > accPx( "px" );
  static const SG::AuxElement::Accessor< float > accPy( "py" );
  static const SG::AuxElement::Accessor< float > accPz( "pz" );
  static const SG::AuxElement::Accessor< float > accE(  "e"  );
  static const SG::AuxElement::Accessor< float > chargeAcc( "charge" );
  static const SG::AuxElement::Accessor< int > pdgAcc( "pdgId" );


  double CompositeParticle_v1::pt() const {
    return std::hypot( px(), py() );
  }

  double CompositeParticle_v1::eta() const {
    return p4().Eta();
  }

  double CompositeParticle_v1::phi() const {
    return p4().Phi();
  }

  double CompositeParticle_v1::m() const {
    return p4().M();
  }

  double CompositeParticle_v1::rapidity() const {
    return p4().Rapidity();
  }

  CompositeParticle_v1::FourMom_t CompositeParticle_v1::p4() const {
    if ( accPx.isAvailable(*this) && accPy.isAvailable(*this)
	 && accPz.isAvailable(*this) && accE.isAvailable(*this) ) {
      // We have everything stored with this CompositeParticle. Just use it.
      const double px = static_cast<double>(accPx(*this));
      const double py = static_cast<double>(accPy(*this));
      const double pz = static_cast<double>(accPz(*this));
      const double e  = static_cast<double>(accE(*this));
      return FourMom_t( px, py, pz, e );
    }
    else {
      // Not everything is stored, we need to re-calculate the p4 based on the constituents.
      // Create an empty vector such that the subsequent call will use all constituents.
      const std::vector<int> partIndices;
      return p4( partIndices );
    }
  }

  // note, if calculating from constituent particles, can be slower than p4()
  CompositeParticle_v1::GenVecFourMom_t CompositeParticle_v1::genvecP4() const {
    if ( accPx.isAvailable(*this) && accPy.isAvailable(*this)
	 && accPz.isAvailable(*this) && accE.isAvailable(*this) ) {
      // We have everything stored with this CompositeParticle. Just use it.
      const double px = static_cast<double>(accPx(*this));
      const double py = static_cast<double>(accPy(*this));
      const double pz = static_cast<double>(accPz(*this));
      const double e  = static_cast<double>(accE(*this));
      return GenVecFourMom_t( px, py, pz, e );
    }
    else {
      // Not everything is stored, we need to re-calculate the p4 based on the constituents.
      // Create an empty vector such that the subsequent call will use all constituents.
      const std::vector<int> partIndices;
      FourMom_t fourmom = p4(partIndices);
      return GenVecFourMom_t(fourmom.Px(), fourmom.Py(), fourmom.Pz(), fourmom.E());
    }
  }

  Type::ObjectType CompositeParticle_v1::type() const {
    return Type::CompositeParticle;
  }


  double CompositeParticle_v1::e() const {
    return p4().E();
  }


  double CompositeParticle_v1::px() const {
    return p4().Px();
  }


  double CompositeParticle_v1::py() const {
    return p4().Py();
  }


  double CompositeParticle_v1::pz() const {
    return p4().Pz();
  }


  double CompositeParticle_v1::et() const {
    return p4().Et();
  }


  void CompositeParticle_v1::setP4( const FourMom_t& vec ) {
    accPx( *this ) = static_cast<float>(vec.Px());
    accPy( *this ) = static_cast<float>(vec.Py());
    accPz( *this ) = static_cast<float>(vec.Pz());
    accE( *this )  = static_cast<float>(vec.E());
  }


  void CompositeParticle_v1::setPxPyPzE( double px,
                                         double py,
                                         double pz,
                                         double e ) {
    static const Accessor< float > acc1( "px" );
    acc1( *this ) = (float)px;
    static const Accessor< float > acc2( "py" );
    acc2( *this ) = (float)py;
    static const Accessor< float > acc3( "pz" );
    acc3( *this ) = (float)pz;
    static const Accessor< float > acc4( "e" );
    acc4( *this ) = (float)e;
  }


  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       px, setPx )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       py, setPy )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       pz, setPz )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       e, setE )

  // End: Functions implementing four-momentum
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing other particly-type properties
  //

  bool CompositeParticle_v1::hasCharge() const {
    return chargeAcc.isAvailable( *this );
  }


  float CompositeParticle_v1::charge() const {
    if ( chargeAcc.isAvailable(*this) ) {
      // If we have stored the charge with this CompositeParticle, just use it.
      return chargeAcc(*this);
    }
    else {
      // Otherwise, try to re-calculate it from the constituents
      float charge = 0.0;
      // Sum up the charges of all constituents, if all have it
      std::size_t nParts = this->nParts();
      for ( std::size_t i=0; i<nParts; ++i ) {
        const xAOD::IParticle* part = this->part(i);
        if (!part) {
          throw std::runtime_error("Got a zero pointer to an xAOD::IParticle!");
        }
        if ( chargeAcc.isAvailable(*part) ) {
          charge += chargeAcc(*part);
        }
        else if ( part->type() != xAOD::Type::CompositeParticle ) {
          // We have an electron
          const xAOD::CompositeParticle_v1* compPart = static_cast<const xAOD::CompositeParticle_v1*>(part);
          charge += compPart->charge();
        }
      } // End: loop over all constituent particles
      return charge;
    }
  }


  void CompositeParticle_v1::setCharge( float charge ) {
    chargeAcc(*this) = charge;
    return;
  }


  bool CompositeParticle_v1::hasPdgId() const {
    return pdgAcc.isAvailable( *this );
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CompositeParticle_v1, int,
                                        pdgId, setPdgId )


  // End: Functions implementing other particly-type properties
  /////////////////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions returning variables that are calculated from 4-momentum
  //     information from constituents.
  //

  CompositeParticle_v1::FourMom_t
  CompositeParticle_v1::p4( const std::vector<int>& partIndices ) const {
    // We want a static here such that this TLorentzVector is only once in
    // memory. We anyhow have to reset it every time this function is called.
    CompositeParticle_v1::FourMom_t fourMom;
    fourMom.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
    // If the given vector of indices is an empty vector, run over all particles
    if ( partIndices.empty() ) {
      // Sum up the 4-momenta of all constituents
      std::size_t nParts = this->nParts();
      for ( std::size_t i=0; i<nParts; ++i ) {
        const xAOD::IParticle* part = this->part(i);
        if (!part) {
          throw std::runtime_error("Got a zero pointer to an xAOD::IParticle!");
        }
        fourMom += part->p4();
      }
      // Also add the 4-momentum of the missingET
      const xAOD::MissingET* met = this->missingET();
      if (met) {
        double px = met->mpx();
        double py = met->mpy();
        fourMom += CompositeParticle_v1::FourMom_t( px, py, 0.0, std::hypot(px, py) );
      }
    }
    else {
      for ( int i : partIndices ) {
        if ( i == -1 ) {
          const xAOD::MissingET* met = this->missingET();
          double px = met->mpx();
          double py = met->mpy();
          fourMom += CompositeParticle_v1::FourMom_t( px, py, 0.0, std::hypot(px, py) );
        }
        else if ( i < 0 ) {
          throw std::runtime_error("Got a negative index which is not -1");
        }
        else {
          const xAOD::IParticle* part = this->part(i);
          if ( !part ) {
            throw std::runtime_error("Got a zero pointer to an xAOD::IParticle");
          }
          fourMom += part->p4();
        }
      }
    }
    return fourMom;
  }


  double CompositeParticle_v1::pt( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Pt();
  }

  double CompositeParticle_v1::eta( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Eta();
  }

  double CompositeParticle_v1::phi( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Phi();
  }

  double CompositeParticle_v1::m( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).M();
  }

  double CompositeParticle_v1::p( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).P();
  }

  double CompositeParticle_v1::e( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).E();
  }

  double CompositeParticle_v1::rapidity( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Rapidity();
  }

  double CompositeParticle_v1::px( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Px();
  }

  double CompositeParticle_v1::py( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Py();
  }

  double CompositeParticle_v1::pz( const std::vector<int>& partIndices ) const {
    return (this->p4(partIndices)).Pz();
  }

  double CompositeParticle_v1::et( const std::vector<int>& partIndices ) const{
    return (this->p4(partIndices)).Et();
  }



  // Get the transverse mass.
  // Specify which calculation method to use as an optional additional argument.
  double CompositeParticle_v1::mt( const std::vector<int>& partIndices, MT::Method method ) const
  {
    // Use the different calculation methods, depending on the given input
    if ( method == xAOD::CompositeParticle_v1::MT::DEFAULT ) {
      // Calculate m_T according to the ATLAS Higgs->WW publication
      // from 2014/2015 (http://arxiv.org/pdf/1412.2641v1.pdf, page 3)

      // Get all the visible constituents and sum up their 4-momenta.
      // If the user provides as one of these indices a -1, then missingET will
      // also be used... THIS WILL GIVE WRONG RESULTS!
      xAOD::CompositeParticle_v1::FourMom_t fourMom( this->p4(partIndices) );

      // Get the missing e_T
      const xAOD::MissingET* metObj = this->missingET();
      if ( !metObj ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got a zero pointer to an xAOD::MissingET when calling mt");
      }
      const double mpx( metObj->mpx() );
      const double mpy( metObj->mpy() );
      const double met( metObj->met() );

      // Now, actually calculate the result (in two parts)
      const double px( fourMom.Px() );
      const double py( fourMom.Py() );
      const double part2 = (px+mpx)*(px+mpx) + (py+mpy)*(py+mpy);

      double mll2( fourMom.M2() );
      // This is needed for rare cases when the TLorentzVector.M2 returns negative values
      mll2 = mll2 < 0.0 ? -mll2 : mll2;
      const double etll( std::sqrt( (px*px + py*py) + mll2 ) );
      const double part1( (etll+met)*(etll+met) );

      // One last sanity check
      if ( part1 < part2 ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got an invalid mt calculation");
      }
      return std::sqrt( part1 - part2 );
    }

    return 0.0;
  }




  // End: Functions returning variables that are calculated from 4-momentum
  //      information from constituents.
  /////////////////////////////////////////////////////////////////////////////




  /////////////////////////////////////////////////////////////////////////////
  // Begin: Functions returning variables that are calculated from
  //        information from constituents (not only using their 4-momenta).

  float CompositeParticle_v1::weight( const std::vector<std::string>& varNames,
                                      const std::vector<int>& partIndices ) const {
    // Default return value
    float weight(1.0);
    // Iterate over each variable name
    for ( const std::string& varName : varNames ) {
      // Protect against empty strings
      if ( varName.empty() ) { continue; }
      const xAOD::IParticle::ConstAccessor<float> varAcc(varName);
      // If the given vector of indices is an empty vector, run over all particles
      if ( partIndices.empty() ) {
        weight *= this->weightHelper( this, varAcc );
      }
      else {
        // If we have specific indices given, we will only look there
        for ( int i : partIndices ) {
          if ( i == -1 ) {
            const xAOD::MissingET* met = this->missingET();
            if ( met && varAcc.isAvailable(*met) ) { weight *= varAcc(*met); }
            continue;
          }
          else if ( i < 0 ) {
            throw std::runtime_error("Got a negative index which is not -1");
          }
          else {
            const xAOD::IParticle* part = this->part(i);
            weight *= this->weightHelper( part, varAcc );
          }
        }
      } // End: if/else partIndices.empty()
    } // End: Loop over variable names
    return weight;
  }



  // This is a private helper method
  float CompositeParticle_v1::weightHelper( const xAOD::IParticle* part,
                                            const xAOD::IParticle::ConstAccessor<float>& varAcc ) const {
    if (!part) {
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle!");
    }
    if ( varAcc.isAvailable(*part) ) {
      return varAcc(*part);
    }
    if ( part->type() != xAOD::Type::CompositeParticle ) {
      // We didn't find the weight variable for this particle and we cannot
      // iterate further down into the constituent tree (as it ends here).
      return 1.0;
    }
    // We have a CompositeParticle, but it doesn't have its own weight variable
    // of the given name. Thus, we search in all of its constituents for the weight.
    const xAOD::CompositeParticle_v1* compPart = static_cast<const xAOD::CompositeParticle_v1*>(part);
    float weight(1.0);
    std::size_t nParts = compPart->nParts();
    for ( std::size_t i=0; i<nParts; ++i ) {
      const xAOD::IParticle* part2 = compPart->part(i);
      weight *= this->weightHelper( part2, varAcc );
    }
    const xAOD::MissingET* met = compPart->missingET();
    if ( met && varAcc.isAvailable(*met) ) { weight *= varAcc(*met); }
    return weight;
  }


  // End: Functions returning variables that are calculated from
  //       information from constituents (not only using their 4-momenta).
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing handling of constituents
  //

  // Get the accessor for the vector of element links to the constituents
  static const SG::AuxElement::Accessor< xAOD::IParticleLinkContainer >
              partLinksAcc( "partLinks" );



  void
  CompositeParticle_v1::addPart( const xAOD::IParticle* part, bool updateFourMom ) {
    // Check if we have a valid pointer
    if ( !part ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when adding constituent!");
      return;
    }
    xAOD::IParticleLinkContainer& constitLinks = partLinksAcc( *this );

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      this->setP4( this->p4() + part->p4() );
      // Cannot add charge automatically as this is NOT part of the xAOD::IParticle interface
      //this->setCharge( this->charge() + (*partLink)->charge() );
    }

    // Add the given ElementLink as a constituent of this composite particle
    const std::size_t partIdx = part->index();
    const xAOD::IParticleContainer* partCont =
      static_cast<const xAOD::IParticleContainer*>(part->container());
    constitLinks.push_back( IParticleLink( *partCont, partIdx ) );
    return;
  }



  void
  CompositeParticle_v1::addPart( const IParticleLink& partLink, bool updateFourMom ) {
    // Check if we have a valid ElementLink
    if ( ! partLink.isValid() ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid ElementLink when adding constituent!");
    }
    xAOD::IParticleLinkContainer& constitLinks = partLinksAcc( *this );

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      this->setP4( this->p4() + (*partLink)->p4() );
      // Cannot add charge automatically as this is NOT part of the xAOD::IParticle interface
      //this->setCharge( this->charge() + (*partLink)->charge() );
    }

    // Add the given ElementLink as a constituent of this composite particle
    constitLinks.push_back( partLink );
    return;
  }


  void
  CompositeParticle_v1::addParts( const xAOD::IParticleLinkContainer& partLinkCont,
                                  bool updateFourMom  ) {
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = partLinkCont.begin(),
            constitItrEnd = partLinkCont.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      this->addPart( currentLink, updateFourMom );
    }
    return;
  }



  void CompositeParticle_v1::removePart( const xAOD::IParticle* part,
                                         bool updateFourMom ) {
    // Check if we have a valid pointer
    if ( !part ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when adding constituent!");
    }

    // Add the ElementLink of the given particle and pass it on
    const std::size_t partIdx = part->index();
    const xAOD::IParticleContainer* partCont =
      static_cast<const xAOD::IParticleContainer*>(part->container());
    this->removePart( IParticleLink( *partCont, partIdx ), updateFourMom );
    return;
  }



  void
  CompositeParticle_v1::removePart( const xAOD::IParticleLink& partLink,
                                    bool updateFourMom ) {
    // Check if we have a valid ElementLink
    if ( ! partLink.isValid() ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid ElementLink when adding constituent!");
    }
    xAOD::IParticleLinkContainer& constitLinks = partLinksAcc( *this );

    // Find the constituent particle that should be removed
    for ( xAOD::IParticleLinkContainer::iterator
            constitItr = constitLinks.begin(),
            constitItrEnd = constitLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( partLink == currentLink ) {
        // Update the four-momentum of this composite particle, if requested
        if ( updateFourMom ) {
          this->setP4( this->p4() - (*partLink)->p4() );
          // Cannot add charge automatically as this is NOT part of the xAOD::IParticle interface
          //this->setCharge( this->charge() - (*partLink)->charge() );
        }

        // Remove the found link
        constitLinks.erase( constitItr );
        return;
      }
    }
    return;
  }

  // Get the accessor for the element link to the MissingET object
  static const SG::AuxElement::Accessor< ElementLink<xAOD::MissingETContainer> >
          metLinkAcc( "missingETLink" );


  const xAOD::MissingET*
  CompositeParticle_v1::missingET() const {
    if ( !(metLinkAcc.isAvailable(*this)) ) {
      return 0;
    }
    const ElementLink<xAOD::MissingETContainer>& metLink = metLinkAcc(*this);
    // Check if we have a valid ElementLink
    if ( ! metLink.isValid() ) {
      return 0;
    }
    return *metLink;
  }



  void
  CompositeParticle_v1::setMissingET( const xAOD::MissingET* met,
                                      bool updateFourMom ) {
    // Check if we have a valid pointer
    if ( !met ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to xAOD::MissingET when adding it!");
    }

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      xAOD::IParticle::FourMom_t metP4;
      double metpx = met->mpx();
      double metpy = met->mpy();
      metP4.SetPxPyPzE( metpx, metpy, 0.0, std::sqrt(metpx*metpx + metpy*metpy) );
      this->setP4( this->p4() + metP4 );
    }

    // Add the given ElementLink as a constituent of this composite particle
    const std::size_t metIdx = met->index();
    const xAOD::MissingETContainer* metCont =
      static_cast<const xAOD::MissingETContainer*>(met->container());
    metLinkAcc(*this) = ElementLink<xAOD::MissingETContainer>( *metCont, metIdx );
    return;
  }



  void
  CompositeParticle_v1::removeMissingET( bool updateFourMom ) {
    const xAOD::MissingET* met = this->missingET();
    if ( !met ) {
      // There is no valid MissingET object assigned... nothing can done
      return;
    }

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      xAOD::IParticle::FourMom_t metP4;
      double metpx = met->mpx();
      double metpy = met->mpy();
      metP4.SetPxPyPzE( metpx, metpy, 0.0, std::sqrt(metpx*metpx + metpy*metpy) );
      this->setP4( this->p4() - metP4 );
    }
    // Assign an invalid ElementLink here
    metLinkAcc(*this) = ElementLink<xAOD::MissingETContainer>();
    return;
  }




  bool
  CompositeParticle_v1::contains( const xAOD::MissingET* met ) const {
    const ElementLink<xAOD::MissingETContainer>& metLink = metLinkAcc(*this);
    // Check if we have a valid ElementLink. If not, return false. This is
    // because the given met that we check against obviously exists, meaning
    // that if it would be the same, the ElementLink should have been valid.
    if ( ! metLink.isValid() ) {
      return false;
    }
    const xAOD::MissingET* thisMet = *metLink;

    // Check if the pointers are equal
    if ( thisMet == met ) {
      return true;
    }

    // Use the MissingET == operator
    if ( (*thisMet) == (*met) ) {
      return true;
    }

    // Otherwise, return false
    return false;
  }



  bool
  CompositeParticle_v1::contains( const xAOD::IParticle* part ) const {
    const xAOD::IParticleLinkContainer& constitLinks = this->partLinks();
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = constitLinks.begin(),
            constitItrEnd = constitLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( ! currentLink.isValid() ) continue;
      const xAOD::IParticle* currentPart = *currentLink;
      if ( part == currentPart ) {
        return true;
      }
    }
    return false;
  }


  bool
  CompositeParticle_v1::contains( const xAOD::IParticleLink& partLink ) const {
    const xAOD::IParticleLinkContainer& constitLinks = this->partLinks();
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = constitLinks.begin(),
            constitItrEnd = constitLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( partLink == currentLink ) {
        return true;
      }
    }
    return false;
  }


  std::size_t CompositeParticle_v1::nParts() const {
    if( partLinksAcc.isAvailable( *this ) ) {
      return partLinksAcc( *this ).size();
    }
    return 0;
  }



// Define a pre-processor macro for the implementations of all these methods
#define NUM_PARTS( FUNCNAME, OBJTYPE )                                                      \
  std::size_t CompositeParticle_v1::FUNCNAME() const {                                      \
    std::size_t n(0);                                                                       \
    std::size_t nParts = this->nParts();                                                    \
    for ( std::size_t i=0; i<nParts; ++i ) {                                                \
      const xAOD::IParticle* part = this->part(i);                                          \
      if (!part) { throw std::runtime_error("Got a zero pointer to an xAOD::IParticle!"); } \
      if ( part->type() == xAOD::Type::OBJTYPE ) { n += 1; }                                \
    }                                                                                       \
    return n;                                                                               \
  }

  NUM_PARTS(nCompParts,CompositeParticle)
  NUM_PARTS(nPhotons,Photon)
  NUM_PARTS(nElectrons,Electron)
  NUM_PARTS(nMuons,Muon)
  NUM_PARTS(nTaus,Tau)

  std::size_t CompositeParticle_v1::nLeptons() const {
    return this->nElectrons() + this->nMuons() + this->nTaus();
  }

  NUM_PARTS(nJets,Jet)
  NUM_PARTS(nTruthParts,TruthParticle)





  const xAOD::IParticle*
  CompositeParticle_v1::part( std::size_t index ) const {
    if ( index >= this->nParts() ) {
       return 0;
    }
    const xAOD::IParticleLink & constitLink = partLink( index );
    if ( ! constitLink.isValid() ) {
       return 0;
    }
    return *constitLink;
  }


  const xAOD::IParticleLink&
  CompositeParticle_v1::partLink( std::size_t index ) const {
    if ( index >= this->nParts() ) {
       static const xAOD::IParticleLink dummy;
       return dummy;
    }
    return partLinks()[ index ];
  }



  AUXSTORE_OBJECT_SETTER_AND_GETTER( CompositeParticle_v1,
                                     xAOD::IParticleLinkContainer,
                                     partLinks,
                                     setPartLinks )


  template<typename CONTTYPE>
  ConstDataVector<CONTTYPE>* CompositeParticle_v1::parts() const {
    // Create the return object
    ConstDataVector<CONTTYPE>* outCont = new ConstDataVector<CONTTYPE>(SG::VIEW_ELEMENTS);

    std::size_t nParts = this->nParts();
    for ( std::size_t i=0; i<nParts; ++i ) {
      const xAOD::IParticle* part = this->part(i);
      if (!part) {
        delete outCont;
        throw std::runtime_error("Got a zero pointer to an xAOD::IParticle!");
      }
      typename ConstDataVector<CONTTYPE>::const_pointer typePart
        = dynamic_cast<typename ConstDataVector<CONTTYPE>::const_pointer>(part);
      if (typePart) {
        outCont->push_back(typePart);
      }
    }
    return outCont;
  }



  xAOD::CompositeParticle_v1*
  CompositeParticle_v1::compPart( const std::vector<int>& partIndices,
                                  const std::vector<int>& otherPartIndices,
                                  bool updateFourMom ) const {
    xAOD::CompositeParticle_v1* compPart = new CompositeParticle_v1();
    compPart->makePrivateStore();
    for ( int i : partIndices ) {
      if ( i == -1 ) {
        compPart->setMissingET( this->missingET(), updateFourMom );
      }
      else if ( i >= 0 ) {
        const xAOD::IParticleLink& pLink = this->partLink( static_cast<std::size_t>(i) );
        compPart->addPart( pLink, updateFourMom );
      }
      else {
        throw std::runtime_error("Got a non-valid index");
      }
    }
    for ( int i : otherPartIndices ) {
      if ( i >= 0 ) {
        const xAOD::IParticleLink& pLink = this->otherPartLink( static_cast<std::size_t>(i) );
        compPart->addPart( pLink, updateFourMom );
      }
      else {
        throw std::runtime_error("Got a non-valid index for an other particle");
      }
    }
    return compPart;
  }




  // End: Functions implementing handling of constituents
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing handling of other constituents
  //

  // Get the accessor for the vector of element links to the other constituents
  static const SG::AuxElement::Accessor< xAOD::IParticleLinkContainer >
              otherPartLinksAcc( "otherPartLinks" );



  void
  CompositeParticle_v1::addOtherPart( const xAOD::IParticle* part ) {
    // Check if we have a valid pointer
    if ( !part ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when adding constituent!");
    }
    xAOD::IParticleLinkContainer& otherPartLinks = otherPartLinksAcc( *this );

    // Add the given ElementLink as a constituent of this composite particle
    const std::size_t partIdx = part->index();
    const xAOD::IParticleContainer* partCont =
      static_cast<const xAOD::IParticleContainer*>(part->container());
    otherPartLinks.push_back( IParticleLink( *partCont, partIdx ) );
    return;
  }



  void
  CompositeParticle_v1::addOtherPart( const IParticleLink& partLink ) {
    // Check if we have a valid ElementLink
    if ( ! partLink.isValid() ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid ElementLink when adding constituent!");
    }
    xAOD::IParticleLinkContainer& otherPartLinks = otherPartLinksAcc( *this );

    // Add the given ElementLink as a constituent of this composite particle
    otherPartLinks.push_back( partLink );
    return;
  }


  void
  CompositeParticle_v1::addOtherParts( const xAOD::IParticleLinkContainer& partLinkCont ) {
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = partLinkCont.begin(),
            constitItrEnd = partLinkCont.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      this->addOtherPart( currentLink );
    }
    return;
  }



  void CompositeParticle_v1::removeOtherPart( const xAOD::IParticle* part ) {
    // Check if we have a valid pointer
    if ( !part ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when adding constituent!");
    }

    // Add the ElementLink of the given particle and pass it on
    const std::size_t partIdx = part->index();
    const xAOD::IParticleContainer* partCont =
      static_cast<const xAOD::IParticleContainer*>(part->container());
    this->removeOtherPart( IParticleLink( *partCont, partIdx ) );
    return;
  }



  void
  CompositeParticle_v1::removeOtherPart( const xAOD::IParticleLink& partLink ) {
    // Check if we have a valid ElementLink
    if ( ! partLink.isValid() ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid ElementLink when adding constituent!");
    }
    xAOD::IParticleLinkContainer& otherPartLinks = otherPartLinksAcc( *this );

    // Find the constituent particle that should be removed
    for ( xAOD::IParticleLinkContainer::iterator
            constitItr = otherPartLinks.begin(),
            constitItrEnd = otherPartLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( partLink == currentLink ) {
        // Remove the found link
        otherPartLinks.erase( constitItr );
        return;
      }
    }
    return;
  }



  bool
  CompositeParticle_v1::containsOther( const xAOD::IParticle* part ) const {
    const xAOD::IParticleLinkContainer& otherPartLinks = this->otherPartLinks();
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = otherPartLinks.begin(),
            constitItrEnd = otherPartLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( ! currentLink.isValid() ) continue;
      const xAOD::IParticle* currentPart = *currentLink;
      if ( part == currentPart ) {
        return true;
      }
    }
    return false;
  }


  bool
  CompositeParticle_v1::containsOther( const xAOD::IParticleLink& partLink ) const {
    const xAOD::IParticleLinkContainer& otherPartLinks = this->otherPartLinks();
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = otherPartLinks.begin(),
            constitItrEnd = otherPartLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( partLink == currentLink ) {
        return true;
      }
    }
    return false;
  }


  std::size_t CompositeParticle_v1::nOtherParts() const {
    if( otherPartLinksAcc.isAvailable( *this ) ) {
      return otherPartLinksAcc( *this ).size();
    }
    return 0;
  }



// Define a pre-processor macro for the implementations of all these methods
#define NUM_OTHERPARTS( FUNCNAME, OBJTYPE )                                                 \
  std::size_t CompositeParticle_v1::FUNCNAME() const {                                      \
    std::size_t n(0);                                                                       \
    std::size_t nParts = this->nOtherParts();                                               \
    for ( std::size_t i=0; i<nParts; ++i ) {                                                \
      const xAOD::IParticle* part = this->otherPart(i);                                     \
      if (!part) { throw std::runtime_error("Got a zero pointer to an xAOD::IParticle!"); } \
      if ( part->type() == xAOD::Type::OBJTYPE ) { n += 1; }                                \
    }                                                                                       \
    return n;                                                                               \
  }

  NUM_OTHERPARTS(nOtherCompParts,CompositeParticle)
  NUM_OTHERPARTS(nOtherPhotons,Photon)
  NUM_OTHERPARTS(nOtherElectrons,Electron)
  NUM_OTHERPARTS(nOtherMuons,Muon)
  NUM_OTHERPARTS(nOtherTaus,Tau)

  std::size_t CompositeParticle_v1::nOtherLeptons() const {
    return this->nOtherElectrons() + this->nOtherMuons() + this->nOtherTaus();
  }

  NUM_OTHERPARTS(nOtherJets,Jet)
  NUM_OTHERPARTS(nOtherTruthParts,TruthParticle)




  const xAOD::IParticle*
  CompositeParticle_v1::otherPart( std::size_t index ) const {
    if ( index >= this->nOtherParts() ) {
       return 0;
    }
    const xAOD::IParticleLink & otherPartLink = this->otherPartLink( index );
    if ( ! otherPartLink.isValid() ) {
       return 0;
    }
    return *otherPartLink;
  }


  const xAOD::IParticleLink&
  CompositeParticle_v1::otherPartLink( std::size_t index ) const {
    if ( index >= this->nOtherParts() ) {
       static const xAOD::IParticleLink dummy;
       return dummy;
    }
    return otherPartLinks()[ index ];
  }



  AUXSTORE_OBJECT_SETTER_AND_GETTER( CompositeParticle_v1,
                                     xAOD::IParticleLinkContainer,
                                     otherPartLinks,
                                     setOtherPartLinks )

  // End: Functions implementing handling of other constituents
  /////////////////////////////////////////////////////////////////////////////






  void CompositeParticle_v1::toPersistent() {
    if( partLinksAcc.isAvailableWritable( *this ) ) {
      for ( xAOD::IParticleLinkContainer::iterator
              itr = partLinksAcc( *this ).begin(),
              itrEnd = partLinksAcc( *this ).end();
            itr != itrEnd;
            ++itr ) {
        itr->toPersistent();
      }
    }
    return;
  }


} // namespace xAOD
