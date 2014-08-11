// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
#define XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H

// System include(s):
#include <string>
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class used to describe composite objects in the HLT
   ///
   /// This is a generic class for describing the output of high-level
   /// HLT algorithms that combine lower-level objects into one.
   ///
   /// @author Tomasz Bold <Tomasz.Bold@cern.ch>
   /// @author Camille Belanger-Champagne <Camille.Belanger-Champagne@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision$
   /// $Date$
   ///
   class TrigComposite_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TrigComposite_v1();

      /// @name Basic properties
      /// @{

      /// Get a human-readable name for the object
      const std::string& name() const;
      /// Set a human-readable name for the object
      void setName( const std::string& name );

      /// @}

      /// @name Functions for accessing basic properties on the object
      /// @{

      /// Check if a given type of detail is available
      template< typename TYPE >
      bool hasDetail( const std::string& name );

      /// Set an integer detail on the object
      bool setDetail( const std::string& name, int value );
      /// Set a floating point detail on the object
      bool setDetail( const std::string& name, float value );
      /// Set a vector<int> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< int >& value );
      /// Set a vector<float> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< float >& value );

      /// Get an integer detail from the object
      bool getDetail( const std::string& name, int& value ) const;
      /// Get a floating point detail from the object
      bool getDetail( const std::string& name, float& value ) const;
      /// Get a vector<int> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< int >& value ) const;
      /// Get a vector<float> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< float >& value ) const;

      /// @}

   }; // class TrigComposite_v1

} // namespace xAOD

// Include the template implementation:
#include "TrigComposite_v1.icc"

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
