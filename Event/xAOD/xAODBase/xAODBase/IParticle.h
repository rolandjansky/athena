// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODBASE_IPARTICLE_H
#define XAODBASE_IPARTICLE_H

// ROOT include(s):
#include <TLorentzVector.h>

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "ObjectType.h"


/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   
    /// Class providing the definition of the 4-vector interface
   ///
   /// All particle-like classes in the xAOD EDM inherit from this simple
   /// interface class to make it simple to write generic analysis code
   /// for the objects.
   ///
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   /// @author Till Eifert <Till.Eifert@cern.ch>
   /// @author Markus Elsing <Markus.Elsing@cern.ch>
   /// @author Dag Gillberg <Dag.Gillberg@cern.ch>
   /// @author Karsten Koeneke <karstenkoeneke@gmail.com>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Edward Moyse <Edward.Moyse@cern.ch>
   ///
   /// $Revision: 604340 $
   /// $Date: 2014-07-01 06:04:52 +0200 (Tue, 01 Jul 2014) $
   ///
   class IParticle : public SG::AuxElement {

   public:
      /// Virtual destructor, to make vtable happy...
      virtual ~IParticle() {}


      /// @name Functions describing the 4-momentum of the object
      /// @{

      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double           pt() const = 0;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double           eta() const = 0;
      /// The azimuthal angle (\f$\phi\f$) of the particle
      virtual double           phi() const = 0;
      /// The invariant mass of the particle
      virtual double           m() const = 0;
      /// The total energy of the particle
      virtual double           e() const = 0;
      /// The true rapidity (y) of the particle
      virtual double           rapidity() const = 0;

      /// Definition of the 4-momentum type
      typedef TLorentzVector FourMom_t;

      /// The full 4-momentum of the particle
      virtual FourMom_t        p4() const = 0;

      /// @}


      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const = 0;


      /// @name Functions for getting and setting user properties
      /// @{

      /// Fetch an aux data variable, as a non-const reference
      ///
      /// This function provides an easy way for users to decorate objects
      /// with auxiliary data.
      ///
      /// Take note that this function is slow. Should not be used inside
      /// time-critical code.
      ///
      /// @param name Name of the aux variable
      /// @param clsname The name of the associated class.  May be blank
      /// @returns A modifyable reference to the decoration
      ///
      template< class T >
      T& auxdata( const std::string& name,
                  const std::string& clsname = "" ) {

         return SG::AuxElement::auxdata< T >( name, clsname );
      }

      /// Fetch an aux data variable, as a const reference
      ///
      /// This function provides an easy way for users to retrieve auxiliary
      /// decorations from an object.
      ///
      /// Take note that this function is slow. Should not be used inside
      /// time-critical code.
      ///
      /// @param name Name of the aux variable
      /// @param clsname The name of the associated class.  May be blank
      /// @returns A constant reference to the decoration
      ///
      template< class T >
      const T& auxdata( const std::string& name,
                        const std::string& clsname = "" ) const {

         return SG::AuxElement::auxdata< T >( name, clsname );
      }

      /// Check if a user property is available for reading or not
      ///
      /// This function should be used to check if a user property which
      /// may or may not exist, is set on the object.
      ///
      /// @param name Name of the auxiliary variable
      /// @param clsname The name of the associated class.  May be blank
      /// @returns Whether the decoration exists or not
      ///
      template< class T >
      bool isAvailable( const std::string& name,
                        const std::string& clsname = "" ) const {

         return SG::AuxElement::isAvailable< T >( name, clsname );
      }

      /// Check if a user property is available for writing or not
      ///
      /// This function can be used to check whether it will be possible to
      /// set a user property on the object.
      ///
      /// @param name Name of the auxiliary variable
      /// @param clsname The name of the associated class.  May be blank
      /// @returns Whether the decoration is possible to set
      ///
      template< class T >
      bool isAvailableWritable( const std::string& name,
                                const std::string& clsname = "" ) const {

         return SG::AuxElement::isAvailableWritable< T >( name, clsname );
      }

      /// @}


   protected:
      // Hide some functions from the regular xAOD users
      using SG::AuxElement::getConstStore;
      using SG::AuxElement::getStore;

      // Hide the Accessor class from the regular xAOD users
      using SG::AuxElement::Accessor;

   }; // class IParticle

} // namespace xAOD

#ifndef XAOD_STANDALONE
#include "SGTools/BaseInfo.h"
SG_BASE (xAOD::IParticle, SG::AuxElement);
#endif // not XAOD_STANDALONE

#endif // XAODBASE_IPARTICLE_H
